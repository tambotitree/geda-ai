## Process this file with automake to produce Makefile.in
## This Makefile.am created for gattrib by SDB 12.13.2003

bin_PROGRAMS = gattrib

## don't forget all *.h files */
gattrib_SOURCES = \
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
	-Wno-unused-function $(Wno_misleading_indentation_IF_SUPPORTED)
gattrib_LDFLAGS = $(GLIB_LIBS) $(GIO_LIBS) $(GTK_LIBS) $(GTHREAD_LIBS) \
	$(GUILE_LIBS) $(MINGW_GUI_LDFLAGS)
gattrib_LDADD = $(top_builddir)/libgeda/src/libgeda.la

localedir = @datadir@/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

## Unit test for the attribute-matrix widget.  Built and run by "make check";
## it needs no display because it only exercises the model API.
check_PROGRAMS = test-gattrib-sheet

test_gattrib_sheet_SOURCES = test_gattrib_sheet.c gattrib_sheet.c
test_gattrib_sheet_CPPFLAGS = -I$(srcdir)/../include -I$(srcdir)
test_gattrib_sheet_CFLAGS = $(GCC_CFLAGS) $(GLIB_CFLAGS) $(GTK_CFLAGS)
test_gattrib_sheet_LDADD = $(GLIB_LIBS) $(GTK_LIBS)

TESTS = test-gattrib-sheet

MOSTLYCLEANFILES = *.log *.ps core FILE *~ #*#  test-gattrib-sheet *.trs
CLEANFILES = *.log *.ps core FILE *~ #*# test-gattrib-sheet *.trs
DISTCLEANFILES = *.log core FILE *~ #*#
MAINTAINERCLEANFILES = *.log *.ps core FILE *~ Makefile.in #*#
