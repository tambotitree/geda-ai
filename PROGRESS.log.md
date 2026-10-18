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

## Not done, and why

The API run is bounded by the 00:00Z stop and a token budget. It completed
133 of 135 actions within ~124k tokens; the two misses are model errors, not
missing work. Every action is in the file from the deterministic generator,
so an unvisited action is unverified, not absent. `status` reports verified
vs remaining, and `run --resume --retry` continues where it stopped.
