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
