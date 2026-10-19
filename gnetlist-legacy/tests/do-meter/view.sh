#!/bin/sh
# Open the fixture with its component libraries registered before page loading.
set -eu
fixture_dir=$(CDPATH= cd "$(dirname "$0")" && pwd)
cd "$fixture_dir"
exec ../../../gschem/src/gschem "$fixture_dir/do-meter.sch"
