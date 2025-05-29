#!/bin/sh

# Clean up autotools cache directories and files
echo "Cleaning up autotools cache..."
rm -rf autom4te.cache
rm -f config.cache
rm -rf tmpwrk*
rm -rf intl
# autopoint should manage this, but clean for good measure:w
rm -f m4/gettext.m4
rm -f m4/iconv.m4
rm -f m4/intlmacosx.m4
rm -f m4/lib-ld.m4
rm -f m4/lib-link.m4
rm -f m4/lib-prefix.m4
rm -f m4/nls.m4
rm -f m4/po.m4
rm -f m4/progtest.m4
rm -f po/Makefile.in.in # Though autopoint should manage this, clean for good measure


# Run autopoint first to ensure m4 macros and the intl directory are set up.
echo "Running initial autopoint..."
autopoint --force || { echo "autopoint failed (initial)"; exit 1; }

# Note: If AM_GNU_GETTEXT([external]) is used in configure.ac,
# autopoint will correctly NOT create intl/Makefile.in.
# The intl/ directory and its contents are not part of the build
# when using an external libintl.

# Mimic autogen.sh's autopoint_fix for intl/ChangeLog
# This is a workaround for some autopoint versions needing intl/ChangeLog for 'make dist'
if [ -d "intl" ] && [ ! -f "intl/ChangeLog" ]; then
    echo "Creating intl/ChangeLog..."
    touch intl/ChangeLog || { echo "touch intl/ChangeLog failed"; exit 1; }
else
    if [ ! -d "intl" ]; then
        echo "Warning: intl/ directory does not exist, cannot create intl/ChangeLog."
    fi
fi

# Run desktop-i18n setup (from autogen.sh)
# This script might call autopoint again if AM_GNU_GETTEXT is used.
if [ -f "build-tools/desktop-i18n" ]; then
    echo "Running desktop-i18n setup..."
    ./build-tools/desktop-i18n --setup || { echo "desktop-i18n --setup failed"; exit 1; }
else
    echo "Warning: build-tools/desktop-i18n not found, skipping."
fi

echo "Running libtoolize..."
# On macOS, glibtoolize is common. autoreconf log shows glibtoolize.
if command -v glibtoolize >/dev/null; then
    LIBTOOLIZE=glibtoolize
else
    LIBTOOLIZE=libtoolize
fi
$LIBTOOLIZE --force --copy || { echo "$LIBTOOLIZE failed"; exit 1; }


echo "Running aclocal..."
aclocal -I m4 --force || { echo "aclocal failed"; exit 1; }

echo "Running autoheader..."
autoheader --force || { echo "autoheader failed"; exit 1; }

echo "Running autoconf..."
autoconf --force || { echo "autoconf failed"; exit 1; }

echo "Running automake..."
# Using flags similar to autogen.sh
automake -Wall --add-missing --copy --gnu || { echo "automake failed"; exit 1; }

# Handle xorn subdirectory as autoreconf did previously
if [ -d "xorn" ]; then
    echo "Running autoreconf for xorn..."
    (cd xorn && autoreconf -ifv) || { echo "autoreconf for xorn failed"; exit 1; }
fi

echo "Bootstrap complete."
