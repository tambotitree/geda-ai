# geda-macos.m4                                        -*-Autoconf-*-
# serial 1

# Copyright (C) 2024 John Ryan <tambotitree@gmail.com>
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
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# Check for st_mtim vs st_mtimespec
AC_DEFUN([AX_CHECK_ST_MTIMESPEC],
[
  AC_MSG_CHECKING([whether struct stat has st_mtim])
  AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <sys/stat.h>
int main() {
  struct stat buf;
  (void) buf.st_mtim;
  return 0;
}
]])],
    [AC_MSG_RESULT([yes])
     AC_DEFINE([HAVE_ST_MTIM], 1, [Define if struct stat has st_mtim])],
    [AC_MSG_RESULT([no])
     AC_MSG_CHECKING([whether struct stat has st_mtimespec])
     AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <sys/stat.h>
int main() {
  struct stat buf;
  (void) buf.st_mtimespec;
  return 0;
}
]])],
     [AC_MSG_RESULT([yes])
      AC_DEFINE([HAVE_ST_MTIMESPEC], 1, [Define if struct stat has st_mtimespec])],
     [AC_MSG_RESULT([no])
      AC_MSG_ERROR([struct stat has neither st_mtim nor st_mtimespec])])])
])dnl



# We need to add the gnu extension flag here.
# m4_ifdef([AM_PROG_CC_C_O], [AM_PROG_CC_C_O])

# m4_append([AM_PROG_CC_C_O], [
#   m4_append([AM_PROG_CC_C_O], [
# AM_PROG_CC_C_O(-std=gnu99)])])