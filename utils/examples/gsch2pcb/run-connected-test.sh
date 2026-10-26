#!/bin/sh
# Convert a connected fixture using a bundled, nonmanufacturing footprint.
set -eu
example=$(CDPATH= cd "$(dirname "$0")" && pwd)
tree=$(CDPATH= cd "$example/../../.." && pwd)
build=${GEDA_TEST_BUILD_ROOT:-$tree}
work=${1:-$(mktemp -d /tmp/geda-gsch2pcb.XXXXXX)}
mkdir -p "$work"
work=$(CDPATH= cd "$work" && pwd)
cp "$example/connected.sch" "$work/connected.sch"
cat > "$work/gafrc" <<RC
(component-library "$tree/symbols/analog")
(scheme-directory "$tree/libgeda/scheme")
(scheme-directory "$tree/gnetlist-legacy/scheme")
RC
export GNETLIST="$build/gnetlist-legacy/src/gnetlist-legacy"
export GEDADATA="$tree/gnetlist-legacy"
export GEDADATARC="$build/gnetlist-legacy/lib"
export GEDALOG="$work"
export GUILE_AUTO_COMPILE=0
cd "$work"
"$build/utils/src/gsch2pcb" --skip-m4 --elements-dir "$example/packages" connected.sch
printf '\nTest outputs: %s\n' "$work"
