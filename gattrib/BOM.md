# BOM / distributor enrichment in gattrib

gattrib can fill in the component table from a parts supplier: Edit ->
**Fetch BOM data** asks the supplier about every part in the design and writes
the manufacturer part number, price and datasheet link into attribute
columns.

## What it writes

For each row the first non-empty of these columns is taken as the part number
to search for, in order:

1. `mpn`
2. `part_number`
3. `value`

Results go into these attribute columns, which are created if the design does
not already have them:

| column         | contents                                   |
|----------------|--------------------------------------------|
| `mpn`          | manufacturer part number                    |
| `manufacturer` | manufacturer name                           |
| `price`        | best offer, e.g. `USD 0.4200 @1 (12500 in stock)` |
| `datasheet`    | datasheet URL                               |
| `lifecycle`    | `Active`, `Obsolete`, ...                   |

Cells that already have a value are **not** overwritten, so a price you have
corrected by hand survives a refresh.  Columns are created through the same
path as Edit -> Add new attrib column, so they are saved with the schematic
like any other attribute.

Nothing is written to disk until you save, and the design is flagged as
modified so gattrib prompts you on exit.

## Configuration

The Octopart API moved to Nexar in 2022; this client therefore talks to
`https://api.nexar.com/graphql` using an OAuth2 client-credentials token from
`https://identity.nexar.com/connect/token`.  Credentials come from the
environment and are never written into the schematic or the exported CSV.

| variable                        | meaning                                   |
|---------------------------------|-------------------------------------------|
| `GATTRIB_NEXAR_CLIENT_ID`       | OAuth2 client id                          |
| `GATTRIB_NEXAR_CLIENT_SECRET`   | OAuth2 client secret                      |
| `GATTRIB_BOM_CURRENCY`          | ISO 4217 code for prices (default `USD`)  |
| `GATTRIB_BOM_COUNTRY`           | country for availability pricing (default `US`) |
| `GATTRIB_BOM_OFFLINE`           | if set, use the built-in offline supplier |

With no credentials (or with `GATTRIB_BOM_OFFLINE` set) the **offline
supplier** is used.  It knows a handful of parts (`LM555CN`, `NE555P`,
`LM317T`) and returns fixed data, which makes it possible to try the feature,
and to run the tests, without an account.

## Running from the source tree

libgeda finds its menus and component library through `GEDADATA`, and the
compiled-in default (`/usr/local/share/geda`) usually does not exist.  Source
the helper script first:

    . ./gattrib_env.sh
    gattrib gattrib/tests/bom_test.sch

`gattrib_env.sh` sets `GEDADATA`/`GEDADATARC` from `GEDA_PREFIX` (default
`/Users/johnryan/geda`) and puts the uninstalled binaries on `PATH`.

gattrib reads its menu definition from `$GEDADATA/gattrib-menus.xml`.  After
editing `gattrib/lib/gattrib-menus.xml` you must install it (`make install` in
`gattrib/lib`, or copy the file) or the new menu item will not appear.

`GATTRIB_DEBUG_DUMP=1` prints the loaded tables to stderr on startup, which is
the quickest way to check that a schematic parsed as expected.

## Structure

| file                            | role                                    |
|---------------------------------|-----------------------------------------|
| `gattrib_json.c`                | small dependency-free JSON parser       |
| `gattrib_bom.c`                 | supplier interface, Octopart/Nexar client, offline supplier |
| `gattrib_bom_hook.c`            | decides which columns to read and write |
| `gattrib_bom_hook.h`            | the hook's interface, including the two injected callbacks |

All network access goes through a `GattribBomTransport` function pointer, so
the client can be driven from tests with canned responses -- no sockets and no
credentials.  The supplier itself is an opaque struct of function pointers, so
adding Mouser, Digi-Key or a local cache does not touch the sheet code.

The hook takes two injected callbacks (`gattrib_bom_hook_set_add_column_func`
and `gattrib_bom_hook_set_touch_func`) rather than calling gattrib's globals
directly, which is what lets `test-gattrib-bom-hook` link without the GUI.

## Tests

    cd gattrib/src && make check

- `test-gattrib-sheet` -- the attribute matrix widget
- `test-gattrib-bom` -- JSON parsing, config, both suppliers, best-offer rule
- `test-gattrib-bom-hook` -- which column is used, overwrite rules, stats

None of them need a display, a network or credentials.
