# gschem action registry: transform protocol

This document describes the mechanical transformation from gschem's
`DEFINE_ACTION` macro pipeline to the `GschemActionSpec` table in
`gschem/src/actions.c`. It exists so the change can be reviewed, repeated
and audited without re-deriving intent from the diff.

## The shape of the change

Before, each action was a macro invocation:

```c
DEFINE_ACTION (file_save,
               "file-save",
               "gtk-save",
               _("Save"),
               _("Save"),
               _("_Save"),
               NULL,
               ACTUATE)
{
  x_highlevel_save_page (w_current, NULL);
}
```

The macro expanded, per action, into declarations in a generated
`actions.decl.x`, global pointer definitions, and registration calls — with
the callback body pasted into a `switch`-like context. The preprocessor
carried no type information, so a misspelt callback or a wrong column was a
link error at best and a silently dead menu item at worst.

After, the same action is one row and one function:

```c
  { "file-save", "gtk-save", N_("Save"), N_("Save"), N_("_Save"),
    NULL, ACTUATE, action_callback_file_save, &action_file_save },
```

```c
void
action_callback_file_save (GschemAction *action,
                           GschemToplevel *w_current)
{
  x_highlevel_save_page (w_current, NULL);
}
```

## Invariants

The transformation preserves these properties, and the verification step
checks all of them:

1. **Identity.** Every cid in `actions.c.bak` has exactly one row and one
   callback. `id` is the string from the oracle, *not* recomputed from the
   cid: five actions (`file_write_png` → `file-image`, `file_close` →
   `file-close-window`, `edit_find` → `edit-find-text`,
   `edit_autonumber_text` → `edit-autonumber`, `options_afeedback` →
   `options-action-feedback`) deliberately differ, and the Scheme bindings
   `&file-image` etc. depend on that.

2. **Metadata.** Icon, name, label, menu label, tooltip and type are copied
   from the oracle. `_()` becomes `N_()` because a static initialiser cannot
   call `gettext()`, and `gschem_action_register()` applies `_()` at
   registration time. Adjacent string literals are preserved verbatim rather
   than being re-escaped, because several tooltips span lines.

3. **Callback bodies.** The body is copied unchanged. There is exactly one
   mechanical rewrite: `call (void)` becomes `call ()` (6 actions, all
   `view_*`). Under the macro a bare `void` was a type name in a textual
   paste; inside a real function it is a hard error.

4. **Order.** Rows appear in oracle order, so the table diffs legibly against
   the original file.

## Why the oracle is `actions.c.bak`

`actions.c.bak` is the last artifact produced before the macro pipeline was
removed, and it is the only complete record of all 135 actions in one place.
It is therefore the acceptance oracle, not the model. Acceptance never
depends on the model's opinion:

* the generated table and bodies are produced **deterministically** by
  `python3 -m gedai.refactor gen`, so the tree compiles before any API call;
* the model stage re-derives each action independently and its output must
  equal the oracle after the same documented rewrites;
* disagreement is journalled as drift and rejected, never applied.

This inverts the usual arrangement. A model that is shown the answer cannot
be wrong except by copying badly, so agreement would prove nothing. A model
asked to work from the original source gives an independent check: when it
agrees, the transformation is very likely mechanical; when it disagrees, the
disagreement is a signal worth reading.

## The three tiers

The refactor keeps actions the Scheme layer defines, rather than pretending
every action is C:

* **Tier A — in-process C table.** The 135 actions in `action_specs`. A
  function pointer, no runtime lookup, no Guile on the hot path.
* **Tier B — Scheme `define-action` at load time.** User and experimental
  actions: the 4 help links in `scheme/gschem/builtins.scm` and the 6
  experimental entries in `scheme/pcb.scm`. These are created lazily through
  the existing `gschem_action_register()` path and are unaffected by the
  refactor. They are not built-in actions and must not be moved into the C
  table.
* **Tier C — external executable owner (deferred).** A future owner of an
  action is a separate program, addressed as:
  `TOOL --describe` → manifest on stdout; `TOOL [args]` → s-expression on
  stdin, s-expression on stdout; exit 0 success, 1 user error, 2 protocol
  error, 3 internal error. Only argv and stdin/stdout cross the boundary, so
  the contract stays valid on mingw, and the executable path is never taken
  from a schematic file.

Tier C is not implemented. Its shape is fixed here so that the C registry
does not acquire an assumption that contradicts it. In particular the
registry stores a callback and a pointer to the action's global slot — the
unit a JSON codegen or a plugin ABI would also need — so adding a tier does
not change the type every existing row is written against.

## What is deliberately not changed

* No `dlopen` for built-in actions.
* No JSON at runtime: the table is a plain C array.
* No new third-party dependencies; the tooling is standard-library Python.
* No semantic change to any callback. Not even whitespace inside a body.

## Why the menu builder still reads Scheme

`x_menus.c` builds the menu bar, context menu and toolbar by walking the
Scheme variables `menubar`, `context-menu` and `toolbar`. That is the last
place where Guile sits on a path that touches actions, and it looks like the
next thing to remove. It should not be removed, for two independent reasons.

**The default menu contains Tier B actions.** `help-menu` in
`lib/system-gschemrc.scm` holds `&help-manual`, `&help-guide`, `&help-faq`
and `&help-wiki`. These are defined with `define-action-public` in
`scheme/gschem/builtins.scm`, their bodies are Scheme (`show-wiki`), and
they are the same four Tier B actions described above. A menu built only from
`action_specs` could not produce the stock Help menu. 108 of the 112 actions
named in the default menus are in the C table; the other 4 are Tier B.

**The menu structure is a published interface.** The comment block at
`lib/system-gschemrc.scm:1131` documents that users may edit `menubar` and
friends from `.gschemrc` using
`set!`/`assoc-set!`/`set-cdr!`, and shows worked examples. Anything that
replaces the Scheme walk with a fixed C layout breaks every user who has
customised a menu — a behaviour regression, not a refactor. Detecting an
override would mean keeping a C copy of the 114-entry default to compare
against, which duplicates `system-gschemrc.scm` and can drift from it
silently; that trades a real interface for a guessed one.

The registry refactor already removed Guile from where it mattered most:
action *registration* is a static table and activation is a function
pointer. What remains is a tree walk that runs once per window, over data
the user is documented to own. It stays.

If this is ever revisited, the order is: promote nothing to the C table
first (Tier B must stay in Scheme), then give the three menu variables a
typed C representation that Scheme can still assign to, then build from
that. That is a design change, not a cleanup, and it needs its own protocol.
