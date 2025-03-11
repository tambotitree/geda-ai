# geda-gattrib.m4                                        -*-Autoconf-*-
# serial 1.0

dnl Check for dependencies for gattrib and optionally disable it.
dnl Copyright (C) 2009  Dan McMahill <dan@mcmahill.net>
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# Check if gattrib should be disabled
AC_DEFUN([AX_OPTION_GATTRIB],
[
  AC_PREREQ([2.69])dnl

  AC_MSG_CHECKING([whether to enable gattrib])
  AC_ARG_ENABLE([gattrib],
    [AS_HELP_STRING([--disable-gattrib], [turn off building and installing gattrib])],
    [], [enable_gattrib="yes"])

  if test "X$enable_gattrib" = "Xyes"; then
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
  fi

  AM_CONDITIONAL([ENABLE_GATTRIB], test "X$enable_gattrib" = "Xyes")

])dnl AX_OPTION_GATTRIB
