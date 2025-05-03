SUBDIRS = po gedai
if ENABLE_GATTRIB
GATTRIB_DIR=gattrib
endif
SUBDIRS += libgeda libgedacairo gaf gschem ${GATTRIB_DIR} \
	gsymcheck gnetlist-legacy utils symbols docs examples contrib xorn

ACLOCAL_AMFLAGS = -I m4

# We need to ensure that `make distcheck' *doesn't* find a git
# repository!
#
# We also should be able to successfully build & install without
# groff.
false=/bin/sh -c '! :'
DISTCHECK_CONFIGURE_FLAGS = \
	GIT="$(false)" \
	GROFF="$(false)" \
	--disable-update-xdg-database

EXTRA_DIST = autogen.sh version.h version.h.in HACKING \
	COPYING.LGPL


doxygen:
if ENABLE_DOXYGEN
	@for subdir in gattrib/docs gschem/docs gnetlist-legacy/docs \
	  libgeda/docs gsymcheck/docs utils/docs ; do \
	  (cd $$subdir; $(MAKE) doxygen) || exit 1; \
        done
else
	@echo 'configure was not run with the --enable-doxygen switch.'
	@echo 'The documentation cannot be built from the sources.'
endif

DISTCLEANFILES = 
MAINTAINERCLEANFILES=Makefile.in aclocal.m4 config.h.in configure
