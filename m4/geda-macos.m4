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



# check for macOS, then create the directories and set the variables.
AC_DEFUN([AX_CONFIGURE_XDG],
[
AC_MSG_CHECKING([for operating system])
case "${host_os}" in
    darwin*)
        AC_MSG_RESULT([macOS])
        AC_MSG_NOTICE([Creating XDG directories and setting XDG environment variables])

        # Create the directories if they don't exist
        AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <sys/stat.h>]],
        [[mkdir("$HOME/.config", 0755);
          mkdir("$HOME/.local", 0755);
          mkdir("$HOME/.local/share", 0755);
          mkdir("$HOME/.local/state", 0755);
          mkdir("$HOME/.cache", 0755);
        ]])],
        [],
        [AC_MSG_ERROR([Failed to create XDG directories])],
        [AC_MSG_ERROR([Failed to create XDG directories])])
        AC_SUBST([XDG_CONFIG_HOME], "$HOME/.config")
        AC_SUBST([XDG_DATA_HOME], "$HOME/.local/share")
        AC_SUBST([XDG_STATE_HOME], "$HOME/.local/state")
        AC_SUBST([XDG_CACHE_HOME], "$HOME/.cache")
        ;;
    *)
        AC_MSG_RESULT([non-macOS])
        ;;
esac
])dnl


