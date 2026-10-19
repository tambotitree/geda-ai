## Process this file with automake to produce Makefile.in
## This Makefile.am created for gattrib by SDB 12.13.2003

bin_PROGRAMS = gattrib

## don't forget all *.h files */
gattrib_SOURCES = \
		gattrib_bom.c \
		gattrib_bom_hook.c \
		gattrib_json.c \
		gattrib_sheet.c \
		globals.c \
		listsort.c \
		parsecmd.c \
		f_export.c \
		g_register.c \
		g_rc.c \
		i_vars.c \
		i_basic.c \
		x_window.c \
		x_gtksheet.c \
		x_dialog.c \
		x_fileselect.c \
		s_rename.c \
		s_misc.c \
		s_string_list.c \
		s_sheet_data.c \
		s_table.c \
		s_object.c \
		s_attrib.c \
		s_toplevel.c \
		s_visibility.c \
		gattrib.c 


gattrib_CPPFLAGS = -I$(top_srcdir)/libgeda/include \
	-I$(srcdir)/../include -I$(top_srcdir) -I$(includedir)
gattrib_CFLAGS = $(GCC_CFLAGS) $(GLIB_CFLAGS) $(GIO_CFLAGS) \
	$(MINGW_CFLAGS) \
	$(GTK_CFLAGS) $(GTHREAD_CFLAGS) $(GUILE_CFLAGS) \
	-Wall -Wextra -Werror -Wpedantic -Wstrict-prototypes \
	-Wmissing-prototypes -Wformat=2
gattrib_LDFLAGS = $(GLIB_LIBS) $(GIO_LIBS) $(GTK_LIBS) $(GTHREAD_LIBS) \
	$(GUILE_LIBS) $(MINGW_GUI_LDFLAGS)
gattrib_LDADD = $(top_builddir)/libgeda/src/libgeda.la

localedir = @datadir@/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

## Unit tests.  Built and run by "make check"; neither needs a display
## because they only exercise the model and the supplier plumbing.
check_PROGRAMS = test-gattrib-sheet test-gattrib-bom test-gattrib-bom-hook

test_gattrib_sheet_SOURCES = test_gattrib_sheet.c gattrib_sheet.c
test_gattrib_sheet_CPPFLAGS = -I$(srcdir)/../include -I$(srcdir)
test_gattrib_sheet_CFLAGS = $(GCC_CFLAGS) $(GLIB_CFLAGS) $(GTK_CFLAGS)
test_gattrib_sheet_LDADD = $(GLIB_LIBS) $(GTK_LIBS)

## The BOM tests drive the Octopart client through an injected transport, so
## they need no credentials and no network.  gio (for GIOError) arrives
## through GTK, as it does for the gattrib binary itself.
test_gattrib_bom_SOURCES = test_gattrib_bom.c gattrib_bom.c gattrib_json.c
test_gattrib_bom_CPPFLAGS = -I$(srcdir)/../include -I$(srcdir)
test_gattrib_bom_CFLAGS = $(GCC_CFLAGS) $(GLIB_CFLAGS) $(GTK_CFLAGS)
test_gattrib_bom_LDADD = $(GLIB_LIBS) $(GTK_LIBS)

TESTS = test-gattrib-sheet test-gattrib-bom test-gattrib-bom-hook

## The hook tests drive the real supplier through the offline backend and the
## real sheet widget, so they pin down the column choices and overwrite rules
## without a display or the rest of gattrib.  globals.c is linked for the
## sheet_head that the default callbacks touch.
test_gattrib_bom_hook_SOURCES = test_gattrib_bom_hook.c gattrib_bom_hook.c \
	gattrib_bom.c gattrib_json.c gattrib_sheet.c globals.c
test_gattrib_bom_hook_CPPFLAGS = -I$(top_srcdir)/libgeda/include \
	-I$(srcdir)/../include -I$(srcdir) -I$(top_srcdir)
test_gattrib_bom_hook_CFLAGS = $(GCC_CFLAGS) $(GLIB_CFLAGS) $(GTK_CFLAGS) \
	$(GUILE_CFLAGS)
test_gattrib_bom_hook_LDADD = $(top_builddir)/libgeda/src/libgeda.la \
	$(GLIB_LIBS) $(GTK_LIBS) $(GUILE_LIBS)

MOSTLYCLEANFILES = *.log *.ps core FILE *~ #*#  test-gattrib-sheet test-gattrib-bom test-gattrib-bom-hook *.trs
CLEANFILES = *.log *.ps core FILE *~ #*# test-gattrib-sheet test-gattrib-bom test-gattrib-bom-hook *.trs
DISTCLEANFILES = *.log core FILE *~ #*#
MAINTAINERCLEANFILES = *.log *.ps core FILE *~ Makefile.in #*#
