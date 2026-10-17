"""Checkpoint and rollback support.

A checkpoint is a git tag (``ckpt/N``) plus a small state file recording
what had been completed when it was taken.  Rollback restores the tagged
tree and the recorded state, so a bad batch of units can be undone without
guessing which of them was the first to break something.

Checkpoints are only ever taken on a clean working tree, and never destroy
commits: rollback moves the branch pointer back, and the abandoned commit
stays reachable through the tag or reflog for inspection.
"""

import json
import subprocess

from . import config, journal

STATE_FILE = config.STATE_DIR / "state.json"


class CheckpointError(RuntimeError):
    """Raised when a checkpoint or rollback cannot be done safely."""


def _git(*args, check=True):
    result = subprocess.run(("git",) + args, cwd=config.REPO_ROOT,
                            capture_output=True, text=True)
    if check and result.returncode != 0:
        raise CheckpointError(f"git {' '.join(args)} failed: "
                              f"{result.stderr.strip()}")
    return result.stdout.strip()


def is_clean():
    """True when there are no uncommitted changes."""
    return _git("status", "--porcelain") == ""


def current_commit():
    """Full SHA of HEAD."""
    return _git("rev-parse", "HEAD")


def write_state(**fields):
    """Replace the state file with the given fields plus a timestamp."""
    import datetime
    state = dict(fields)
    state["at"] = datetime.datetime.now(
        datetime.timezone.utc).isoformat(timespec="seconds")
    STATE_FILE.parent.mkdir(parents=True, exist_ok=True)
    tmp = STATE_FILE.with_suffix(".json.tmp")
    tmp.write_text(json.dumps(state, indent=2) + "\n", encoding="utf-8")
    tmp.replace(STATE_FILE)
    return state


def read_state():
    """Return the last written state, or {} if none."""
    if not STATE_FILE.is_file():
        return {}
    return json.loads(STATE_FILE.read_text(encoding="utf-8"))


def save(number=None):
    """Commit the working tree and tag it ``ckpt/<number>``.

    The next number is derived from the existing tags when not given.
    """
    if number is None:
        number = next_number()
    name = f"ckpt/{number}"
    _git("add", "-A")
    if not is_clean():
        _git("commit", "-q", "-m", f"checkpoint {name}")
    _git("tag", "-f", name)
    commit = current_commit()
    journal.append("checkpoint", name=name, commit=commit)
    return name, commit


def next_number():
    """One past the highest existing ``ckpt/N`` tag."""
    tags = _git("tag", "-l", "ckpt/*").splitlines()
    numbers = []
    for tag in tags:
        try:
            numbers.append(int(tag.split("/", 1)[1]))
        except (IndexError, ValueError):
            continue
    return max(numbers, default=0) + 1


def list_checkpoints():
    """Return [(name, commit, subject)] for each checkpoint, newest last."""
    out = []
    for tag in _git("tag", "-l", "ckpt/*").splitlines():
        row = _git("log", "-1", "--format=%H%x09%s", tag)
        commit, _, subject = row.partition("\t")
        out.append((tag, commit, subject))
    out.sort(key=lambda r: int(r[0].split("/", 1)[1]))
    return out


def rollback(name):
    """Reset the branch to a checkpoint tag.

    Refuses to run on a dirty tree: silently discarding uncommitted work is
    exactly the failure mode this machinery exists to prevent.
    """
    if not is_clean():
        raise CheckpointError(
            "working tree is dirty; commit or discard changes first "
            "(run `git status` to see them)")
    tags = {tag for tag, _, _ in list_checkpoints()}
    if name not in tags:
        raise CheckpointError(f"no such checkpoint: {name} (have: "
                              f"{', '.join(sorted(tags)) or 'none'})")
    _git("reset", "--hard", name)
    journal.append("rollback", checkpoint=name, commit=current_commit())
    return current_commit()
