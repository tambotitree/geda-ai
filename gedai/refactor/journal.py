"""Append-only journal of everything the refactor run did.

The journal is the audit trail: one JSON object per line, flushed to disk
before the operation it describes is considered done.  An interrupted run
can be reconstructed by replaying it, and ``status`` summarises it.

Records are deliberately small and flat so they can be grepped.
"""

import datetime
import json

from . import config

JOURNAL_FILE = config.STATE_DIR / "journal.jsonl"


def _now():
    return datetime.datetime.now(datetime.timezone.utc).isoformat(
        timespec="seconds")


def append(event, **fields):
    """Append one record to the journal and flush it to disk."""
    JOURNAL_FILE.parent.mkdir(parents=True, exist_ok=True)
    record = {"at": _now(), "event": event}
    record.update(fields)
    with open(JOURNAL_FILE, "a", encoding="utf-8") as handle:
        handle.write(json.dumps(record, sort_keys=True) + "\n")
        handle.flush()
    return record


def records():
    """Return every journal record, oldest first."""
    if not JOURNAL_FILE.is_file():
        return []
    out = []
    for line in JOURNAL_FILE.read_text(encoding="utf-8").splitlines():
        if line.strip():
            out.append(json.loads(line))
    return out


def last_commit_for(cid):
    """Return the commit recorded for a cid, or None."""
    for record in reversed(records()):
        if record.get("event") == "unit-committed" and record.get("cid") == cid:
            return record.get("commit")
    return None


def completed_cids():
    """Set of cids whose unit was applied and verified.

    A unit counts as done whether the model's output was identical to the
    oracle (``unit-verified``) or changed something that then passed
    verification and was committed (``unit-committed``).
    """
    finished = {"unit-committed", "unit-verified"}
    return {r["cid"] for r in records()
            if r.get("event") in finished and "cid" in r}


def drift():
    """Records where the model deviated from the oracle."""
    return [r for r in records() if r.get("event") == "drift"]


def usage_totals():
    """Sum of prompt and completion tokens recorded so far."""
    prompt = completion = 0
    for record in records():
        usage = record.get("usage") or {}
        prompt += usage.get("prompt_tokens", 0)
        completion += usage.get("completion_tokens", 0)
    return {"prompt_tokens": prompt, "completion_tokens": completion,
            "total_tokens": prompt + completion}
