"""Inventory of gschem actions, derived deterministically from the oracle.

The inventory is the single list every other stage agrees on: the C
generator, the compiler gate, and the model prompts all iterate it in the
same order.  It is written to ``.refactor/actions.json`` for inspection and
for resuming without re-parsing.
"""

import json

from . import config, oracle


def build(records=None):
    """Return the inventory as a list of plain JSON-serialisable dicts."""
    if records is None:
        records = oracle.parse_file(config.ORACLE_FILE)
    inventory = []
    for index, record in enumerate(records):
        entry = {
            "index": index,
            "cid": record["cid"],
            "id": oracle.unquote(record["id"]),
            "icon": oracle.unquote(record["icon"]),
            "name": oracle.unquote(record["name"]),
            "label": oracle.unquote(record["label"]),
            "menu_label": oracle.unquote(record["menu_label"]),
            "tooltip": oracle.unquote(record["tooltip"]),
            "type": record["type"],
            "comment": record["comment"],
            "body": record["body"],
            "body_sha256": record["body_sha256"],
        }
        # The raw C expressions are kept alongside the decoded values: the
        # generator re-emits them so that adjacent string concatenation and
        # wrapping macros survive verbatim.  Decoded values are what the
        # model sees and what equality checks use.
        for field in ("id", "icon", "name", "label", "menu_label", "tooltip",
                      "type"):
            entry["raw_" + field] = record[field]
        inventory.append(entry)
    return inventory


def write(inventory, path=None):
    """Atomically write the inventory to ``.refactor/actions.json``."""
    path = path or (config.STATE_DIR / "actions.json")
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_suffix(".json.tmp")
    tmp.write_text(json.dumps(inventory, indent=2) + "\n", encoding="utf-8")
    tmp.replace(path)
    return path


def read(path=None):
    """Read a previously written inventory."""
    path = path or (config.STATE_DIR / "actions.json")
    return json.loads(path.read_text(encoding="utf-8"))


def region(cid):
    """Return the file region a cid belongs to (its ``/*! \\section`` group).

    Used to keep the generated table grouped the way the original file
    was, so reviews stay readable.
    """
    head = cid.split("_", 1)[0]
    return {
        "file": "File", "page": "File",
        "edit": "Edit", "clipboard": "Edit",
        "add": "Add", "buffer": "Add",
        "view": "View", "hierarchy": "View",
        "zoom": "View", "pan": "View",
        "options": "Options",
        "attributes": "Attributes",
        "select": "Select",
    }.get(head, "Misc")
