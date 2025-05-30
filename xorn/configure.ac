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

AC_PREREQ([2.69])
LT_PREREQ([2.4.6])

AC_INIT([Xorn], [0.0], [bug-xorn@hedmen.org], [], [http://hedmen.org/xorn/])
AC_CONFIG_SRCDIR([src/cpython/storage/module.c])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror silent-rules subdir-objects])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT([])

AX_PROG_CC_WARNING([cast_function_type], [cast-function-type])

AC_PROG_CXX
AC_MSG_CHECKING([for python2 >= 2.7])
AM_PATH_PYTHON([2.7])
PKG_CHECK_MODULES([PYTHON_PKG], [python-2.7 >= 2.7], [
  PYTHON_CFLAGS="$PYTHON_PKG_CFLAGS"
  PYTHON_LIBS="$PYTHON_PKG_LIBS"
  AC_MSG_RESULT([yes, via pkg-config (python-2.7)])
  have_python=yes
], [
  PKG_CHECK_MODULES([PYTHON_PKG], [python2 >= 2.7], [
    PYTHON_CFLAGS="$PYTHON_PKG_CFLAGS"
    PYTHON_LIBS="$PYTHON_PKG_LIBS"
    AC_MSG_RESULT([yes, via pkg-config (python2)])
    have_python=yes
  ], [
    AC_MSG_RESULT([no, via pkg-config])
    # Fallback to pythonX-config if pkg-config fails
    AC_PATH_PROG([PYTHON_CONFIG], [python2.7-config python2-config], [no])
    if test "x$PYTHON_CONFIG" != "xno"; then
      AC_MSG_CHECKING([for python2 >= 2.7 via $PYTHON_CONFIG])
      PYTHON_CFLAGS=`$PYTHON_CONFIG --includes`
      PYTHON_LIBS=`$PYTHON_CONFIG --libs`
      AC_MSG_RESULT([yes, via $PYTHON_CONFIG])
      have_python=yes
    fi
  ])
])

if test "x$have_python" != "xyes"; then
  AC_MSG_ERROR([
Python 2.7 development files were not found.

Tried pkg-config for 'python-2.7' and 'python2', and also searched for
'python2.7-config' or 'python2-config' scripts.

Please ensure Python 2.7 development headers and libraries are installed
and discoverable (e.g., via PKG_CONFIG_PATH or by having pythonX-config
in your PATH). Alternatively, set PYTHON_CFLAGS and PYTHON_LIBS manually.
])
fi

AC_SUBST(PYTHON_CFLAGS)
AC_SUBST(PYTHON_LIBS)

# Check for Guile for xorn (trying 3.0 then 2.0)
AC_MSG_CHECKING([for Guile for xorn (trying 3.0 then 2.0)])
_found_guile_for_xorn=no
PKG_CHECK_MODULES([XORN_GUILE_CHECK], [guile-3.0 >= 2.2], # Use a reasonable minimum for Guile 3
    [
        _found_guile_for_xorn=yes
        GUILE_CFLAGS="$XORN_GUILE_CHECK_CFLAGS"
        GUILE_LIBS="$XORN_GUILE_CHECK_LIBS"
        GUILE_EFFECTIVE_VERSION="3.0" # Or extract from pkg-config if needed
        GUILE_PKG_NAME="guile-3.0"
        AC_MSG_RESULT([found Guile 3.0])
    ],
    [ # If Guile 3.0 not found, try Guile 2.0
        PKG_CHECK_MODULES([XORN_GUILE_CHECK], [guile-2.0 >= 2.0.10],
            [
                _found_guile_for_xorn=yes
                GUILE_CFLAGS="$XORN_GUILE_CHECK_CFLAGS"
                GUILE_LIBS="$XORN_GUILE_CHECK_LIBS"
                GUILE_EFFECTIVE_VERSION="2.0" # Or extract from pkg-config if needed
                GUILE_PKG_NAME="guile-2.0"
                AC_MSG_RESULT([found Guile 2.0])
            ],
            [:]) # Silent failure if Guile 2.0 also not found
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

# Check for specific Guile functions needed by xorn's C extensions.
# We need GUILE_CFLAGS and GUILE_LIBS to be set for this check to link correctly.
OLD_CFLAGS="$CFLAGS"
OLD_LIBS="$LIBS"
CFLAGS="$CFLAGS $GUILE_CFLAGS"
LIBS="$LIBS $GUILE_LIBS" # Add scm_procedure_name to the check
AC_CHECK_FUNCS([scm_frame_procedure_name scm_procedure_name])
CFLAGS="$OLD_CFLAGS"
LIBS="$OLD_LIBS"

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

# use location passed by main configure script
AC_MSG_CHECKING([where to look for gEDA shared data])
AC_ARG_VAR([GEDADATADIR], [path to gEDA shared data])
gedadatadir="$GEDADATADIR"
AC_MSG_RESULT([$gedadatadir])
AS_IF([test "x$gedadatadir" = "x"],
      [AC_MSG_ERROR([No gEDA shared data location given

==> This sometimes happens when the configuration scripts aren't run in
    the correct order.  Re-running `configure' should solve the issue.
])])
AC_SUBST([gedadatadir])

# use location passed by main configure script
AC_MSG_CHECKING([where to look for gEDA rc files])
AC_ARG_VAR([GEDARCDIR], [path to gEDA rc files])
gedasysconfdir="$GEDARCDIR"
AC_MSG_RESULT([$gedasysconfdir])
AC_SUBST([gedasysconfdir])

AC_CONFIG_HEADERS([config.h])
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
