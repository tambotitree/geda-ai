"""Configuration for the actions refactor run.

All state lives under the repo-root ``.refactor/`` directory (git-ignored).
The API key is read, in priority order, from:

1. the ``DEEPSEEK_API`` environment variable, or
2. a ``DEEPSEEK_API`` line in the repo-root ``.env`` file.

There is no fallback: if no key is found we fail fast rather than silently
skipping the model stage, because a missing key would otherwise be
indistinguishable from "the model produced nothing".
"""

import datetime
import os
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
STATE_DIR = REPO_ROOT / ".refactor"

ORACLE_FILE = REPO_ROOT / "gschem" / "src" / "actions.c.bak"
ACTIONS_C = REPO_ROOT / "gschem" / "src" / "actions.c"
ACTIONS_H = REPO_ROOT / "gschem" / "src" / "actions.h"

# The half-price window closes at 00:30 UTC; the operator asked for 00:00 UTC.
DEFAULT_UNTIL = datetime.datetime(2026, 10, 18, 0, 0,
                                  tzinfo=datetime.timezone.utc)

API_BASE = "https://api.deepseek.com/chat/completions"
API_MODEL = "deepseek-chat"


class ConfigError(RuntimeError):
    """Raised when the run cannot proceed for configuration reasons."""


def _read_dotenv():
    """Return a dict of the repo-root .env, or {} when absent/unreadable."""
    env_path = REPO_ROOT / ".env"
    if not env_path.is_file():
        return {}
    values = {}
    for line in env_path.read_text(encoding="utf-8", errors="replace").splitlines():
        line = line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        key, _, value = line.partition("=")
        values[key.strip()] = value.strip().strip("'\"")
    return values


def api_key(required=True):
    """Return the DeepSeek API key, or None when absent and not required."""
    key = os.environ.get("DEEPSEEK_API", "").strip()
    if not key:
        key = _read_dotenv().get("DEEPSEEK_API", "").strip()
    if not key and required:
        raise ConfigError(
            "DEEPSEEK_API not found: set it in the environment or in "
            f"{REPO_ROOT / '.env'} (e.g. DEEPSEEK_API='sk-...')."
        )
    return key or None


def parse_until(text=None):
    """Parse an ISO-8601 instant; default to the 00:00 UTC hard stop."""
    if not text:
        return DEFAULT_UNTIL
    # Accept a trailing Z as UTC, which datetime.fromisoformat only gained
    # in 3.11.
    normalized = text.strip()
    if normalized.endswith("Z"):
        normalized = normalized[:-1] + "+00:00"
    parsed = datetime.datetime.fromisoformat(normalized)
    if parsed.tzinfo is None:
        parsed = parsed.replace(tzinfo=datetime.timezone.utc)
    return parsed.astimezone(datetime.timezone.utc)


def minutes_left(until=None, now=None):
    """Whole minutes remaining before the hard stop (may be negative)."""
    now = now or datetime.datetime.now(datetime.timezone.utc)
    until = until or DEFAULT_UNTIL
    return int((until - now).total_seconds() // 60)
