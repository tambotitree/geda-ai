#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([geda-ai], [1.0.0], [https://github.com/tambotitree/geda-ai/issues], [geda-ai])
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_AUX_DIR([build-tools]) # Tell automake where helper scripts are
AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])
AC_SUBST([PYTHON_VERSION], [3.9])
AC_CONFIG_FILES([environment.yml])
GEDA_PYTHON_SETUP


AC_CONFIG_MACRO_DIRS([m4 xorn/m4]) # Ensure m4 directory is listed
AC_CONFIG_HEADERS([config.h version.h]) # Ensure version.h is listed for generation

# Get detailed git version information using AX_GIT_VERSION.
# This macro typically sets its own variables (e.g., AX_GIT_COMMIT) and might
# also influence how PACKAGE_VERSION or similar are defined if not already set by AC_INIT.
AX_GIT_VERSION([$PACKAGE_VERSION])

# For consistency in user-facing messages and to simplify gschemrc version checks,
# we will ensure PACKAGE_DATE_VERSION (used in C code and messages)
# is the same as the simple PACKAGE_VERSION (e.g., "1.0.0").
temp_package_date_version="$PACKAGE_VERSION"
AC_DEFINE_UNQUOTED([PACKAGE_DATE_VERSION], ["$temp_package_date_version"], [The package version string, simplified to dotted version for geda-ai.])
AC_SUBST([PACKAGE_DATE_VERSION], [$temp_package_date_version])
 

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

# Check for a program to open URIs (web links, help files)
# GEDA_PROG_URI_VIEWER # Switching to the AX_OPTION_ variant used by geda-gaf
AX_OPTION_ASSERT
AX_OPTION_URI_VIEWER
AX_OPTION_GATTRIB

# Initialize libtool
LT_INIT

# --- Windows (MinGW) specific settings ---
case "${host_os}" in
  mingw*)
    WINDOWS_LIBTOOL_FLAGS="-no-undefined"
    MINGW_LDFLAGS="-Wl,--enable-auto-image-base"
    AC_DEFINE(WIN32, 1, [Define if compiling for Win32])
    ;;
  *)
    WINDOWS_LIBTOOL_FLAGS=""
    MINGW_LDFLAGS=""
    ;;
esac
AC_SUBST(WINDOWS_LIBTOOL_FLAGS)
AC_SUBST(MINGW_LDFLAGS)


# Check for Guile and its development tools (needed for libgeda)
GEDA_GUILE_CHECKS
# GEDA_GUILE_CHECKS # This macro might not be defined in the current m4/geda-guile.m4
AX_CHECK_GUILE([1.8.0]) # Call the macro that is defined in m4/geda-guile.m4

# Set up libgeda with the correct ld version number
LIBGEDA_LT_CURRENT=1
LIBGEDA_LT_REVISION=0
LIBGEDA_LT_AGE=0
LIBGEDA_LT_VERSION="${LIBGEDA_LT_CURRENT}:${LIBGEDA_LT_REVISION}:${LIBGEDA_LT_AGE}"
AC_SUBST(LIBGEDA_LT_VERSION)

# Set up libgedacairo with the correct ld version number
LIBGEDACAIRO_LT_CURRENT=1
LIBGEDACAIRO_LT_REVISION=0
LIBGEDACAIRO_LT_AGE=0
LIBGEDACAIRO_LT_VERSION="${LIBGEDACAIRO_LT_CURRENT}:${LIBGEDACAIRO_LT_REVISION}:${LIBGEDACAIRO_LT_AGE}"
AC_SUBST(LIBGEDACAIRO_LT_VERSION)


# --- Checks for libraries ---
# FIXME: Replace 'main' with a function in '-lm':
AC_CHECK_LIB([m], [main])

# Check for GLib (needed by libgeda and other components)
PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.32,
                  [have_glib=yes], [have_glib=no])
if test "x$have_glib" = "xno"; then
  AC_MSG_ERROR([GLib >= 2.32 not found. Please install the GLib development package.])
fi

# Check for GDK-Pixbuf (needed by libgeda for image handling)
PKG_CHECK_MODULES(GDK_PIXBUF, gdk-pixbuf-2.0 >= 2.24.0,
                  [have_gdk_pixbuf=yes], [have_gdk_pixbuf=no])
if test "x$have_gdk_pixbuf" = "xno"; then
  AC_MSG_ERROR([GDK-Pixbuf >= 2.24.0 not found. Please install the GDK-Pixbuf development package.])
fi

# Check for Cairo (needed by libgedacairo)
PKG_CHECK_MODULES(CAIRO, cairo >= 1.2.0,
                  [have_cairo=yes], [have_cairo=no])
if test "x$have_cairo" = "xno"; then
  AC_MSG_ERROR([Cairo >= 1.2.0 not found. Please install the Cairo development package (e.g., libcairo2-dev).])
fi

# Check for Pango and PangoCairo (needed by libgedacairo for text rendering)
PKG_CHECK_MODULES(PANGO, [pango >= 1.10.0 pangocairo >= 1.10.0],
                  [have_pango=yes], [have_pango=no])
if test "x$have_pango" = "xno"; then
  AC_MSG_ERROR([Pango/PangoCairo >= 1.10.0 not found. Please install the Pango development package (e.g., libpango1.0-dev).])
fi

# Check for GTK+ 2.x (includes GDK, needed by libgedacairo and gschem, gattrib etc.)
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.10.0,
                  [have_gtk=yes], [have_gtk=no])
if test "x$have_gtk" = "xno"; then
  AC_MSG_ERROR([GTK+ 2.x (>= 2.10.0) not found. Please install the GTK+ 2 development package (e.g., libgtk2.0-dev).])
fi

# Hacked in we should modify the libgedacairo/Makefile.am 
GDK_CFLAGS="$GTK_CFLAGS"
GDK_LIBS="$GTK_LIBS"
AC_SUBST(GDK_CFLAGS)
AC_SUBST(GDK_LIBS)

# --- Checks for header files ---
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([argz.h ctype.h fcntl.h inttypes.h langinfo.h libintl.h malloc.h stdint.h stdio_ext.h strings.h sys/param.h sys/time.h unistd.h])

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
AX_DESKTOP_I18N # Use the macro from geda-desktop-i18n.m4
# Define the text domain for libgeda
AC_DEFINE_UNQUOTED([LIBGEDA_GETTEXT_DOMAIN], ["libgeda"], [The gettext domain for libgeda])

# Define PKGDATADIR for use in C code
AC_DEFINE_UNQUOTED([PKGDATADIR], ["${pkgdatadir}"], [Package data directory])

# Define GETOPT_OPTIONS for gnetlist-legacy
AC_SUBST(GNETLIST_GETOPT_OPTIONS) # For Makefiles, if needed
AC_DEFINE_UNQUOTED(GETOPT_OPTIONS, "$GNETLIST_GETOPT_OPTIONS", [Getopt options string for gnetlist-legacy])

# --- gEDA Specific Path and Library Setup (using AX_ macros from geda-gaf) ---
# Where should data files be installed/searched for?
AX_DATA_DIRS
# Where should PCB footprints be searched for?
AX_PCB_DIRS
# Set up libgeda with the correct ld version number
# Using 1:0:0 for geda-ai for now. geda-gaf used a different scheme (e.g., 47:0:0)
AX_LIBGEDA([1:0:0])
# Set up libgedacairo with the correct ld version number
# Using 1:0:0 for geda-ai for now. geda-gaf used a different scheme (e.g., 1:3:0)
AX_LIBGEDACAIRO([1:0:0])

#####################################################################
# Recurse into Xorn configuration
#####################################################################

export GEDADATADIR
GEDAIDATADIR="${GEDADATADIR}"
export GEDAIDATADIR
export GEDARCDIR

AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_MBRTOWC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([__argz_count __argz_next __argz_stringify floor getcwd localeconv mblen mbrlen memchr memmove mempcpy memset mkdir munmap nl_langinfo pow rint setlocale sqrt stpcpy strcasecmp strchr strcspn strdup strerror strrchr strstr strtol strtoul])

# --- Feature Conditionals (MUST BE BEFORE AC_OUTPUT) ---

# Check for groff (for HTML docs)
AC_PATH_PROG([GROFF], [groff], [no])
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


# Optional XDG integration (uses AX_* macros from m4/geda-desktop.m4)
# Explicitly define the conditional here as a fallback/diagnostic
AM_CONDITIONAL(HAVE_ICON_INSTALLER, test "x$ax_cv_xdg_icon_installer" != xno)
AX_OPTION_XDG_DATA # Define ICON_THEME_INSTALLER and XDGDATADIR
AX_OPTION_XDG_DB   # Should define ENABLE_UPDATE_XDG_DATABASE conditional
# AX_OPTION_KDE3_DATA # Keep commented unless KDE3 support is needed


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

AX_CREATE_CONDA_ENVIRONMENT_YML

# --- Configuration Files and Output (MUST BE AT THE END) ---
AC_CONFIG_FILES([Makefile
                 po/Makefile
                 gedai/Makefile
                 contrib/Makefile
                 contrib/gmk_sym/Makefile
                 contrib/olib/Makefile
                 contrib/sarlacc_schem/Makefile
                 contrib/scripts/Makefile
                 contrib/scripts/gnet_hier_verilog.1
                 contrib/scripts/pads_backannotate.1
                 contrib/scripts/sarlacc_sym.1
                 contrib/scripts/sw2asc
                 contrib/scripts/sw2asc.1
                 contrib/smash_megafile/Makefile
                 docs/Makefile
                 docs/toplevel/gedadocs.html
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
                 gattrib/lib/system-gattribrc
                 gattrib/data/Makefile
                 gattrib/design/Makefile
                 gattrib/docs/Makefile
                 gattrib/include/Makefile
                 gattrib/lib/Makefile
                 gattrib/src/Makefile
                 gnetlist-legacy/Makefile
                 gnetlist-legacy/docs/Makefile
                 gnetlist-legacy/lib/system-gnetlistrc
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
                 gsymcheck/include/Makefile
                 gsymcheck/docs/Makefile
                 gsymcheck/lib/system-gsymcheckrc
                 gsymcheck/lib/Makefile
                 gsymcheck/src/Makefile
                 gsymcheck/tests/Makefile
                 libgeda/Makefile
                 libgeda/libgeda.pc
                 libgeda/data/Makefile
                 libgeda/docs/Makefile
                 libgeda/docs/images/Makefile
                 libgeda/scheme/Makefile
                 libgeda/src/Makefile
                 libgedacairo/Makefile
                 libgedacairo/libgedacairo.pc
                 symbols/Makefile
                 symbols/documentation/Makefile
                 utils/Makefile
                 utils/docs/Makefile
                 utils/examples/Makefile
                 utils/examples/gmk_sym/Makefile
                 utils/examples/gsch2pcb/Makefile
                 utils/examples/tragesym/Makefile
                 utils/gschlas/Makefile
                 utils/lib/system-gschlasrc
                 utils/gxyrs/Makefile
                 utils/include/Makefile
                 utils/lib/Makefile
                 utils/scripts/Makefile
                 utils/src/Makefile
                 utils/tests/gxyrs/Makefile
                 utils/tests/refdes_renum/Makefile
                 admin/Makefile
                 admin/docs/Makefile
                 admin/tests/Makefile])
# --- Define and Export Paths for Subdirectories ---



# --- Configuration Files and Output (MUST BE AT THE END) ---
AC_CONFIG_SUBDIRS([xorn])
AC_OUTPUT
