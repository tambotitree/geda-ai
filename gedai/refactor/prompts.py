"""Prompt construction for the per-action unit stage.

Design rule: the model is shown one action and asked to reproduce it in the
new form.  It is *not* shown the oracle answer in the prompt, because a
model that has seen the answer can only fail by copying badly; without it,
agreement is weak evidence but disagreement is strong evidence.  The oracle
decides acceptance either way.

The task is a mechanical rewrite, so the instructions are explicit about
what must not change: every call, argument and comment in the callback.
"""

import json

SYSTEM_PROMPT = """\
You are refactoring the C source of gschem, a schematic editor, from a
preprocessor macro to a plain function-pointer registry.

The old form defined one action per DEFINE_ACTION block:

    DEFINE_ACTION (cid, "id", "icon", _("Name"), _("Label"), _("_Menu"),
                   _("tooltip"), TYPE)
    {
      ...body...
    }

The new form has two parts.

1. A row in the table `static const GschemActionSpec action_specs[]`:

    { "id", "icon", N_("Name"), N_("Label"), N_("_Menu"),
      N_("tooltip"), TYPE, action_callback_cid, &action_cid },

   `_()` becomes `N_()` because a static initialiser cannot call gettext.
   The `id` and `icon` are bare string literals: do NOT wrap them in `N_()`
   or `_()`.  NULL stays NULL.  Adjacent string literals stay as they are.

2. An ordinary function whose body is the macro body, unchanged:

    void
    action_callback_cid (GschemAction *action, GschemToplevel *w_current)
    {
      ...body...
    }

Rules:
- Do not change the body's behaviour.  Same calls, same arguments, same
  order, same comments.  This is a mechanical transformation, not a cleanup.
- Do not rename identifiers, reformat expressions, or "improve" anything.
- A call written as `foo (void)` in the old body becomes `foo ()`, because
  `void` is not a value.
- `action` and `w_current` are already in scope; do not declare them.

Reply with a JSON object and nothing else, with exactly these keys:

{
  "cid":        the cid from the input,
  "spec":       the table row, starting with "{ " and ending with " },",
  "callback":   the complete C function definition,
  "notes":      "" or a short explanation if you had to deviate,
  "confidence": 0.0 to 1.0
}
"""


def unit_messages(entry):
    """Build the message list for one action.

    The entry is rendered as JSON rather than embedded in prose so the model
    sees the fields unambiguously, including ones whose values are empty.
    """
    payload = {
        "cid": entry["cid"],
        "id": entry["id"],
        "icon": entry["icon"],
        "name": entry["name"],
        "label": entry["label"],
        "menu_label": entry["menu_label"],
        "tooltip": entry["tooltip"],
        "type": entry["type"],
        "doc_comment": entry["comment"] or None,
        "body": entry["body"],
    }
    user = ("Refactor this single action.  Reply with the JSON object "
            "described in the system prompt.\n\n" +
            json.dumps(payload, indent=2))
    return [
        {"role": "system", "content": SYSTEM_PROMPT},
        {"role": "user", "content": user},
    ]
