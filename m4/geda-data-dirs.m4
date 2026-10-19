# geda-data-dirs.m4                                     -*-Autoconf-*-
# serial 1.0

dnl gEDA data and configuration directories
dnl Copyright (C) 2009  Peter Brett <peter@peter-b.co.uk>
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

# Check where gEDA data and configuration should be stored.
AC_DEFUN([AX_DATA_DIRS],
[
  AC_PREREQ([2.69])dnl

  # Match Automake's default source archive directory, including release.
  GEDA_PACKAGE_DIR="$PACKAGE_TARNAME-$PACKAGE_VERSION"
  AC_SUBST([GEDA_PACKAGE_DIR])
  AC_DEFINE_UNQUOTED([GEDA_PACKAGE_DIR], ["$GEDA_PACKAGE_DIR"],
    [Release-specific shared data directory name.])
  GEDADATADIR='${datarootdir}/'"$GEDA_PACKAGE_DIR"
  GEDAIDATADIR="$GEDADATADIR"
  AC_MSG_CHECKING([where to install gEDA shared data])
  AC_MSG_RESULT([$GEDADATADIR])

  # Do not inherit a stale rc directory from a previous configure run.
  GEDARCDIR="$GEDADATADIR"
  AC_ARG_WITH([rcdir],
    AS_HELP_STRING([--with-rcdir=DIR], [install system configuration in DIR]),
    [case "$withval" in
       yes) GEDARCDIR='${sysconfdir}/'"$GEDA_PACKAGE_DIR" ;;
       no) ;;
       *) GEDARCDIR="$withval" ;;
     esac])
  AC_MSG_CHECKING([where to install gEDA rc files])
  AC_MSG_RESULT([$GEDARCDIR])

  # Fully expand configure directory variables only for C string defaults.
  geda_saved_prefix="$prefix"
  geda_saved_exec_prefix="$exec_prefix"
  test "x$prefix" != xNONE || prefix="$ac_default_prefix"
  test "x$exec_prefix" != xNONE || exec_prefix='$prefix'
  # Now define some preprocessor symbols with the *expanded* values
  GEDADATADIR_expand=`eval "echo $GEDADATADIR"`
  GEDADATADIR_expand=`eval "echo $GEDADATADIR_expand"`
  GEDADATADIR_expand=`eval "echo $GEDADATADIR_expand"`
  AC_DEFINE_UNQUOTED([GEDADATADIR], ["$GEDADATADIR_expand"],
    [Define to gEDA/gaf shared data directory.
Only libgeda should use this - apps should use s_path_sys_data()])

  if test "x$GEDARCDIR" != "x"; then
    GEDARCDIR_expand=`eval "echo $GEDARCDIR"`
    GEDARCDIR_expand=`eval "echo $GEDARCDIR_expand"`
    GEDARCDIR_expand=`eval "echo $GEDARCDIR_expand"`
    AC_DEFINE_UNQUOTED([GEDARCDIR], ["$GEDARCDIR_expand"],
      [Define to gEDA/gaf rc directory if different from GEDADATADIR.
Only libgeda should use this - apps should use s_path_sys_config()])

  else
    GEDARCDIR=$GEDADATADIR
  fi

  AC_DEFINE_UNQUOTED([PKGDATADIR], ["$GEDADATADIR_expand"],
    [Release-specific package data directory.])
  prefix="$geda_saved_prefix"
  exec_prefix="$geda_saved_exec_prefix"
  AC_SUBST([GEDAIDATADIR])
  AC_SUBST([GEDADATADIR])
  AC_SUBST([GEDARCDIR])

])dnl AX_DATA_DIRS
