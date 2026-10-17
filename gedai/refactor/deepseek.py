"""Minimal DeepSeek chat-completions client.

Uses only the standard library: the project must not grow a new runtime
dependency for a one-off refactor, and ``urllib`` is enough for a
non-streaming JSON request.

The client is deliberately strict.  It returns the decoded JSON object the
model was asked for, or raises.  It never retries into an unbounded loop and
never treats a malformed reply as success: a unit that cannot be parsed is
recorded as failed and left for the next run.
"""

import json
import socket
import time
import urllib.error
import urllib.request

from . import config


class DeepSeekError(RuntimeError):
    """Transport, protocol or billing failure."""


class NonRetryableError(DeepSeekError):
    """A failure that retrying cannot fix (bad request, auth, budget)."""


def _post(payload, api_key, timeout):
    request = urllib.request.Request(
        config.API_BASE,
        data=json.dumps(payload).encode("utf-8"),
        headers={"Content-Type": "application/json",
                 "Authorization": f"Bearer {api_key}"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(request, timeout=timeout) as response:
            return json.loads(response.read().decode("utf-8"))
    except urllib.error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")[:400]
        if exc.code in (400, 401, 402, 403, 404, 422):
            raise NonRetryableError(f"HTTP {exc.code}: {body}") from exc
        raise DeepSeekError(f"HTTP {exc.code}: {body}") from exc
    except (urllib.error.URLError, socket.timeout) as exc:
        raise DeepSeekError(f"transport failure: {exc}") from exc


def complete(messages, api_key=None, model=None, temperature=0.0,
             max_tokens=2048, retries=3, timeout=120):
    """Send one chat completion and return (text, usage_dict).

    ``temperature`` defaults to 0 because the task is a deterministic
    transformation: we want the same input to give the same output, so that
    a differing answer signals a real problem rather than sampling noise.
    """
    api_key = api_key or config.api_key()
    payload = {
        "model": model or config.API_MODEL,
        "messages": messages,
        "temperature": temperature,
        "max_tokens": max_tokens,
        "stream": False,
        "response_format": {"type": "json_object"},
    }

    last_error = None
    for attempt in range(retries):
        try:
            body = _post(payload, api_key, timeout)
        except NonRetryableError:
            raise
        except DeepSeekError as exc:
            last_error = exc
            time.sleep(min(2 ** attempt, 8))
            continue

        try:
            text = body["choices"][0]["message"]["content"]
        except (KeyError, IndexError, TypeError) as exc:
            raise DeepSeekError(f"unexpected response envelope: "
                                f"{json.dumps(body)[:300]}") from exc

        usage = body.get("usage") or {}
        if not usage.get("total_tokens") and usage.get("prompt_tokens"):
            usage["total_tokens"] = (usage.get("prompt_tokens", 0)
                                     + usage.get("completion_tokens", 0))
        return text, usage

    raise DeepSeekError(f"giving up after {retries} attempts: {last_error}")


def complete_json(messages, **kwargs):
    """Like :func:`complete`, but parse the reply as a JSON object."""
    text, usage = complete(messages, **kwargs)
    stripped = text.strip()
    if stripped.startswith("```"):
        # Tolerate a fenced reply even though we asked for a bare object.
        stripped = stripped.split("\n", 1)[-1]
        if stripped.endswith("```"):
            stripped = stripped.rsplit("```", 1)[0]
        stripped = stripped.strip()
    try:
        parsed = json.loads(stripped)
    except json.JSONDecodeError as exc:
        raise DeepSeekError(f"reply was not valid JSON: {exc}; "
                            f"first 200 chars: {stripped[:200]!r}") from exc
    if not isinstance(parsed, dict):
        raise DeepSeekError(f"expected a JSON object, got {type(parsed).__name__}")
    return parsed, usage
