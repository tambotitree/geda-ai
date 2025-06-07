# Process this file with autoconf to produce a configure script.
# Copyright (C) 2013-2020 Roland Lutz
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([xorn], [1.0.0], [https://github.com/tambotitree/geda-ai/issues])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])
AC_CONFIG_MACRO_DIR([m4])

# Required for automake Python support (must define PYTHON, etc.)
AM_PATH_PYTHON([3.6])

# gEDA Python environment
m4_include([../m4/geda-python.m4])
GEDA_PYTHON_SETUP
AC_SUBST(PYTHON_CFLAGS)
AC_SUBST(PYTHON_LIBS)
AC_ARG_VAR([PYTHON_CFLAGS], [C compiler flags for Python])
AC_ARG_VAR([PYTHON_LIBS], [linker flags for Python])

AC_CONFIG_SRCDIR([src/backend/gnet_geda.py])
AC_CONFIG_HEADERS([config.h])

# Required compilers and tools
AC_PROG_CC
AC_PROG_CXX
AM_PROG_AR
LT_INIT

# Required for building shared libraries with libtool and Python modules
PKG_PROG_PKG_CONFIG

# Check for Guile for xorn (trying 3.0 then 2.0)
AC_MSG_CHECKING([for Guile for xorn (trying 3.0 then 2.0)])
_found_guile_for_xorn=no
PKG_CHECK_MODULES([XORN_GUILE_CHECK], [guile-3.0 >= 2.2],
    [
        _found_guile_for_xorn=yes
        GUILE_CFLAGS="$XORN_GUILE_CHECK_CFLAGS"
        GUILE_LIBS="$XORN_GUILE_CHECK_LIBS"
        GUILE_EFFECTIVE_VERSION="3.0"
        GUILE_PKG_NAME="guile-3.0"
        AC_MSG_RESULT([found Guile 3.0])
    ],
    [
        PKG_CHECK_MODULES([XORN_GUILE_CHECK], [guile-2.0 >= 2.0.10],
            [
                _found_guile_for_xorn=yes
                GUILE_CFLAGS="$XORN_GUILE_CHECK_CFLAGS"
                GUILE_LIBS="$XORN_GUILE_CHECK_LIBS"
                GUILE_EFFECTIVE_VERSION="2.0"
                GUILE_PKG_NAME="guile-2.0"
                AC_MSG_RESULT([found Guile 2.0])
            ],
            [:])
    ])

if test "x$_found_guile_for_xorn" = "xyes"; then
    AC_SUBST(GUILE_CFLAGS)
    AC_SUBST(GUILE_LIBS)
    AC_SUBST(GUILE_EFFECTIVE_VERSION)
    AC_SUBST(GUILE_PKG_NAME)
    AC_DEFINE(HAVE_GUILE, 1, [Define if Guile is available])
else
    AC_MSG_RESULT([no suitable Guile found])
    AC_MSG_ERROR([Guile (>= 3.0 or >= 2.0.10) is required for xorn.])
fi

# Check for specific Guile functions
OLD_CFLAGS="$CFLAGS"
OLD_LIBS="$LIBS"
CFLAGS="$CFLAGS $GUILE_CFLAGS"
LIBS="$LIBS $GUILE_LIBS"
AC_CHECK_FUNCS([scm_frame_procedure_name scm_procedure_name])
CFLAGS="$OLD_CFLAGS"
LIBS="$OLD_LIBS"

# gettext support
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

# Look for shared gEDA data (passed from top-level)
AC_MSG_CHECKING([where to look for gEDA shared data])
AC_ARG_VAR([GEDADATADIR], [path to gEDA shared data])
gedadatadir="$GEDADATADIR"
AC_MSG_RESULT([$gedadatadir])
AS_IF([test "x$gedadatadir" = "x"],
      [AC_MSG_ERROR([No gEDA shared data location given. Try re-running configure from top level.])])
AC_SUBST([gedadatadir])

# Look for gEDA rc files (passed from top-level)
AC_MSG_CHECKING([where to look for gEDA rc files])
AC_ARG_VAR([GEDARCDIR], [path to gEDA rc files])
gedasysconfdir="$GEDARCDIR"
AC_MSG_RESULT([$gedasysconfdir])
AC_SUBST([gedasysconfdir])

# Declare output files
AC_CONFIG_FILES([
    Makefile
    po/Makefile.in
    src/storage/Makefile
    src/cpython/storage/Makefile
    src/cpython/guile/Makefile
    src/python/Makefile
    src/gaf/Makefile
    src/command/Makefile
    src/backend/Makefile
    tests/Makefile
    tests/netlist/Makefile
])

AC_OUTPUT