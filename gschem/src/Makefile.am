bin_PROGRAMS = gschem

BUILT_SOURCES = \
	gschem_icon_resources.c \
	gschem_icon_resources.h \
	g_attrib.x \
	g_hook.x \
	g_keys.x \
	g_util.x \
	g_window.x \
	g_select.x

gschem_SOURCES = \
	a_zoom.c \
	g_action.c \
	actions.c \
	g_attrib.c \
	g_funcs.c \
	g_hook.c \
	g_keys.c \
	g_rc.c \
	g_register.c \
	g_select.c \
	g_util.c \
	g_window.c \
	globals.c \
	gschem.c \
	gschem_signal.c \
	gschem_icons.c \
	gschem_appearance.c \
	gschemhotkeystore.c \
	gschem_about_dialog.c \
	gschem_accel_label.c \
	gschem_action.c \
	gschem_ai_assist_dialog.c \
	gschem_alignment_combo.c \
	gschem_arc_dialog.c \
	gschem_binding.c \
	gschem_binding_integer.c \
	gschem_bottom_widget.c \
	gschem_change_notification.c \
	gschem_close_confirmation_dialog.c \
	gschem_compselect_dockable.c \
	gschem_dialog.c \
	gschem_dialog_misc.c \
	gschem_dockable.c \
	gschem_fill_swatch_cell_renderer.c \
	gschem_find_text_dockable.c \
	gschem_find_text_widget.c \
	gschem_hotkey_dialog.c \
	gschem_integer_combo_box.c \
	gschem_log_dockable.c \
	gschem_macro_widget.c \
	gschem_main_window.c \
	gschem_messages_dockable.c \
	gschem_multiattrib_dockable.c \
	gschem_object_properties_dockable.c \
	gschem_options.c \
	gschem_options_dockable.c \
	gschem_page_geometry.c \
	gschem_page_view.c \
	gschem_pagesel_dockable.c \
	gschem_patch.c \
	gschem_patch_dockable.c \
	gschem_pin_type_combo.c \
	gschem_preview.c \
	gschem_rotation_combo.c \
	gschem_selection_adapter.c \
	gschem_show_hide_text_widget.c \
	gschem_slot_edit_dialog.c \
	gschem_swatch_column_renderer.c \
	gschem_text_properties_dockable.c \
	gschem_toplevel.c \
	i_basic.c \
	i_vars.c \
	m_basic.c \
	o_arc.c \
	o_attrib.c \
	o_basic.c \
	o_box.c \
	o_buffer.c \
	o_bus.c \
	o_circle.c \
	o_complex.c \
	o_copy.c \
	o_delete.c \
	o_find.c \
	o_grips.c \
	o_line.c \
	o_misc.c \
	o_move.c \
	o_net.c \
	o_ognrst.c \
	o_path.c \
	o_picture.c \
	o_pin.c \
	o_place.c \
	o_select.c \
	o_slot.c \
	o_text.c \
	o_undo.c \
	parsecmd.c \
	s_stretch.c \
	stroke.c \
	x_attribedit.c \
	x_autonumber.c \
	x_basic.c \
	x_clipboard.c \
	x_color.c \
	x_colorcb.c \
	x_controlfd.c \
	x_dialog.c \
	x_event.c \
	x_fam.c \
	x_fileselect.c \
	x_fstylecb.c \
	x_grid.c \
	x_grid_size_sb.c \
	x_hierarchy.c \
	x_highlevel.c \
	x_image.c \
	x_integerls.c \
	x_linecapcb.c \
	x_linetypecb.c \
	x_lowlevel.c \
	x_menus.c \
	x_misc.c \
	x_newtext.c \
	x_print.c \
	x_rc.c \
	x_script.c \
	x_stroke.c \
	x_window.c \
	unit_tests.c

gschem_CPPFLAGS = -I$(top_srcdir)/libgeda/include  -I$(srcdir)/../include \
	-I$(top_srcdir) -I$(includedir)
gschem_CFLAGS = $(GCC_CFLAGS) $(LIBFAM_CFLAGS) \
	$(MINGW_CFLAGS) \
	$(GLIB_CFLAGS) $(GIO_CFLAGS) $(GTK_CFLAGS) $(GTHREAD_CFLAGS) \
	$(GUILE_CFLAGS)
gschem_LDFLAGS = $(LIBFAM_LDFLAGS) $(GLIB_LIBS) $(GIO_LIBS) $(GTK_LIBS) \
	$(GTHREAD_LIBS) $(GUILE_LIBS) $(MINGW_GUI_LDFLAGS)
gschem_LDADD = \
	$(top_builddir)/libgedacairo/libgedacairo.la \
	$(top_builddir)/libgeda/src/libgeda.la

# Resource inputs are distributed in ../icon-theme. Track PNG changes too.
icon_theme_dir = $(top_srcdir)/gschem/icon-theme
gschem_icon_xml = $(icon_theme_dir)/gschem-icons.gresource.xml
gschem_icon_files = \
	$(icon_theme_dir)/16x16/actions/attribute-attach.png \
	$(icon_theme_dir)/16x16/actions/attribute-detach.png \
	$(icon_theme_dir)/16x16/actions/attribute-show-both.png \
	$(icon_theme_dir)/16x16/actions/attribute-show-name.png \
	$(icon_theme_dir)/16x16/actions/attribute-show-value.png \
	$(icon_theme_dir)/16x16/actions/clone.png \
	$(icon_theme_dir)/16x16/actions/deselect.png \
	$(icon_theme_dir)/16x16/actions/gtk-about.png \
	$(icon_theme_dir)/16x16/actions/gtk-cancel.png \
	$(icon_theme_dir)/16x16/actions/gtk-close.png \
	$(icon_theme_dir)/16x16/actions/gtk-copy.png \
	$(icon_theme_dir)/16x16/actions/gtk-cut.png \
	$(icon_theme_dir)/16x16/actions/gtk-delete.png \
	$(icon_theme_dir)/16x16/actions/gtk-edit.png \
	$(icon_theme_dir)/16x16/actions/gtk-enable-accels.png \
	$(icon_theme_dir)/16x16/actions/gtk-execute.png \
	$(icon_theme_dir)/16x16/actions/gtk-find.png \
	$(icon_theme_dir)/16x16/actions/gtk-go-back.png \
	$(icon_theme_dir)/16x16/actions/gtk-go-down.png \
	$(icon_theme_dir)/16x16/actions/gtk-go-forward.png \
	$(icon_theme_dir)/16x16/actions/gtk-go-up.png \
	$(icon_theme_dir)/16x16/actions/gtk-goto-bottom.png \
	$(icon_theme_dir)/16x16/actions/gtk-new.png \
	$(icon_theme_dir)/16x16/actions/gtk-ok.png \
	$(icon_theme_dir)/16x16/actions/gtk-open.png \
	$(icon_theme_dir)/16x16/actions/gtk-paste.png \
	$(icon_theme_dir)/16x16/actions/gtk-preferences.png \
	$(icon_theme_dir)/16x16/actions/gtk-print.png \
	$(icon_theme_dir)/16x16/actions/gtk-properties.png \
	$(icon_theme_dir)/16x16/actions/gtk-quit.png \
	$(icon_theme_dir)/16x16/actions/gtk-redo.png \
	$(icon_theme_dir)/16x16/actions/gtk-refresh.png \
	$(icon_theme_dir)/16x16/actions/gtk-revert-to-saved.png \
	$(icon_theme_dir)/16x16/actions/gtk-save-as.png \
	$(icon_theme_dir)/16x16/actions/gtk-save.png \
	$(icon_theme_dir)/16x16/actions/gtk-select-all.png \
	$(icon_theme_dir)/16x16/actions/gtk-undo.png \
	$(icon_theme_dir)/16x16/actions/gtk-zoom-fit.png \
	$(icon_theme_dir)/16x16/actions/gtk-zoom-in.png \
	$(icon_theme_dir)/16x16/actions/gtk-zoom-out.png \
	$(icon_theme_dir)/16x16/actions/insert-arc.png \
	$(icon_theme_dir)/16x16/actions/insert-attribute.png \
	$(icon_theme_dir)/16x16/actions/insert-box.png \
	$(icon_theme_dir)/16x16/actions/insert-bus.png \
	$(icon_theme_dir)/16x16/actions/insert-circle.png \
	$(icon_theme_dir)/16x16/actions/insert-line.png \
	$(icon_theme_dir)/16x16/actions/insert-net.png \
	$(icon_theme_dir)/16x16/actions/insert-path.png \
	$(icon_theme_dir)/16x16/actions/insert-pin.png \
	$(icon_theme_dir)/16x16/actions/insert-symbol.png \
	$(icon_theme_dir)/16x16/actions/insert-text.png \
	$(icon_theme_dir)/16x16/actions/multi-clone.png \
	$(icon_theme_dir)/16x16/actions/select.png \
	$(icon_theme_dir)/16x16/actions/symbol-datasheet.png \
	$(icon_theme_dir)/24x24/actions/object-flip-horizontal.xpm \
	$(icon_theme_dir)/24x24/actions/object-rotate-left.xpm \
	$(icon_theme_dir)/scalable/actions/insert-image.svg \
	$(icon_theme_dir)/scalable/actions/window-new.svg \
	$(icon_theme_dir)/48x48/apps/geda-gschem.png \
	$(icon_theme_dir)/scalable/actions/help-browser.svg \
	$(icon_theme_dir)/scalable/actions/web-browser.svg
nodist_gschem_SOURCES = gschem_icon_resources.c gschem_icon_resources.h

# --compiler=msvc selects portable numeric byte arrays, avoiding oversized
# string literals under -Wpedantic. It does not select or invoke our C compiler.
gschem_icon_resources.c: $(gschem_icon_xml) $(gschem_icon_files)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@.tmp \
	  --sourcedir=$(icon_theme_dir) --generate-source --manual-register \
	  --compiler=msvc --c-name=gschem_icon_data $(gschem_icon_xml) && mv $@.tmp $@

gschem_icon_resources.h: $(gschem_icon_xml)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@.tmp \
	  --sourcedir=$(icon_theme_dir) --generate-header --manual-register \
	  --c-name=gschem_icon_data \
	  $(gschem_icon_xml) && mv $@.tmp $@

# Keep strict diagnostics on source builds, outside configure's feature probes.
GSCHEM_WARNING_CFLAGS = -Wall -Wextra -Werror -Wpedantic \
  -Wstrict-prototypes -Wmissing-prototypes -Wformat=2
gschem_CFLAGS += $(GSCHEM_WARNING_CFLAGS)

# This is used to generate boilerplate for defining Scheme functions
# in C.
SUFFIXES = .x
snarf_cpp_opts = $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(gschem_CPPFLAGS) $(AM_CFLAGS) $(gschem_CFLAGS) $(CPPFLAGS)
AM_V_SNARF = $(am__v_SNARF_@AM_V@)
am__v_SNARF_ = $(am__v_SNARF_@AM_DEFAULT_V@)
am__v_SNARF_0 = @echo "  SNARF   " $@;
am__v_SNARF_1 =
.c.x:
	$(AM_V_SNARF)CPP="$(CPP)" $(GUILE_SNARF) -o $@ $< $(snarf_cpp_opts)



EXTRA_DIST = actions.c

localedir = @datadir@/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

# Add a custom pedantic target ... make pedantic is for the obssesive complusively code cleaners
#	1.	make PEDANTIC=1
#	•	Runs clang-tidy and valgrind.
#	•	If both succeed, creates pedantic.tidy.
#	2.	make clean-pedantic
#	•	Removes the sentinel, forcing a re-run next time.
#	3.	If you change any .c file, make sees $(srcdir)/*.c newer than pedantic.tidy and re-runs.
.PHONY: pedantic clean-pedantic

PEDSENTINEL = pedantic.tidy

pedantic: $(PEDSENTINEL)

$(PEDSENTINEL): $(gschem_SOURCES)
	@echo "🔍 Running clang-tidy on all source files..."
	@for file in $(gschem_SOURCES); do \
	  clang-tidy $(srcdir)/$$file -- $(gschem_CPPFLAGS) $(gschem_CFLAGS) || exit 1; \
	  @command -v clang-tidy >/dev/null 2>&1 || { echo "clang-tidy not found!"; exit 1; }
	  @echo "🔍 Running clang-tidy on all source files..."
	done
	@echo "🧪 Running valgrind on gschem binary..."
	@valgrind --leak-check=full --track-origins=yes $(builddir)/gschem || exit 1
	@touch $(PEDSENTINEL)

clean-pedantic:
	@rm -f $(PEDSENTINEL)

clean-local: clean-pedantic

MOSTLYCLEANFILES = *.log core FILE *~
CLEANFILES = *.log core FILE *~ $(BUILT_SOURCES)
DISTCLEANFILES = *.log core FILE *~
MAINTAINERCLEANFILES = *.log core FILE *~ Makefile.in

# Exercise signal bookkeeping without creating a GTK window.
check_PROGRAMS = test-gschem-signal
TESTS = $(check_PROGRAMS)
test_gschem_signal_SOURCES = $(top_srcdir)/tests/test_gschem_signal.c gschem_signal.c
test_gschem_signal_CPPFLAGS = -I$(srcdir)/../include $(GLIB_CFLAGS)
test_gschem_signal_CFLAGS = $(GCC_CFLAGS) $(GSCHEM_WARNING_CFLAGS)
test_gschem_signal_LDADD = $(GLIB_LIBS) -lgobject-2.0

check_PROGRAMS += test-gschem-icons
test_gschem_icons_SOURCES = $(top_srcdir)/tests/test_gschem_icons.c gschem_icons.c gschem_appearance.c
nodist_test_gschem_icons_SOURCES = gschem_icon_resources.c gschem_icon_resources.h
test_gschem_icons_CPPFLAGS = -I$(srcdir)/../include $(GTK_CFLAGS)
test_gschem_icons_CFLAGS = $(GCC_CFLAGS) $(GSCHEM_WARNING_CFLAGS)
test_gschem_icons_LDADD = $(GTK_LIBS)

# Render the production viewport path without a display server.
EXTRA_DIST += $(top_srcdir)/tests/test_viewport_rendering.py \
  $(top_srcdir)/tests/test_picker_attributes.py \
  $(top_srcdir)/tests/test_net_interaction.py
check-local: gschem-gschem_page_geometry.o
	GEDA_TEST_BUILD_ROOT='$(abs_top_builddir)' $(PYTHON) \
	  $(top_srcdir)/tests/test_viewport_rendering.py
	GEDA_TEST_BUILD_ROOT='$(abs_top_builddir)' $(PYTHON) \
	  $(top_srcdir)/tests/test_picker_attributes.py
	GEDA_TEST_BUILD_ROOT='$(abs_top_builddir)' $(PYTHON) \
	  $(top_srcdir)/tests/test_net_interaction.py
