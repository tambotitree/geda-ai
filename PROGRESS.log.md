# PROGRESS: gschem action registry refactor

Branch `refactor/gschem-actions-deepseek`, from `main @ 6347d0e`.

Hard stop for the API run: **2026-10-18T00:00:00Z**.

## Checkpoints

| tag     | commit    | state                                                  |
|---------|-----------|--------------------------------------------------------|
| ckpt/0  | `03555ce` | pre-refactor tree, 673 dirty files committed           |
| ckpt/1  | `c42e15c` | generated `actions.{c,h}`, gate green (0 errors)       |
| ckpt/2  | `c42e15c` | full `gschem/src` build + link green (no tree change)  |
| ckpt/3  | `79c8324` | tooling complete, `verify` green                       |
## Done

**P0 — deterministic generation (green before any API call).**

- Parsed all 135 `DEFINE_ACTION` blocks out of `actions.c.bak`.
- Generated `actions.c`: 135 global slots, 135 table rows,
  `gschem_register_all_actions()`, 135 callbacks.
- Generated `actions.h`: 135 externs, 135 declarations, registration entry
  point.
- Added `GschemActionCallback` (used but never declared) and
  `GschemActionSpec` to `gschem/include/gschem_action.h`.
- Moved `action_*` storage from `gschem_action.c` into `actions.c`: only 48
  of 135 were defined there, and `actions.decl.x` is empty.
- Fixed the stray `:w` (a vim artifact) on `Makefile.am:14`.

**P1 — build gate.** `make -C gschem/src` completes with 0 errors and links
`gschem/src/.libs/gschem`, with 135 global slots present.

**P2 — resumable unit loop.** `python3 -m gedai.refactor run --resume`
drives each action through prompt → API → oracle comparison → splice →
compile → commit, journalling every step.

## Defects the gates caught

Each was found by a check rather than by reading the diff, which is the
point of building them first.

1. **Adjacent string literals.** Re-escaping decoded metadata split two-line
   tooltips into invalid C. Fixed by re-emitting the oracle's raw
   expressions.
2. **`_()` → `N_()`.** A prefix bug produced `N__(`. Caught by compiling.
3. **`call (void)`.** 6 `view_*` callbacks contain `f (void)`; legal under
   the macro, a hard error in a real function. Rewritten to `f ()` and
   journalled as a `body-rewrite`.
4. **`gschem_register_all_actions()` undeclared.** Caught by compiling
   `gschem_action.c`.
5. **Stale-object gate.** `make` said "up to date", so the compile gate
   could bless an edit that never reached the compiler. Now the object file
   is deleted first; verified by injecting a syntax error, watching the gate
   fail, and restoring.
6. **Counts-only arity check.** A row naming the wrong callback still
   balanced the totals. Added `check_rows()` and cid-set comparison;
   verified by tampering with a callback name, a callback swap and an
   argument, all three now rejected.
7. **Units verified but never applied.** The first loop accepted 12 actions
   and committed none. Fixed; the journal from that run is kept as
   `journal.pre-commit-fix.jsonl` rather than deleted.
8. **Non-idempotent splice.** Re-applying an identical body rewrote the
   opening brace onto the first body line, creating spurious diffs.
9. **Inconsistent `(void)` handling.** `apply_body` rewrote it but
   `check_bodies` did not, so a correct file failed verification.
10. **Model drift, real.** The first live unit was rejected because the
    model wrapped the `id` in `N_()`. The prompt's own example caused it
    (it showed `N_("id")`); corrected to a bare literal and the unit passed.
11. **Colliding include guard (found last, and silent).**
    `gschem/include/actions.h` was a stale WIP file declaring only 48 of 135
    actions, and it used the same `GSCHEM_ACTIONS_H` guard as the generated
    header. Whichever the compiler found first won, so a header claiming to
    declare the registry could be silently replaced by an incomplete one —
    with no compile error, because all 48 it declares are real. Verified
    that nothing included it and that it was not installed, then deleted it,
    and added `check_include_guard()` so any future duplicate fails `verify`
    (tested by re-introducing the collision). A clean rebuild from scratch
    passes with 0 errors.

## Drift audit

On the tree as checked in, the file is the generator's output and matches the
oracle exactly for all 135 actions; the model stage never edits the file
except to write a body it has just verified equal to the oracle.

### Model stage outcome

133 of 135 actions independently reproduced by the model and verified equal
to the oracle. Two were rejected on every attempt, both with the body
correct and the metadata wrong:

| action               | oracle            | model produced    | defect                                   |
|----------------------|-------------------|-------------------|------------------------------------------|
| `add_bus`            | `N_("B_us")`      | `N_("_B_us")`     | mnemonic underscore moved to the front   |
| `options_rubberband` | `"… which are " "attached to them"` | `"… which are \"attached to them"` | stray `\"` inside the literal — would not compile |

`add_bus` was confirmed against `actions.c.bak`, where the menu label is
`_("B_us")`; the generated row is correct. The `options_rubberband` reply
was inspected with its raw JSON to rule out a comparison artefact: the
model really does emit the backslash-quote. Both are exactly the class of
error the oracle comparison exists to catch — a one-character change to a
menu mnemonic is invisible in review and is a real user-facing change, and
a stray escape is invalid C that a naive string-diff review would miss.

Both actions remain in place from the deterministic generator, so nothing is
missing from the tree. They are unverified rather than broken.

### Comparison bugs found in the tooling

The first spec comparison was whitespace-sensitive and reported a legitimate
reformatting as drift. It now normalises whitespace and treats the two
equivalent ways of breaking a string across lines — juxtaposition (`"a"
"b"`) and a backslash continuation — as equal, while still rejecting a
changed mnemonic, a wrong callback or a changed word. Tested against a
synthetic case and against the real replies.

### Process defects found in the tooling

- The first live unit was rejected because the model wrapped the `id` in
  `N_()`. The prompt's own example showed `N_("id")`, so this was our bug;
  corrected, after which the unit passed.
- A rejected unit was retried on every resume, burning budget on a
  deterministic miss. `--resume` now skips rejected cids unless `--retry`
  is given.

No drift has been accepted into the tree.

## Deliberately deferred

- **Tier C** (external executable owner) — contract specified in
  `gschem/src/TRANSFORM_PROTOCOL.md`, not implemented.
- **De-Guile of the menu hot path** — *investigated, deliberately not done.*
  See "Why the menu builder still reads Scheme" in
  `gschem/src/TRANSFORM_PROTOCOL.md`. Two independent blockers: the default
  `help-menu` contains 4 Tier B actions that are not in the C table, so a
  C-only builder cannot reproduce the stock UI; and `menubar`/
  `context-menu`/`toolbar` are a documented user-editable interface in
  `system-gschemrc.scm`, so replacing the Scheme walk with a fixed layout
  would be a behaviour regression. Guile is already off the registration
  and activation paths, which is where it mattered.
- **GTK3 port fallout** — untouched; out of scope for this refactor.
- **Per-body documentation (P3)** — `TRANSFORM_PROTOCOL.md` plus the
  `/*! \brief */` comments retained on 29 of 135 callbacks.
- **`x_run_transform()`** — the Tier C seam is documented, not stubbed, to
  avoid an unused function in the tree.

## AI Assist entry point

Added outside the refactor, after it was closed, so that the gEDA base can be
used with an assistant before any model call exists.

- New modeless dialog (`gschem/src/gschem_ai_assist_dialog.c`) with two
  tabs: the current page rendered as gEDA text, and a count of objects,
  components and `model-name`/`device` attributes with the command needed to
  netlist it. It does not call any API.
- The schematic text comes from `o_save_buffer (s_page_objects (page))`, so
  it is the *edit buffer*: unsaved changes are included and nothing is
  written to disk. A Refresh button re-reads the page.
- Registered through `gschem_action_register()`, not the `action_specs`
  table. `actions.h` is generated in full by `gedai.refactor.gen`, so a
  hand-added row would be destroyed on the next generation and would also
  break the 135-action count. The action and its callback are declared in
  `gschem/include/gschem_ai_assist.h` for the same reason.
- `&ai-assist` is exported automatically by `gschem_action_register()`, so
  `system-gschemrc.scm` puts it in `tools-menu` (last section) and a user can
  move or remove it like any other action. `gschem_action_init()` runs before
  rc parsing, so the binding exists when the menu is read.
- `validators.strip_extra_actions()` filters callbacks whose cid is not in
  the inventory, by name, before the count/name/body checks. Filtering by
  name rather than by position keeps the file layout free to change.
  Confirmed the checks still fail on an injected body change and on a
  deleted table row.

Known limitation: SPICE *netlisting* is not done by the dialog. `gaf netlist`
is not registered as a command, the installed xorn `gnetlist` wrapper is
missing `xorn.command`, and `gnetlist-legacy -g spice-sdb` produced a 7-line
file on the test schematic because it could not resolve `transistor.sym`.
Rather than show a silently wrong netlist, the report describes readiness and
prints the command to run after saving.

## Headless prompting

`--control-fd` already existed as a live, line-oriented channel (`visit`,
`save`, `close`, `quit`, ...) but had no way to ask a question. Added one
command rather than a one-shot `--prompt_file`, so a single running gschem
serves many prompts — which is what an agent loop needs.

    gschem design.sch --control-fd=3 3<>pipe

    ai-prompt /abs/prompt.txt
    ai-prompt: wrote /abs/prompt.txt.out

- `ai-prompt FILE` reads the prompt, bundles it with the schematic as gEDA
  text and the SPICE report, and writes `FILE.out` next to it. Verified on
  `gnetlist-legacy/tests/common/TwoStageAmp.sch`: 162 objects, 36
  components, 1 `model-name`, 21 `device`, and 38 `C` records, matching the
  source file's 38.
- The bundle's section markers (`=== PROMPT ===`, `=== REPORT ===`,
  `=== SCHEMATIC ===`) are deliberately not translated, so a caller can
  parse them regardless of locale.
- The result goes to stderr, matching every other line of this protocol, so
  it cannot interleave with a caller's own stdout when the control fd is
  stdin. Two prompts in one session write two distinct files.
- Errors are reported and do not kill the session: a missing prompt file and
  a wrong argument count both leave gschem running.

### Bugs found and fixed on the way

Three of these blocked headless operation, so they were fixed rather than
worked around.

- **`HAVE_GETOPT_LONG` was never defined.** The `getopt.h` / `getopt_long`
  checks existed in `configure.ac.orig` but had been dropped from
  `configure.ac`, so `parsecmd.c` compiled the short-option path and *every*
  long option was rejected — `--control-fd`, `--help`, `--quiet`,
  `--verbose` alike. Restored both checks, then had to run `autoheader` as
  well: without it the define never reached `config.h.in` and the fix
  silently did nothing. Confirmed the only `config.h` delta is the two new
  defines.
- **Dockable lookup always returned NULL.** `gschem_dockable.c` stored the
  `GschemDockable` in the GtkWidget `"user-data"` *property*, which is
  construct-only in GTK3, so both the set and the get raised a critical and
  every `get_dockable_by_widget()` returned NULL. Switched to
  `g_object_set_data()`/`g_object_get_data()`. The `dockable != NULL` and
  `user-data` criticals went from 7 in a run to 0. One of these criticals
  fired *inside* a critical handler, which GLib escalates to `abort()` —
  this was the actual cause of gschem dying at startup with
  `--control-fd`.
- **`can_read` abused `errno` and ignored its `condition`.** A hung-up fd
  reports `G_IO_HUP` together with `G_IO_IN`, which was never checked, and
  `getline()` on a closed stream was allowed to run. It also `fclose()`d the
  stream while the source was still attached. Now handles HUP/ERR/NVAL
  first, distinguishes EOF (`len == -1` with `errno == 0`) from a real
  error, and detaches the source before closing the stream;
  `x_controlfd_free()` is idempotent and owns the close.

Still not done: the dialog and this command do not call a model. They produce
the bundle and stop, so the feature works offline and is testable.

### Known crash on `quit`

`quit` segfaults during teardown. The backtrace is
`process_command` -> `x_window_close_all` -> `x_window_close` ->
`gschem_toplevel_free` -> `g_object_unref` -> `eda_renderer_finalize` ->
`cairo_surface_destroy`, so it is renderer/cairo teardown, not the control
channel. It reproduces on unmodified HEAD (verified by stashing these fixes
and rebuilding), so it is pre-existing. It does not affect the AI Assist
path: `ai-prompt` has already written its output and flushed before `quit`
is reached. Leave the session running rather than quitting it, and no crash
occurs.

## Startup hygiene: a blank sheet should start quietly

A user with a blank idea opens gschem with no arguments. That path created an
in-memory untitled page and ran, but it printed 125 Gtk/Atk/GLib diagnostics
and `-V` printed autoconf placeholders instead of a version. All of the below
are pre-existing at `ckpt/9` and unrelated to AI Assist.

### `-V` printed `gEDA @PACKAGE_DOTTED_VERSION@ (g@PACKAG)`

`version.h` is generated through `AC_CONFIG_HEADERS([config.h version.h])`,
but `AX_GIT_VERSION` only `AC_SUBST`s `DOTTED_VERSION`/`DATE_VERSION` while
the header wants the `PACKAGE_`-prefixed names, so two of the three macros
never resolved. `version.h.in` also declared them as
`#define X "@X@"` rather than `#undef X`, which is the form `config.status`
actually substitutes. `configure.ac` now derives and substitutes both
`PACKAGE_DOTTED_VERSION` and `PACKAGE_GIT_COMMIT`, and the template uses
`#undef` plus a stray `#undef` that sat outside the include guard is gone.
Verified from a clean `git archive` tree: `gEDA 1.0.0 (g5d185da)`.

### 97 `Gtk-WARNING`s: two children in one `GtkMenuItem`

`gschem_action_create_menu_item()` built a `GtkBox` holding an icon and a
label and added it to the menu item, then fell through to a second block that
created a `GschemAccelLabel` and added that too. A `GtkMenuItem` is a
`GtkBin`, so the second add failed for every icon-bearing action. The label is
now created only when there is no icon, and the dispatcher label updates are
guarded on `label != NULL`.

### 28 `Gtk-CRITICAL`s: compselect widgets that were never created

`preview_expander`, `attribs_expander`, `preview_box`, `attribs_box`,
`preview_paned` and `attribs_paned` were declared in the header and freed in
`dispose()` but never assigned anywhere, so they were permanently `NULL` and
`compselect_restore_internal_geometry()` passed them to
`gtk_paned_set_position()`/`gtk_expander_set_expanded()`. They are now
created in `compselect_create_widget()`, every read is guarded, and the three
`gtk_bin_get_child (GTK_BIN (compselect->top))` calls -- `top` is a `GtkBox`,
not a `GtkBin` -- go through a new `compselect_get_only_child()` helper.

### `Atk-CRITICAL`: `atk_object_set_name (aobj, NULL)`

`gschem_accel_label_set_accel_string()` cleared the accessible name by passing
`NULL`, which ATK asserts against. It now passes `""`, and both branches check
the `AtkObject` for `NULL`.

### `GLib-GObject-CRITICAL`: `GtkFrame` has no `shadow`

GTK3 renamed the property to `shadow-type`; that is what
`gschem_multiattrib_dockable.c` uses now.

### `GLib-GObject-CRITICAL`: pagesel instance size smaller than parent

`GschemPageselDockable` is registered with `g_type_register_static
(GSCHEM_TYPE_DOCKABLE, ...)` but its structs declared `GschemDialog` as the
parent, so the instance struct was smaller than `GschemDockable` and GType
refused the registration. The header now matches the registration.

### Result

`gschem` with no arguments and `--control-fd` both start and stay up with 0
criticals. The only diagnostics left are two environment warnings from this
Homebrew install, both missing icon assets and not code defects:
`check-symbolic.svg` is absent from Adwaita and `gtk-new-ltr` is missing from
hicolor. `g_rc_parse_gtkrc()` no longer emits its two "deprecated" warnings;
it was building paths it never read.


## Not done, and why

The API run is bounded by the 00:00Z stop and a token budget. It completed
133 of 135 actions within ~124k tokens; the two misses are model errors, not
missing work. Every action is in the file from the deterministic generator,
so an unvisited action is unverified, not absent. `status` reports verified
vs remaining, and `run --resume --retry` continues where it stopped.


## GTK2 removal: gattrib's bundled GtkSheet replaced (2026-10-18)

GTK2 is no longer supported anywhere in the tree. When this work started the
only remaining GTK2 code was `gattrib`: the vendored GTK-extra `GtkSheet`
widget (`gtksheet_2_2.c`, 8,667 lines + 871-line header) with its marshaller.
Against GTK3 it produced 757 compile errors, including 81 direct-draw
(`gdk_draw_*`/`gdk_gc_*`) sites that a port would have had to rewrite in
cairo, so it was replaced rather than converted.

### What replaced it

`gattrib_sheet.{c,h}` is a small GTK3 widget: a `GtkBox` containing a
`GtkTreeView` over a `GtkListStore`. Column 0 is the read-only component name;
each attribute is one editable text column. That mirrors what gattrib actually
needed, which the investigation showed was only 12 API entry points -- the
original count of "182 call sites" was inflated by gtksheet's internal
self-calls.

Cell colour still encodes visibility, preserving the documented meanings:
`GREY` hidden, `BLUE` name=value, `RED` name only, `BLACK` value only. The
colours live in their own model columns so setting text never clobbers them.

### Dead code removed

Nets and Pins sheets were behind `#ifdef UNIMPLEMENTED_FEATURES`, a macro never
defined anywhere, so `sheets[1]` and `sheets[2]` were always NULL while
`s_visibility.c` and `x_dialog.c` dereferenced them and relied on
`g_return_if_fail` to survive. `NUM_SHEETS` is now 1 and the switch arms for
pages 1 and 2 remain as no-ops.

`s_visibility.c`'s four `switch (sheet->state)` bodies read GtkSheet's private
`range`/`active_cell`/`state` fields. They now share one helper built on
`gattrib_sheet_get_selection()`, with the active cell treated as a one-cell
selection.

### Pre-existing bug fixed along the way

`gattrib` never installed libgeda's rendered-text-bounds callback, so every
text object raised `world_get_text_bounds: assertion
'toplevel->rendered_text_bounds_func != NULL' failed` -- 62 criticals on a
single small schematic. gattrib has no page view, so `x_gtksheet.c` now
supplies a Pango-based measurement (created from the default font map, since a
bare `pango_context_new()` has no font map and raises its own criticals).

### Verification

- `gattrib`, `libgeda`, `libgedacairo`, `gaf`, `gschem`, `gsymcheck`: 0 errors.
- `make check` in `gattrib`: `PASS: test-gattrib-sheet`, 41 assertions covering
  the cell model, colour/text independence, column append and delete, row
  resize, and selection.
- `gattrib <schematic>` starts, stays up, and logs 0 criticals (down from 62).
- No `gtk_sheet_*`/`GtkSheet` references remain in compiled code; only comments.

### Not verified

Visual parity -- column widths, exact colours and in-cell editing behaviour -- needs a human looking at a real design. The tests cover the data model, not
appearance. `gtkextra-marshal` was also removed; nothing outside gtksheet used
it.

