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

#####################################################################
# PKG NAME WORKAROUND
#####################################################################
# Here we HARDCODE some version name CHECKS. There is a subtle 
# logic flaw in pg-config. It assumes that the package is just the 
# base name without a version, however maintainers are free to choose
# their own name so the package should be guile but in fact it is 
# guie-3.0 and pkg-config can't find the package without the correct
# name.  So we create a list of package dependencies that break the
# paradigm here to centralize management of them. 
GUILE_PKG_NAME="guile-3.0"

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

AM_PATH_PYTHON([2.7])
# Check for Python 2.7 using pkg-config
PKG_CHECK_MODULES([PYTHON], python-2.7 >= 2.7,
  [ : ],  # Action if found: do nothing special, just succeed
  [ AC_MSG_ERROR( # Action if NOT found: print custom error and exit
[Required Python 2.7 development files were not found by pkg-config.

This commonly occurs when Python is installed in a non-standard prefix,
such as with 'pyenv' or 'conda'.

If you are using pyenv and have Python 2.7 installed (e.g., version 2.7.18),
please ensure its pkg-config directory is in your PKG_CONFIG_PATH. Try:

  export PKG_CONFIG_PATH="$HOME/.pyenv/versions/2.7.18/lib/pkgconfig:\${PKG_CONFIG_PATH}"
  ./configure

(Adjust the path '$HOME/.pyenv/versions/2.7.18/lib/pkgconfig' if your
version or installation location differs).

Alternatively, manually set PYTHON_CFLAGS and PYTHON_LIBS environment variables.]
  ) ] # Closing parenthesis for AC_MSG_ERROR and closing bracket for action
) # Closing parenthesis for PKG_CHECK_MODULES

# Updateing Guile to version 3 - LISP is used for routing traces which lisp is not so important
# tambotitree@gmail.com - Thu Mar 13 05:20:57 CDT 2025
# PKG_CHECK_MODULES([GUILE], [guile-2.2],, [
#   PKG_CHECK_MODULES([GUILE], [guile-2.0 >= 2.0.10])
# ])
PKG_CHECK_MODULES([GUILE], [guile-3.0],, [
  PKG_CHECK_MODULES([GUILE], [guile-3.0 >= 3.0.10])
])

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
