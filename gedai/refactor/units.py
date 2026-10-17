"""Per-action unit loop: ask the model, then let the oracle decide.

The loop's contract is narrow on purpose.  For one action it:

1. builds the prompt from the inventory (never from the file under edit);
2. asks the model for the spec row and the callback;
3. compares the callback body against the oracle, after the same documented
   mechanical rewrites the generator applies;
4. regenerates the file from the inventory and splices in the model's
   callback, so a rejected body cannot half-apply;
5. compiles ``actions.c``;
6. commits only if compile and body equality both pass.

A unit that fails any step is journalled and left uncommitted, so the file
always contains either the previous accepted state or a fully verified new
one.
"""

import re
import subprocess
import time

from . import (checkpoint, config, deepseek, generate, journal, oracle,
               prompts, validators)

_FUNC_RE = re.compile(
    r"^void\s+action_callback_(\w+)\s*\(GschemAction \*\s*action,\s*\n?"
    r"\s*GschemToplevel \*\s*w_current\)\s*\n?\{(.*)\n?\}\s*$",
    re.DOTALL | re.MULTILINE)


class UnitError(RuntimeError):
    """A unit could not be accepted."""


def compile_actions_c():
    """Compile ``actions.c`` and return (ok, first_error_lines).

    ``actions.c`` is removed first, then rebuilt through the build system's
    own pattern rule, so the gate cannot pass on a stale object file: make
    reports "up to date" when the timestamp comparison says nothing changed,
    which would silently bless an edit that never reached the compiler.
    """
    obj = config.REPO_ROOT / "gschem" / "src" / "gschem-actions.o"
    try:
        obj.unlink()
    except FileNotFoundError:
        pass

    result = subprocess.run(
        ["make", "-C", "gschem/src", "gschem-actions.o"],
        cwd=config.REPO_ROOT, capture_output=True, text=True)
    if result.returncode == 0 and obj.exists():
        return True, []
    errors = [line for line in result.stderr.splitlines()
              if line.startswith("actions.c:") and "error:" in line]
    if not errors:
        tail = (result.stderr or result.stdout).strip().splitlines()[-5:]
        errors = tail or ["compilation produced no object file"]
    return False, errors


def extract_callback(reply, cid):
    """Pull the function definition out of a model reply."""
    text = reply.get("callback")
    if not isinstance(text, str) or "action_callback_" not in text:
        raise UnitError("reply has no 'callback' string")
    match = _FUNC_RE.search(text.strip())
    if not match:
        raise UnitError("callback is not a well-formed "
                        "action_callback_<cid> definition")
    if match.group(1) != cid:
        raise UnitError(f"callback defines {match.group(1)!r}, expected {cid!r}")
    return match.group(2)


def check_spec(reply, entry):
    """Compare the model's table row against the oracle-derived one."""
    expected = generate._spec_row(entry).strip()
    actual = (reply.get("spec") or "").strip()
    normalize = lambda s: re.sub(r"\s+", " ", s).strip()
    return normalize(actual) == normalize(expected), expected, actual


def apply_body(cid, body):
    """Replace one callback body in ``actions.c`` with ``body``.

    The file is regenerated from the inventory first, so the result is a pure
    function of (oracle, inventory, this body): no other unit's edit can be
    lost, and re-applying the same body is a no-op.
    """
    from . import inventory as inventory_mod

    entries = inventory_mod.build()
    source, _ = generate.render_source(entries)
    pattern = re.compile(
        r"(^action_callback_" + re.escape(cid) +
        r" \(GschemAction \*action,\s*\n\s*GschemToplevel \*w_current\)\n\{\n)"
        r"(.*?)"
        r"(\n\}\n)",
        re.DOTALL | re.MULTILINE)
    if not pattern.search(source):
        raise UnitError(f"cannot locate callback for {cid} in regenerated source")
    updated = pattern.sub(
        lambda m: m.group(1) + oracle.normalize_body(
            generate.rewrite_body(body)[0]) + m.group(3),
        source, count=1)
    if updated == source:
        return False
    config.ACTIONS_C.write_text(updated)
    return True


def commit_unit(entry, result):
    """Write, verify and commit one accepted unit.

    Returns the short commit hash, or None when the tree already matched the
    accepted body (a re-run of an already-applied unit).
    """
    cid = entry["cid"]
    reply_body = result.get("body")
    if reply_body is not None:
        apply_body(cid, reply_body)

    source = config.ACTIONS_C.read_text()
    entries = [entry]
    problems = (validators.check_bodies(source, entries)
                + validators.check_rows(source, [entry]))
    if problems:
        checkpoint._git("checkout", "--", "gschem/src/actions.c")
        raise UnitError("post-apply verification failed: "
                        + "; ".join(problems[:3]))

    checkpoint._git("add", "gschem/src/actions.c")
    staged = subprocess.run(
        ["git", "diff", "--cached", "--quiet"],
        cwd=config.REPO_ROOT).returncode != 0
    if not staged:
        return None

    checkpoint._git("commit", "-q", "-m",
                    f"unit: {cid}\n\n"
                    f"Model: {config.API_MODEL}; "
                    f"body verified equal to oracle "
                    f"{entry['body_sha256'][:12]}; gschem-actions.o compiles.")
    short = checkpoint._git("rev-parse", "--short", "HEAD")
    journal.append("unit-committed", cid=cid, commit=short,
                   usage=result.get("usage"),
                   elapsed_s=result.get("elapsed_s"))
    return short


def run_unit(entry, api_key=None, include_spec=True):
    """Process one action.  Returns a result dict; raises on transport error."""
    cid = entry["cid"]
    started = time.monotonic()
    reply, usage = deepseek.complete_json(prompts.unit_messages(entry),
                                          api_key=api_key)
    elapsed = round(time.monotonic() - started, 1)

    body = extract_callback(reply, cid)
    expected_body, _ = generate.rewrite_body(entry["body"])
    body_matches = (oracle.normalize_body(body)
                    == oracle.normalize_body(expected_body))
    spec_ok, expected_spec, actual_spec = (check_spec(reply, entry)
                                           if include_spec else (True, "", ""))

    result = {
        "cid": cid,
        "elapsed_s": elapsed,
        "usage": usage,
        "body": body,
        "body_matches": body_matches,
        "spec_matches": spec_ok,
        "notes": reply.get("notes", ""),
        "confidence": reply.get("confidence"),
    }
    if not body_matches or not spec_ok:
        journal.append("drift", cid=cid,
                       body_matches=body_matches,
                       spec_matches=spec_ok,
                       expected_spec=expected_spec if not spec_ok else None,
                       actual_spec=actual_spec if not spec_ok else None,
                       notes=result["notes"])
        raise UnitError("model output differs from oracle "
                        f"(body={body_matches}, spec={spec_ok})")
    return result
