lib_LTLIBRARIES = libgedacairo.la

check_PROGRAMS = test-libgedacairo
TESTS = $(check_PROGRAMS)
test_libgedacairo_SOURCES = $(top_srcdir)/tests/test_libgedacairo.c
test_libgedacairo_CPPFLAGS = $(AM_CPPFLAGS)
test_libgedacairo_LDADD = libgedacairo.la \
                        $(top_builddir)/libgeda/src/libgeda.la \
                        $(GUILE_LIBS) $(GLIB_LIBS) $(CAIRO_LIBS) $(PANGO_LIBS)

libgedacairo_la_SOURCES = \
	edacairo.c \
	edapangorenderer.c \
	edarenderer.c

libgedacairoincludedir = $(includedir)/libgedacairo

libgedacairoinclude_HEADERS = \
	libgedacairo.h \
	edacairo.h \
	edarenderer.h

noinst_HEADERS = \
	edapangorenderer.h

EXTRA_DIST = README ChangeLog

AM_CPPFLAGS = \
	-DLOCALEDIR=\"$(localedir)\" -DG_LOG_DOMAIN=\"libgedacairo\" \
	-I$(top_srcdir)/libgeda/include -I$(top_srcdir)
AM_CFLAGS = \
	$(GCC_CFLAGS) $(MINGW_CFLAGS) $(GUILE_CFLAGS) $(GLIB_CFLAGS) \
	$(GDK_CFLAGS) $(CAIRO_CFLAGS) $(PANGO_CFLAGS) $(GDK_PIXBUF_CFLAGS)
libgedacairo_la_LDFLAGS = -version-info $(LIBGEDACAIRO_LT_VERSION) \
	$(WINDOWS_LIBTOOL_FLAGS) $(MINGW_LDFLAGS) $(GUILE_LIBS) \
	$(GDK_LIBS) $(GLIB_LIBS) $(CAIRO_LIBS) $(PANGO_LIBS) $(GDK_PIXBUF_LIBS)
LIBTOOL=@LIBTOOL@ --silent

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgedacairo.pc

if HAVE_GIT_REPO
ChangeLog: $(top_builddir)/stamp-git
	( \
	  cd $(srcdir) && \
	  $(GIT) log --pretty=medium $(CHANGELOG_BASE).. -- . || \
	  echo "WARNING: ChangeLog information not available from git" >&2 ; \
	) > $@
endif HAVE_GIT_REPO
