#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.72])
AC_INIT([geda-ai], [0.9], [https://github.com/tambotitree/geda-ai/issues], [geda-ai])
AC_USE_SYSTEM_EXTENSIONS
AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])


AC_CONFIG_MACRO_DIRS([m4 xorn/m4]) # Ensure m4 directory is listed
AX_DEBUG_TRACE_TEST() # Call our test macro

AC_CONFIG_HEADERS([config.h])

# --- Enable/Disable Arguments ---
AC_ARG_ENABLE([gattrib],
    AS_HELP_STRING([--enable-gattrib], [Enable gattrib (default: yes)]),
    [case "${enableval}" in
        yes) ENABLE_GATTRIB=yes ;;
        no) ENABLE_GATTRIB=no ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-gattrib]) ;;
    esac],
    [ENABLE_GATTRIB=yes])
AM_CONDITIONAL([ENABLE_GATTRIB], [test "$ENABLE_GATTRIB" = "yes"])

AC_ARG_ENABLE([doxygen],
    AS_HELP_STRING([--enable-doxygen], [Enable doxygen documentation (default: no)]),
    [case "${enableval}" in
        yes) ENABLE_DOXYGEN=yes ;;
        no) ENABLE_DOXYGEN=no ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-doxygen]) ;;
    esac],
    [ENABLE_DOXYGEN=no])
AM_CONDITIONAL([ENABLE_DOXYGEN], [test "$ENABLE_DOXYGEN" = "yes"])

# --- Checks for programs ---
AC_PROG_CXX
AM_PROG_AR
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_YACC
AC_PROG_LEX([yywrap])


# Initialize libtool
LT_INIT

# --- Checks for libraries ---
# FIXME: Replace 'main' with a function in '-lm':
AC_CHECK_LIB([m], [main])

# --- Checks for header files ---
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([argz.h fcntl.h inttypes.h langinfo.h libintl.h malloc.h stdint.h stdio_ext.h strings.h sys/param.h sys/time.h unistd.h])

# --- Checks for typedefs, structures, and compiler characteristics ---
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])
dnl # AX_CHECK_ST_MTIMESPEC # Call the macro from geda-macos.m4

# --- Checks for library functions ---
AC_FUNC_CHOWN
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19])
AM_GNU_GETTEXT_REQUIRE_VERSION([0.19])
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_MBRTOWC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify floor getcwd localeconv mblen mbrlen memchr memmove mempcpy memset mkdir munmap nl_langinfo pow rint setlocale sqrt stpcpy strcasecmp strchr strcspn strdup strerror strrchr strstr strtol strtoul])

# --- Feature Conditionals (MUST BE BEFORE AC_OUTPUT) ---

# Check for groff (for HTML docs)
AC_CHECK_PROG(GROFF, groff, yes, no)
AC_ARG_ENABLE(groff-html,
  AS_HELP_STRING([--disable-groff-html],[disable HTML documentation generation via groff]),
  [enable_groff_html=$enableval],
  [enable_groff_html=$GROFF]) # Default: enable if groff is found
AM_CONDITIONAL(ENABLE_GROFF_HTML, test "x$enable_groff_html" = xyes)
if test "x$enable_groff_html" = xyes; then
  AC_DEFINE(ENABLE_GROFF_HTML, 1, [Define if HTML documentation generation via groff is enabled])
fi

# Check if we are building from a git repository
AC_MSG_CHECKING([whether we are in a git repository])
if test -d "$srcdir/.git"; then
  have_git_repo=yes
else
  have_git_repo=no
fi
AC_MSG_RESULT([$have_git_repo])
AM_CONDITIONAL(HAVE_GIT_REPO, test "x$have_git_repo" = xyes)
if test "x$have_git_repo" = xyes; then
  AC_DEFINE(HAVE_GIT_REPO, 1, [Define if building from a git repository])
fi

# Check for XDG utils (for desktop integration database updates)
AC_CHECK_PROG(UPDATE_MIME_DATABASE, update-mime-database, yes, no)
AC_CHECK_PROG(UPDATE_DESKTOP_DATABASE, update-desktop-database, yes, no)
AC_ARG_ENABLE(update-xdg-database,
  AS_HELP_STRING([--disable-update-xdg-database],[disable updating XDG database (default: yes, if tools found)]),
  [enable_update_xdg_database=$enableval],
  [enable_update_xdg_database=yes]) # Default: yes
if test "x$enable_update_xdg_database" = xyes; then
  if test "x$UPDATE_MIME_DATABASE" = xno; then
    AC_MSG_WARN([update-mime-database not found, disabling XDG database update])
    enable_update_xdg_database=no
  fi
  if test "x$UPDATE_DESKTOP_DATABASE" = xno; then
    AC_MSG_WARN([update-desktop-database not found, disabling XDG database update])
    enable_update_xdg_database=no
  fi
fi
AM_CONDITIONAL(ENABLE_UPDATE_XDG_DATABASE, test "x$enable_update_xdg_database" = xyes)

# Check for KDE desktop data installation
kde_default=yes
case "${host_os}" in
  darwin*)
    AC_MSG_NOTICE([Disabling KDE desktop data installation by default on macOS])
    kde_default=no
    ;;
esac
AC_ARG_ENABLE(kde-desktop-data,
  AS_HELP_STRING([--enable-kde-desktop-data],[enable installation of KDE desktop data (default: disabled on macOS)]),
  [enable_kde_desktop_data=$enableval],
  [enable_kde_desktop_data=$kde_default]) # Default depends on OS detection
if test "x$enable_kde_desktop_data" = xyes; then
   AC_MSG_WARN([KDE desktop data installation enabled. Ensure KDE tools/libs are available if needed.])
fi
AM_CONDITIONAL(ENABLE_KDE_DESKTOP_DATA, test "x$enable_kde_desktop_data" = xyes)


# --- Configuration Files and Output (MUST BE AT THE END) ---
AC_CONFIG_FILES([Makefile
                 contrib/Makefile
                 contrib/gmk_sym/Makefile
                 contrib/olib/Makefile
                 contrib/sarlacc_schem/Makefile
                 contrib/scripts/Makefile
                 contrib/smash_megafile/Makefile
                 docs/Makefile
                 docs/scheme-api/Makefile
                 docs/toplevel/Makefile
                 docs/wiki/Makefile
                 examples/Makefile
                 examples/RF_Amp/Makefile
                 examples/RF_Amp/model/Makefile
                 examples/RF_Amp/sym/Makefile
                 examples/TwoStageAmp/Makefile
                 examples/gTAG/Makefile
                 examples/lightning_detector/Makefile
                 gaf/Makefile
                 gattrib/Makefile
                 gattrib/data/Makefile
                 gattrib/design/Makefile
                 gattrib/docs/Makefile
                 gattrib/include/Makefile
                 gattrib/lib/Makefile
                 gattrib/src/Makefile
                 gnetlist-legacy/Makefile
                 gnetlist-legacy/docs/Makefile
                 gnetlist-legacy/docs/vams/Makefile
                 gnetlist-legacy/examples/Makefile
                 gnetlist-legacy/examples/switcap/Makefile
                 gnetlist-legacy/examples/vams/Makefile
                 gnetlist-legacy/examples/vams/vhdl/Makefile
                 gnetlist-legacy/examples/vams/vhdl/basic-vhdl/Makefile
                 gnetlist-legacy/examples/vams/vhdl/new-vhdl/Makefile
                 gnetlist-legacy/include/Makefile
                 gnetlist-legacy/lib/Makefile
                 gnetlist-legacy/scheme/Makefile
                 gnetlist-legacy/scripts/Makefile
                 gnetlist-legacy/src/Makefile
                 gnetlist-legacy/tests/Makefile
                 gnetlist-legacy/utils/Makefile
                 gschem/Makefile
                 gschem/bitmap/Makefile
                 gschem/data/Makefile
                 gschem/docs/Makefile
                 gschem/examples/Makefile
                 gschem/include/Makefile
                 gschem/lib/Makefile
                 gschem/scheme/Makefile
                 gschem/scripts/Makefile
                 gschem/src/Makefile
                 gschem/tests/Makefile
                 gsymcheck/Makefile
                 gsymcheck/docs/Makefile
                 gsymcheck/include/Makefile
                 gsymcheck/lib/Makefile
                 gsymcheck/src/Makefile
                 gsymcheck/tests/Makefile
                 libgeda/Makefile
                 libgeda/data/Makefile
                 libgeda/docs/Makefile
                 libgeda/docs/images/Makefile
                 libgeda/include/Makefile
                 libgeda/lib/Makefile
                 libgeda/scheme/Makefile
                 libgeda/src/Makefile
                 libgedacairo/Makefile
                 symbols/Makefile
                 symbols/documentation/Makefile
                 tmpwrk23234/gettext-0.19/intl/Makefile
                 tmpwrk71399/archive/intl/Makefile
                 utils/Makefile
                 utils/docs/Makefile
                 utils/examples/Makefile
                 utils/examples/gmk_sym/Makefile
                 utils/examples/gsch2pcb/Makefile
                 utils/examples/tragesym/Makefile
                 utils/gschlas/Makefile
                 utils/gxyrs/Makefile
                 utils/include/Makefile
                 utils/lib/Makefile
                 utils/scripts/Makefile
                 utils/src/Makefile
                 utils/tests/gxyrs/Makefile
                 utils/tests/refdes_renum/Makefile])
# --- Define and Export Paths for Subdirectories ---

# Define the location for shared gEDA data relative to datadir
# Default datadir is ${prefix}/share
GEDADATADIR="${datadir}/geda"
AC_SUBST(GEDADATADIR)
AC_EXPORT(GEDADATADIR) # Restore this line

# Define the location for gEDA rc files relative to sysconfdir
# Default sysconfdir is ${prefix}/etc
GEDARCDIR="${sysconfdir}/geda"
AC_SUBST(GEDARCDIR)
AC_EXPORT(GEDARCDIR) # Restore this line

# --- Configuration Files and Output (MUST BE AT THE END) ---
AC_CONFIG_SUBDIRS([xorn])
AC_OUTPUT
