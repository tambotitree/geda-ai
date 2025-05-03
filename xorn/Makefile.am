# Makefile.am for xorn

# Tell aclocal where to find our M4 macros
ACLOCAL_AMFLAGS = -I m4

# We are building one program, 'xorn', and installing it in bindir
bin_PROGRAMS = xorn

# Source files needed to compile the xorn executable
# Make sure this list matches the actual files in xorn/src/
xorn_SOURCES = \
	src/main.c \
	src/xorn.c src/xorn.h \
	src/action.c src/action.h \
	src/config.c src/config.h \
	src/log.c src/log.h \
	src/module.c src/module.h \
	src/plugin.c src/plugin.h \
	src/rcfile.c src/rcfile.h \
	src/utils.c src/utils.h \
	version.h

# Compiler Preprocessor Flags (include paths, defines)
# These rely on configure checks (PKG_CHECK_MODULES, AM_GNU_GETTEXT, etc.)
# being present in xorn/configure.ac
xorn_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I$(srcdir)/include \
	-I$(srcdir)/src \
	$(GLIB_CFLAGS) \
	$(GMODULE_CFLAGS) \
	$(GIO_CFLAGS) \
	$(INTLLIBS_CFLAGS)

# Linker Flags
xorn_LDFLAGS = \
	$(AM_LDFLAGS) \
	$(GLIB_LDFLAGS) \
	$(GMODULE_LDFLAGS) \
	$(GIO_LDFLAGS)

# Libraries to link against
xorn_LDADD = \
	$(AM_LDADD) \
	$(GLIB_LIBS) \
	$(GMODULE_LIBS) \
	$(GIO_LIBS) \
	$(INTLLIBS)

# Rule to generate version.h from configure variables
# Depends on configure.ac having AC_INIT([xorn], [VERSION], ...)
stamp-h: $(top_srcdir)/configure.ac
	@echo Creating $@
	@echo "#define VERSION \"$(VERSION)\"" > version.h.tmp
	@mv version.h.tmp version.h
	@echo timestamp > $@

# Ensure version.h is built before any source file that includes it
BUILT_SOURCES = stamp-h

# Files to remove when 'make clean' is run
CLEANFILES = stamp-h version.h version.h.tmp

# Additional files to include in the distribution tarball
EXTRA_DIST = \
	README.md \
	COPYING \
	AUTHORS \
	ChangeLog \
	NEWS \
	m4/introspection.m4 # Add other m4 files if needed

# If you have subdirectories like 'po' (for translations) or 'tests'
# that need their own Makefiles and build steps, uncomment SUBDIRS.
# Make sure those subdirectories have their own Makefile.am.
# SUBDIRS = po tests

# Optional: If you have Doxygen documentation specific to xorn
# doxygen:
#	cd $(srcdir)/doc && doxygen Doxyfile
# .PHONY: doxygen

