# gEDA-ai TODO

## Current priority: clear blockers

- [ ] Review and repair one build/runtime blocker at a time, keeping explicit
  callback types and all strict compiler diagnostics enabled.
- [x] Finish the gsymcheck blocker fix: enforce strict flags by default,
  rebuild cleanly, and run its existing regression suite (46/46 passed).
- [x] Build gaf with strict diagnostics and pass its four command/export checks.
- [x] Clean-build gattrib with strict diagnostics and pass its three regression tests.
- [x] Verify gattrib menus and editing workflow interactively (user confirmed).
- [x] Clear gnetlist-legacy Guile registration callback type errors; clean strict
  build passes, with 302 regression passes and two existing expected failures.
- [ ] Resolve the TPM footprint pad-width selection before assigning a PCB
  footprint; retain the source drawing's dimensional limits.

## Build walk (2026-10-20)

- [x] Full-tree default build completes on the Apple Silicon development machine.
- [x] Enforce strict diagnostics in utils/src and utils/gschlas; check PCB header
  formats and schematic embed/unembed without changing saved attributes.
- [x] Run utils regressions: 17 refdes_renum and 121 gxyrs cases passed.
- [x] Enforce strict diagnostics for contrib/smash_megafile; clean build passes.
- [ ] Continue strict compilation through the remaining contributed utilities.
- [ ] Clear Xorn Python-extension callback-cast and initializer warnings; its
  default build passes but is not yet warning-clean under the full strict flags.

## GUI icons and conversion follow-up (2026-10-20)

- [x] Bundle GTK applications-science directional icon aliases in gschem.
- [x] Embed gattrib application artwork and share the fixed appearance policy;
  verify icon lookups with desktop icon paths removed.
- [x] Own the six alternative picker-layout widgets until disposal; exercise
  parented/unparented destruction with fatal GTK warnings after initialization.
- [x] Add a connected gsch2pcb example with local test footprints and require
  a populated PCB and exact LINK net in its offline regression.

## Deferred integrations

- [ ] Evaluate PDF datasheet extraction tools, starting with datasheet-cli;
  compare uConfig and datasheet-parser using the verified ATTPM20P pinout.
  Require typed records, source-page provenance, units and dimensional limits.
  Keep electrical pin mapping and physical pad geometry separately reviewable
  before writing gEDA symbols or footprints. Check licensing and configurable
  model-provider support before adopting a dependency.
  - https://github.com/akiselev/datasheet-cli
  - https://github.com/Robotips/uConfig
  - https://github.com/guyco3/datasheet-parser
- [ ] Add DigiKey API integration for part search, datasheets, stock and BOM
  pricing, with configurable credentials and explicit quantity/currency data.

These integrations are deferred while build and core engineering blockers
are being cleared.

## Resize/startup regression — 2026-10-26

- Fixed pre-realize geometry queries: use positive widget allocation dimensions
  instead of requiring a GdkWindow during page construction.
- Applied PCB-ai's owned-buffer presentation pattern to gschem. Render each main
  canvas/preview frame into an owned Cairo image surface and copy to GTK's draw
  context; persistent EdaRenderer/Pango contexts no longer retain GTK's transient
  draw context. Preserve widget positioning, clipping and display scale.
- Verified empty-page and cmos-nand example startup with 16 resize/maximize cycles
  each, fatal criticals enabled, private settings and no design writes.
  Repeat with `python3 tests/test_gschem_resize.py` on macOS.
- Scoped `make -C libgedacairo check` and `make -C gschem/src check` pass.
  Full-project check still has the separately observed 41 Xorn failures.

## Usable startup window — 2026-10-26

- Default to two-thirds of the monitor work-area width/height, with a minimum
  1280x720 client size where the display permits it. Raise old tiny saved sizes,
  retain larger saved sizes, and clamp to the usable display. Use GTK logical
  coordinates so Retina scaling does not inflate the window beyond the monitor.
- Cap remembered dock sizes to one-third of window width/height so stale large
  dock requests cannot consume the canvas or force an oversized main window.
- Rebuilt gschem and verified empty/example startup at 1280x720 on this monitor,
  followed by 16 resize/maximize cycles each, with fatal criticals enabled.
