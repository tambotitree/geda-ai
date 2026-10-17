#ifndef GSCHEM_FILE_HANDLERS_H
#define GSCHEM_FILE_HANDLERS_H

/*! \file gschem_file_handlers.h
 * \brief File handler type definitions, prototypes, and factory.
 */

typedef enum {
    FILE_TYPE_UNKNOWN,
    FILE_TYPE_SCHEMATIC,
    FILE_TYPE_SYMBOL,
    FILE_TYPE_MEMORY_STREAM,
    FILE_TYPE_WEB_STREAM
} GschemFileType;

typedef enum {
    FILE_STATE_CLOSED,
    FILE_STATE_OPENING,
    FILE_STATE_CLEAN,     /*!< No unsaved changes */
    FILE_STATE_DIRTY,     /*!< Unsaved changes present */
    FILE_STATE_SAVING,    /*!< Actively writing to disk */
    FILE_STATE_ERROR      /*!< Error state for recovery */
} GschemFileState;

typedef struct {
    char *filename;                  /*!< Full path of the file */
    GschemFileType type;             /*!< Schematic, symbol, etc. */
    GschemFileState current_state;   /*!< Current lifecycle state */
    GschemFileState previous_state;  /*!< Prior state (for debugging) */
    int is_journaled;                /*!< 1 = journal active, 0 = none */
    int is_async;                    /*!< 1 = async ops enabled */
    int dirty;                       /*!< 1 = unsaved changes */
} GschemFileHandler;

/*! \brief Factory to allocate and initialize a new handler */
GschemFileHandler *gschem_file_handler_create(const char *filename);
void gschem_file_handler_destroy(GschemFileHandler *handler);

/* State machine helpers */
void gschem_file_change_state(GschemFileHandler *handler, GschemFileState new_state);

/* File operations */
void gschem_file_open(GschemFileHandler *handler, const char *filename);
void gschem_file_save(GschemFileHandler *handler);
void gschem_file_close(GschemFileHandler *handler);
void gschem_file_write_journal(GschemFileHandler *handler);

#endif
/*--------- Legacy Declarations ---------*/
/*
../include/gettext.h:43:   On pre-ANSI systems without 'const', the config.h file is supposed to
../include/globals.h:29:/* Manager for recently used files */
../include/globals.h:36:extern char *rc_filename;
../include/globals.h:37:extern char *output_filename;
../include/gschem_accel_label.h:14: * file for a list of people on the GTK+ Team.  See the ChangeLog
../include/gschem_accel_label.h:15: * files for a list of changes.  These files are distributed with
../include/gschem_binding_integer.h:21: * \file gschem_binding_integer.h
../include/gschem_binding.h:21: * \file gschem_binding.h
../include/gschem_bottom_widget.h:21: * \file gschem_bottom_widget.h
../include/gschem_compselect_dockable.h:65:  gchar *selected_filename;
../include/gschem_dialog_misc.h:21: * \file gschem_dialog_misc.h
../include/gschem_file_handlers.h:4:/*! \file gschem_file_handlers.h
../include/gschem_file_handlers.h:20: * State machine for file lifecycle.
../include/gschem_file_handlers.h:35:    char *filename;                  /*!< Full path of the file */
../include/gschem_file_handlers.h:45:void gschem_file_change_state(GschemFileHandler *handler, GschemFileState new_state);
../include/gschem_file_handlers.h:48:void gschem_file_open(GschemFileHandler *handler, const char *filename);
../include/gschem_file_handlers.h:49:void gschem_file_save(GschemFileHandler *handler);
../include/gschem_file_handlers.h:50:void gschem_file_close(GschemFileHandler *handler);
../include/gschem_file_handlers.h:51:void gschem_file_write_journal(GschemFileHandler *handler);
../include/gschem_fill_swatch_cell_renderer.h:21: * \file gschem_fill_swatch_cell_renderer.h
../include/gschem_find_text_dockable.h:21: * \file gschem_find_text_dockable.h
../include/gschem_find_text_widget.h:21: * \file gschem_find_text_widget.h
../include/gschem_integer_combo_box.h:21: * \file gschem_integer_combo_box.h
../include/gschem_macro_widget.h:21: * \file gschem_macro_widget.h
../include/gschem_main_window.h:21: * \file gschem_main_window.h
../include/gschem_object_properties_dockable.h:21: * \file gschem_object_properties_dockable.h
../include/gschem_options_dockable.h:21: * \file gschem_options_dockable.h
../include/gschem_options.h:21: * \file gschem_options.h
../include/gschem_page_geometry.h:21: * \file gschem_page_geometry.h
../include/gschem_page_view.h:21: * \file gschem_page_view.h
../include/gschem_pango.h:21: * This file based on GDK's gdkpango.h (LGPL V2+)
../include/gschem_patch.h:20:/*! \file gschem_patch.h
../include/gschem_pin_type_combo.h:21: * \file gschem_pin_type_combo.h
../include/gschem_preview.h:21: *  \file gschem_preview.h
../include/gschem_preview.h:46:  gchar *filename;
../include/gschem_selection_adapter.h:21: * \file gschem_selection_adapter.h
../include/gschem_show_hide_text_widget.h:21: * \file gschem_show_hide_text_widget.h
../include/gschem_swatch_column_renderer.h:21: * \file gschem_swatch_column_renderer.h
../include/gschem_text_properties_dockable.h:21: * \file gschem_text_properties_dockable.h
../include/gschem_toplevel.h:50:  GschemChangeNotification *file_change_notification;
../include/gschem_toplevel.h:125:  char *pixbuf_filename;
../include/gschem_toplevel.h:198:  int file_preview;       /* controls if the preview area is enabled or not */
../include/i_vars.h:31:extern int default_file_preview;
../include/prototype.h:15:SCM g_funcs_pdf(SCM filename);
../include/prototype.h:16:SCM g_funcs_image(SCM filename);
../include/prototype.h:21:SCM g_funcs_filesel(SCM msg, SCM templ, SCM flags);
../include/prototype.h:57:SCM g_rc_file_preview(SCM mode);
../include/prototype.h:119:void i_update_filename(GschemToplevel *w_current);
../include/prototype.h:266:gboolean o_picture_exchange(GschemToplevel *w_current, const gchar *filename, GError **error);
../include/prototype.h:267:void picture_change_filename_dialog (GschemToplevel *w_current);
../include/prototype.h:268:void o_picture_set_pixbuf(GschemToplevel *w_current, GdkPixbuf *pixbuf, char *filename);
../include/prototype.h:326:char *o_undo_find_prev_filename(UNDO *start);
../include/prototype.h:383:char * generic_filesel_dialog(const char *, const char *, gint);
../include/prototype.h:390:gboolean x_dialog_confirm_create (GtkWindow *parent, const gchar *message, const gchar *filename);
../include/prototype.h:411:/* x_fileselect.c */
../include/prototype.h:412:PAGE *x_fileselect_create (GschemToplevel *w_current, const gchar *dirname, const gchar *basename);
../include/prototype.h:413:void x_fileselect_open(GschemToplevel *w_current);
../include/prototype.h:414:gboolean x_fileselect_save(GschemToplevel *w_current);
../include/prototype.h:415:int x_fileselect_load_backup(void *user_data, GString *message);
../include/prototype.h:430:PAGE *x_highlevel_new_page (GschemToplevel *w_current, const gchar *filename);
../include/prototype.h:431:PAGE *x_highlevel_open_page (GschemToplevel *w_current, const gchar *filename);
../include/prototype.h:432:gboolean x_highlevel_open_pages (GschemToplevel *w_current, GSList *filenames, gboolean already_confirmed);
../include/prototype.h:438:void x_image_lowlevel(GschemToplevel *w_current, const char* filename,
../include/prototype.h:439:		      int desired_width, int desired_height, char *filetype);
../include/prototype.h:456:PAGE *x_lowlevel_new_page (GschemToplevel *w_current, const gchar *filename);
../include/prototype.h:457:PAGE *x_lowlevel_open_page (GschemToplevel *w_current, const gchar *filename);
../include/prototype.h:458:gboolean x_lowlevel_save_page (GschemToplevel *w_current, PAGE *page, const gchar *filename);
../include/prototype.h:478:gchar *x_patch_guess_filename (PAGE *page);
../include/prototype.h:483:gboolean x_print_export_pdf_page (GschemToplevel *w_current, const gchar *filename);
../include/prototype.h:484:gboolean x_print_export_pdf (GschemToplevel *w_current, const gchar *filename);
../include/prototype.h:487:void x_rc_parse_gschem (TOPLEVEL *toplevel, const gchar *rcfile);
../include/prototype.h:503:void x_window_update_file_change_notification (GschemToplevel *w_current, PAGE *page);
../include/stroke.h:6:   a copy of this software and associated documentation files (the
../include/stroke.h:28:   See the file "LICENSE" for a copy of the GNU GPL terms.
../include/x_dialog.h:26: * Flags for generic_filesel_dialog()
*/
/*---------------------------------------*/
