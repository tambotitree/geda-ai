## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gschlas

# don't forget all *.h files */
gschlas_SOURCES = \
	g_rc.c \
	g_register.c \
	globals.c \
	gschlas.c \
	i_vars.c \
	parsecmd.c \
	s_util.c

gschlas_CPPFLAGS = -I$(top_srcdir)/libgeda/include -I$(srcdir)/../include \
	-I$(top_srcdir) -I$(includedir)
gschlas_CFLAGS = $(GCC_CFLAGS) $(MINGW_CFLAGS) $(GLIB_CFLAGS) $(GIO_CFLAGS) \
	$(GUILE_CFLAGS) $(GDK_PIXBUF_CFLAGS) \
	-Wall -Wextra -Werror -Wpedantic -Wstrict-prototypes \
	-Wmissing-prototypes -Wformat=2
gschlas_LDFLAGS = $(GLIB_LIBS) $(GIO_LIBS) $(GUILE_LIBS) $(GDK_PIXBUF_LIBS)
gschlas_LDADD = $(top_builddir)/libgeda/src/libgeda.la

MOSTLYCLEANFILES = *.log *.ps core FILE *~
CLEANFILES = *.log *.ps core FILE *~
DISTCLEANFILES = *.log core FILE *~
MAINTAINERCLEANFILES = *.log *.ps core FILE *~ Makefile.in

EXTRA_DIST = $(top_srcdir)/tests/test_gschlas.py
check-local: gschlas
	GEDA_TEST_BUILD_ROOT='$(abs_top_builddir)' $(PYTHON) \
	  $(top_srcdir)/tests/test_gschlas.py
