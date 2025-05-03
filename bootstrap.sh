#!/bin/sh

# Clean up autotools cache directories and files
echo "Cleaning up autotools cache..."
rm -rf autom4te.cache
rm -f config.cache

# -touch build/config.rpath
aclocal -I m4
autoreconf --install --force --verbose
automake --add-missing --copy >/dev/null 2>&1
