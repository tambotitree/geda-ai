# gschem/src action registry

`actions.c` contains one table (`action_specs`) and one callback per action.
`actions.h` declares the matching `action_<cid>` globals and callbacks.

The file was produced by a one-off, resumable refactor driven by
`python3 -m gedai.refactor`; see `../TRANSFORM_PROTOCOL.md` in this
directory for the transformation and its invariants.

## Editing an action

Edit the row and the callback directly. Once the refactor is closed, the
generator is no longer the source of truth: it exists to reproduce the
historical file exactly, and running it again will overwrite hand edits.

## Adding an action

Add a row and a callback, and add the `extern GschemAction *action_<cid>;`
declaration to `actions.h`. The `GschemActionSpec` row needs:

| field       | notes                                                    |
|-------------|----------------------------------------------------------|
| `id`        | stable, used as the Scheme binding `&<id>`; bare literal  |
| `icon_name` | icon theme name or `NULL`                                |
| `name`      | long name, `N_()`                                        |
| `label`     | short label, `N_()`                                      |
| `menu_label`| mnemonic label, `N_()`                                   |
| `tooltip`   | tooltip or `NULL`                                        |
| `type`      | `ACTUATE`, `TOGGLE_CHECK`, `TOGGLE_PLAIN`, `TOGGLE_RADIO`|
| `activate`  | the callback function                                    |
| `action`    | `&action_<cid>`, filled in at registration               |

`N_()` marks a string for translation; `gschem_action_register()` applies
`_()` when the action is registered, because a static initialiser cannot
call `gettext()`.

## The refactor tooling

State lives in `.refactor/` at the repository root (git-ignored): the parsed
inventory, an append-only journal, gate reports and snapshots.

```sh
python3 -m gedai.refactor oracle      # verify actions.c.bak parses cleanly
python3 -m gedai.refactor inventory   # write .refactor/actions.json
python3 -m gedai.refactor gen         # regenerate actions.{c,h}
python3 -m gedai.refactor verify      # acceptance checks + compile gate
python3 -m gedai.refactor status      # progress, tokens, time left
python3 -m gedai.refactor checkpoint  # commit and tag ckpt/N
python3 -m gedai.refactor rollback ckpt/N
```

`verify` is the gate everything else defers to. It checks that the table
rows, callback definitions, externs and declarations all cover the same 135
cids; that every row names its own callback and global slot; that every
callback body still equals the oracle after the documented rewrites; that no
other header claims this file's include guard; and that `gschem-actions.o`
compiles from scratch.

The compile step deletes the object file first, because `make` reports
"up to date" when timestamps say nothing changed, which would let a broken
edit pass the gate.

## Why both a generator and a model

The generator is deterministic and makes the tree green regardless of the
API. The model stage re-derives each action from the original source and
must agree with the oracle; disagreement is journalled as drift and
rejected. Neither stage can silently change behaviour.
