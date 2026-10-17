"""Acceptance checks shared by the generator, the compiler gate and the
model stage.

Every check returns a list of problems; an empty list means the artifact is
acceptable.  They are deliberately independent of the model: the same
functions validate generated output and model output, so there is one
definition of "correct".
"""

import re

from . import config, oracle

_SPEC_ROW_RE = re.compile(r"^\s*\{ ")
_CALLBACK_DEF_RE = re.compile(
    r"^action_callback_(\w+) \(GschemAction \*action,", re.MULTILINE)
_EXTERN_RE = re.compile(r"^extern GschemAction \*action_(\w+);", re.MULTILINE)
_CALLBACK_DECL_RE = re.compile(r"^void action_callback_(\w+) \(GschemAction",
                               re.MULTILINE)


def count_spec_rows(source):
    """Number of rows in the action_specs table.

    Rows are counted by their marker pair: every row ends with
    ``action_callback_<cid>, &action_<cid> },`` because each spec points at
    both its callback and its own global slot.
    """
    return len(re.findall(r"action_callback_\w+, &action_\w+ \},", source))


def check_arity(source, header, expected, entries=None):
    """Verify the generated pair declares and defines all actions.

    The three counts that must agree are: table rows, callback definitions
    in the .c, and both externs and callback declarations in the .h.

    Counts alone would accept a file where one row names the wrong callback
    and the totals still balance, so when ``entries`` is supplied the set of
    names is compared against the inventory as well.
    """
    problems = []
    rows = count_spec_rows(source)
    definitions = _CALLBACK_DEF_RE.findall(source)
    externs = _EXTERN_RE.findall(header)
    declarations = _CALLBACK_DECL_RE.findall(header)

    for label, actual in (("spec rows", rows),
                          ("callback definitions", len(definitions)),
                          ("extern pointers", len(externs)),
                          ("callback declarations", len(declarations))):
        if actual != expected:
            problems.append(f"{label}: {actual}, expected {expected}")

    unique_defs = set(definitions)
    for label, names in (("extern pointers", externs),
                         ("callback declarations", declarations)):
        missing = unique_defs - set(names)
        if missing:
            problems.append(f"{label} missing for: {sorted(missing)[:5]}")

    if len(unique_defs) != len(definitions):
        problems.append("duplicate callback definitions")

    if entries is not None:
        wanted = {e["cid"] for e in entries}
        for label, names in (("definitions", unique_defs),
                             ("externs", set(externs)),
                             ("declarations", set(declarations))):
            unexpected = names - wanted
            absent = wanted - names
            if unexpected:
                problems.append(f"{label} for unknown cids: "
                                f"{sorted(unexpected)[:5]}")
            if absent:
                problems.append(f"{label} missing cids: {sorted(absent)[:5]}")
    return problems


def extract_bodies(source):
    """Return {cid: body_text} for every callback defined in a source file."""
    bodies = {}
    for match in re.finditer(
            r"^action_callback_(\w+) \(GschemAction \*action,\s*\n"
            r"\s*GschemToplevel \*w_current\)\n\{(.*?)\n\}\n",
            source, re.DOTALL | re.MULTILINE):
        bodies[match.group(1)] = match.group(2)
    return bodies


def check_bodies(source, entries):
    """Verify each callback body still matches the oracle semantics.

    The comparison is on normalized text after the documented mechanical
    rewrites (see :func:`generate.rewrite_body`), so a reformatted but
    semantically identical body passes while a changed call fails.
    """
    from . import generate

    problems = []
    actual = extract_bodies(source)
    for entry in entries:
        cid = entry["cid"]
        if cid not in actual:
            problems.append(f"{cid}: callback body not found")
            continue
        expected, _ = generate.rewrite_body(entry["body"])
        if oracle.normalize_body(actual[cid]) != oracle.normalize_body(expected):
            got = oracle.body_sha256(actual[cid])[:12]
            problems.append(f"{cid}: body differs from oracle "
                            f"(expected {entry['body_sha256'][:12]}, got {got})")
    return problems


def check_rows(source, entries):
    """Verify every table row's callback and global slot belong to its cid.

    A row whose metadata is right but whose callback is another action's
    would still compile, and would make a menu item invoke the wrong
    handler; only a structural check catches that.
    """
    from . import generate

    problems = []
    for entry in entries:
        row = generate._spec_row(entry)
        if row not in source:
            problems.append(f"{entry['cid']}: expected row not found "
                            f"(wrong callback, global slot or metadata)")
    return problems


def check_include_guard(header, guard="GSCHEM_ACTIONS_H"):
    """Verify no other header claims the generated header's include guard.

    Two headers sharing a guard silently suppress one of them depending on
    include order, which is how a stale 48-action declaration file was able
    to shadow the real 135-action one without any compile error.  Catching
    it here turns a silent include-order dependency into a gate failure.
    """
    problems = []
    scope = config.REPO_ROOT / "gschem"
    for path in scope.rglob("*.h"):
        if path == config.ACTIONS_H:
            continue
        try:
            text = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            continue
        if f"#define {guard}" in text:
            problems.append(f"{path.relative_to(config.REPO_ROOT)} also "
                            f"defines {guard}")
    return problems


def check_no_forbidden_source(source):
    """Reject constructs the new registry must not use.

    Only code is inspected: the file's own comments explain what was
    replaced, and mentioning a retired name in prose is not a defect.
    """
    code = re.sub(r"/\*.*?\*/", " ", source, flags=re.DOTALL)
    code = re.sub(r"//[^\n]*", " ", code)
    problems = []
    if "DEFINE_ACTION" in code:
        problems.append("references the retired DEFINE_ACTION macro")
    if "actions.decl" in code:
        problems.append("includes the retired actions.decl.x")
    if "dlopen" in code:
        problems.append("uses dlopen (built-in actions must not)")
    return problems
