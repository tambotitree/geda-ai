#!/bin/sh
# -touch build/config.rpath
aclocal -I m4
autoreconf --install --force --verbose
automake --add-missing --copy >/dev/null 2>&1
