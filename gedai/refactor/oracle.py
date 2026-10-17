"""Deterministic parser for gschem's original DEFINE_ACTION blocks.

``gschem/src/actions.c.bak`` is the last artifact produced by the C
preprocessor pipeline and is treated as the ground truth for all 135
actions.  Nothing in this module calls a model: given the same input it
always yields the same inventory, so it can serve as an acceptance oracle.

The macro has this shape::

    DEFINE_ACTION (file_new,
                   "file-new",
                   "gtk-new",
                   _("New File"),
                   _("New"),
                   _("_New"),
                   NULL,
                   ACTUATE)
    {
      ...callback body, using `action' and `w_current'...
    }

Blocks are separated (and documented) by lines containing only ``/**``.
"""

import hashlib
import re

FIELD_NAMES = ("cid", "id", "icon", "name", "label", "menu_label",
               "tooltip", "type")

_OPEN_RE = re.compile(r"^DEFINE_ACTION\s*\(", re.MULTILINE)


class OracleError(RuntimeError):
    """Raised when the oracle file does not have the expected shape."""


def _split_args(text, start):
    """Split the macro argument list beginning at ``start`` (just past '(').

    Returns (args, index_after_closing_paren).  Commas inside parentheses or
    string literals do not split.
    """
    depth = 1
    in_string = False
    escaped = False
    current = []
    args = []
    i = start
    while i < len(text):
        ch = text[i]
        if in_string:
            current.append(ch)
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == '"':
                in_string = False
        elif ch == '"':
            in_string = True
            current.append(ch)
        elif ch == "(":
            depth += 1
            current.append(ch)
        elif ch == ")":
            depth -= 1
            if depth == 0:
                args.append("".join(current).strip())
                return args, i + 1
            current.append(ch)
        elif ch == "," and depth == 1:
            args.append("".join(current).strip())
            current = []
        else:
            current.append(ch)
        i += 1
    raise OracleError("unterminated DEFINE_ACTION argument list")


def _brace_body(text, start):
    """Return (body, index_after_closing_brace) for a {...} block at start."""
    if start >= len(text) or text[start] != "{":
        raise OracleError(f"expected '{{' at offset {start}, found "
                          f"{text[start:start + 20]!r}")
    depth = 0
    in_string = False
    in_char = False
    in_line_comment = False
    in_block_comment = False
    escaped = False
    i = start
    while i < len(text):
        ch = text[i]
        nxt = text[i + 1] if i + 1 < len(text) else ""
        if in_line_comment:
            if ch == "\n":
                in_line_comment = False
        elif in_block_comment:
            if ch == "*" and nxt == "/":
                in_block_comment = False
                i += 1
        elif in_string:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == '"':
                in_string = False
        elif in_char:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == "'":
                in_char = False
        elif ch == "/" and nxt == "/":
            in_line_comment = True
            i += 1
        elif ch == "/" and nxt == "*":
            in_block_comment = True
            i += 1
        elif ch == '"':
            in_string = True
        elif ch == "'":
            in_char = True
        elif ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return text[start + 1:i], i + 1
        i += 1
    raise OracleError(f"unterminated callback body at offset {start}")


def normalize_body(body):
    """Collapse trailing whitespace so bodies compare after reformatting."""
    lines = [line.rstrip() for line in body.splitlines()]
    while lines and not lines[0].strip():
        lines.pop(0)
    while lines and not lines[-1].strip():
        lines.pop()
    return "\n".join(lines)


def body_sha256(body):
    """Stable digest of a normalized callback body."""
    return hashlib.sha256(normalize_body(body).encode("utf-8")).hexdigest()


def is_void(expr):
    """True when a C expression names the ``void`` type rather than a value.

    ``actions.c.bak`` was written for a macro whose body was textual, so a
    handful of callbacks contain ``foo (void)`` where the author meant
    ``foo ()``.  Once the body is wrapped in a real function, ``void`` is no
    longer a valid argument, so the generator must know to drop it.
    """
    return expr is not None and expr.strip() == "void"


def _leading_comment(text, block_start):
    """Return the ``/*! ... */`` doc comment immediately above a block.

    gschem writes these as a one-line ``/*! \\brief ... */`` (sometimes
    multi-line) followed by a single blank line, so the blank line must be
    skipped rather than treated as a terminator.
    """
    collected = []
    for line in reversed(text[:block_start].splitlines()):
        stripped = line.strip()
        if not stripped:
            if collected:
                break
            continue
        if stripped.startswith("/*!") or (collected and stripped.startswith("*")):
            collected.append(stripped)
            if stripped.startswith("/*!"):
                break
        else:
            break
    return "\n".join(reversed(collected))


def unquote(expr):
    """Return the value of a C string literal, or None for ``NULL``.

    Non-literal expressions (macro calls, identifiers) are returned as-is so
    callers can decide what to do with them.  ``_("x")`` and ``N_("x")`` are
    unwrapped to their string value.
    """
    expr = expr.strip()
    if expr == "NULL":
        return None
    for wrapper in ("_", "N_"):
        prefix = wrapper + "("
        if expr.startswith(prefix) and expr.endswith(")"):
            return unquote(expr[len(prefix):-1])
    if len(expr) >= 2 and expr[0] == '"' and expr[-1] == '"':
        return expr[1:-1].replace('\\"', '"').replace("\\\\", "\\")
    return expr


def parse(text):
    """Parse oracle source text into an ordered list of action records."""
    records = []
    for match in _OPEN_RE.finditer(text):
        args, after_args = _split_args(text, match.end())
        if len(args) != len(FIELD_NAMES):
            raise OracleError(
                f"DEFINE_ACTION for {args[0]!r} has {len(args)} arguments, "
                f"expected {len(FIELD_NAMES)}")
        # Skip whitespace between ')' and '{'.
        i = after_args
        while i < len(text) and text[i] in " \t\r\n":
            i += 1
        body, _ = _brace_body(text, i)
        record = dict(zip(FIELD_NAMES, args))
        record["comment"] = _leading_comment(text, match.start())
        record["body"] = normalize_body(body)
        record["body_sha256"] = body_sha256(body)
        records.append(record)
    return records


def parse_file(path):
    """Parse an oracle file into action records."""
    return parse(path.read_text(encoding="utf-8"))


def validate(records):
    """Check the inventory is complete and free of collisions.

    Returns a list of human-readable problems; empty means clean.  A few
    actions in the original file deliberately use an ``id`` that differs
    from their cid (``file_write_png`` is ``"file-image"``), so those are
    reported by :func:`id_mismatches` rather than as errors.
    """
    problems = []
    seen_cid = {}
    seen_id = {}
    for record in records:
        cid = record["cid"]
        ident = unquote(record["id"])
        if cid in seen_cid:
            problems.append(f"duplicate cid {cid!r}")
        if ident in seen_id:
            problems.append(f"duplicate id {ident!r}")
        seen_cid[cid] = record
        seen_id[ident] = record
        if not record["body"].strip():
            problems.append(f"{cid}: empty callback body")
    return problems


def id_mismatches(records):
    """Return (cid, id) pairs whose id is not simply cid with dashes."""
    out = []
    for record in records:
        expected = record["cid"].replace("_", "-")
        actual = unquote(record["id"])
        if actual != expected:
            out.append((record["cid"], actual, expected))
    return out
