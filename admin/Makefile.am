# In admin/Makefile.am
# Makefile.am for gEDA-AI administrative scripts.
#
# Distributed under the GNU General Public License v2 or later
# Author: gEDA Maintainers
# Original Date: 2025-06-01
#
# This file is part of gEDA-AI, a suite of Electronic Design Automation tools.
# See the file COPYING for copying conditions.
# This file is maintained by the gEDA-AI project.

# Scripts and documentation in this directory.
# geda-update-databases.sh is used by 'make install' from other Makefiles.
# test_geda-update-databases.sh is run during 'make check'.
# geda-update-databases.1 is a man page for the script.

SUBDIRS = tests docs

EXTRA_DIST = \
    geda-update-databases.sh \
    README.md \
    tests/test_geda-update-databases.sh \
    docs/geda-update-databases.1

# Define the test script to be run during 'make check'
TESTS = tests/test_geda-update-databases.sh

# Install the man page for geda-update-databases.sh
# Even though the script itself isn't installed in $(bindir),
# its documentation can be useful for developers and packagers.
man1_MANS = docs/geda-update-databases.1

# No programs or libraries are built or installed directly from this Makefile.
