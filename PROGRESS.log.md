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

## Drift audit

On the tree as checked in, the file is the generator's output and matches the
oracle exactly for all 135 actions; the model stage never edits the file
except to write a body it has just verified equal to the oracle.

Drift observed during the run, all resolved:

1. **Prompt-induced, `file_open_recent`.** The model wrapped the `id` in
   `N_()`. The prompt's own example showed `N_("id")`, so this was our bug;
   corrected to a bare literal, after which the unit passed.
2. **Real model error, `add_bus`.** The oracle's menu label is `_("B_us")`;
   the model produced `N_("B_us")` — it moved the mnemonic underscore to the
   front (`_B_us`). Confirmed against `actions.c.bak` that `B_us` is the
   original text and that the generated row is correct. The unit was
   rejected and the action remains unverified pending a retry. This is the
   behaviour the oracle comparison exists for: a one-character change to a
   menu mnemonic is invisible in review and is a real user-facing change.

No drift has been accepted into the tree.

## Deliberately deferred

- **Tier C** (external executable owner) — contract specified in
  `gschem/src/TRANSFORM_PROTOCOL.md`, not implemented.
- **De-Guile of the menu hot path** — menus are still built from the Scheme
  variable in `x_menus.c`; actions no longer depend on the macro pipeline
  for registration, but the menu builder is unchanged.
- **GTK3 port fallout** — untouched; out of scope for this refactor.
- **Per-body documentation (P3)** — `TRANSFORM_PROTOCOL.md` plus the
  `/*! \brief */` comments retained on 29 of 135 callbacks.
- **`x_run_transform()`** — the Tier C seam is documented, not stubbed, to
  avoid an unused function in the tree.

## Not done, and why

The API run is bounded by the 00:00Z stop and a token budget, so it may not
cover all 135 actions. That is safe by construction: every action is already
in the file from the deterministic generator, so an unvisited action is
unverified, not missing. `status` reports verified vs remaining, and
`run --resume` continues where it stopped.
