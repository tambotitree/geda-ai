/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 2013 Ales Hvezda
 * Copyright (C) 2013-2020 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*!
 * \file x_newtext.c
 *
 * \brief A dialog box for adding new text to a schematic.
 */

#include <config.h>

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"
#include <gdk/gdkkeysyms.h>



#define TYPE_NEWTEXT           (newtext_get_type())
#define NEWTEXT(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_NEWTEXT, NewText))
#define NEWTEXT_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass),  TYPE_NEWTEXT, NewTextClass))
#define IS_NEWTEXT(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_NEWTEXT))

typedef struct _NewTextClass NewTextClass;
typedef struct _NewText NewText;

struct _NewTextClass {
  GschemDialogClass parent_class;
};

struct _NewText {
    GschemDialog parent;

    GtkWidget *aligncb;
    GtkWidget *colorcb;
    GtkWidget *rotatecb;
    GtkWidget *textsizecb;
    GtkWidget *text_view;
};



/*! \brief Handles the user response when apply is selected
 *
 *  \par Function Description
 *  This function applies the text from the text entry dialog.
 *
 *  \param [in] dialog The new text dialog
 */
static void
dialog_response_apply (NewText *dialog)
{
  int align = LOWER_LEFT;
  int color = TEXT_COLOR;
  int rotate = 0;
  int size = 12;
  gchar *string = NULL;
  gchar *tmp = NULL;
  GtkTextBuffer *textbuffer;
  GtkTextIter start, end;
  int value;

  textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(dialog->text_view));
  gtk_text_buffer_get_bounds (textbuffer, &start, &end);
  string =  gtk_text_iter_get_text (&start, &end);

  if (string[0] == '\0' )
    return;

  switch(dialog->parent.w_current->text_caps) {
    case(LOWER):
      tmp = g_utf8_strdown (string, -1);
      break;

    case(UPPER):
      tmp = g_utf8_strup (string, -1);
      break;

    case(BOTH):
    default:
      /* do nothing */
      break;
  }

  value = x_colorcb_get_index (dialog->colorcb);
  if (value >= 0) {
    color = value;
  }

  value = gschem_alignment_combo_get_align (dialog->aligncb);
  if (value >= 0) {
    align = value;
  }

  value = gschem_integer_combo_box_get_value (dialog->textsizecb);
  if (value > 0) {
    size = value;
  }

  value = gschem_rotation_combo_get_angle (dialog->rotatecb);
  if (value >= 0) {
    rotate = value;
  }

  /* select the text, so you can continue immediatly writing the next text */
  select_all_text_in_textview(GTK_TEXT_VIEW(dialog->text_view));
  gtk_widget_grab_focus(dialog->text_view);

  o_text_prepare_place (dialog->parent.w_current,
                        tmp == NULL ? string : tmp,
                        color,
                        align,
                        rotate,
                        size);

  //gschem_toplevel_page_content_changed (dialog->parent.w_current,
  //                                      dialog->parent.w_current->toplevel->page_current);

  g_free (string);
  g_free (tmp);
}



/*! \brief Handles the user response when cancel is selected
 *
 *
 *
 *  \param [in,out] dialog The new text dialog
 */
static void dialog_response_cancel(NewText *dialog)
{
  i_cancel(dialog->parent.w_current);
  gtk_widget_destroy(dialog->parent.w_current->tiwindow);
  dialog->parent.w_current->tiwindow=NULL;
}



/*! \brief Handles user responses from the new text dialog box

 *  \par Function Description
 *  Callback function for the text entry dialog.
 *
 *  \param [in,out] widget The new text dialog
 *  \param [na]     unused Unused parameter
 */
static void text_input_dialog_response(NewText *dialog, gint response, gpointer unused)
{
  switch(response) {
    case GTK_RESPONSE_APPLY:
      dialog_response_apply(dialog);
      break;
    case GTK_RESPONSE_CLOSE:
    case GTK_RESPONSE_DELETE_EVENT:
      dialog_response_cancel(dialog);
      break;
    default:
      printf("text_edit_dialog_response(): strange signal %d\n", response);
  }
}


/*! \brief Initialize NewText class
 *
 *  \par Function Description
 *
 *  GType class initialiser for Multiattrib. We override our parent
 *  virtual class methods as needed and register our GObject properties.
 *
 *  \param [in] klass
 */
static void newtext_class_init(NewTextClass *klass)
{
}



/*! \brief Initialize NewText instance
 *
 *  \param [in,out] dialog The new text dialog
 */
static void newtext_init(NewText *dialog)
{
  GtkWidget *alignment;
  GtkWidget *vbox;
  GtkWidget *label = NULL;
  GtkWidget *viewport1 = NULL;
  GtkWidget *scrolled_window = NULL;
  PangoTabArray *tab_array;
  int real_tab_width;
  GtkWidget *table;

  gtk_dialog_add_button(GTK_DIALOG(dialog),
                        GTK_STOCK_CLOSE,
                        GTK_RESPONSE_CLOSE);
  gtk_dialog_add_button(GTK_DIALOG(dialog),
                        GTK_STOCK_APPLY,
                        GTK_RESPONSE_APPLY);

  g_signal_connect(G_OBJECT(dialog), "response",
                   G_CALLBACK(text_input_dialog_response),
                   NULL);

  gtk_dialog_set_default_response(GTK_DIALOG(dialog),
                                  GTK_RESPONSE_APPLY);

  vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  gtk_box_set_spacing(GTK_BOX(vbox), DIALOG_V_SPACING);

  // Replace deprecated GtkTable with GtkGrid
  table = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(table), DIALOG_V_SPACING);
  gtk_grid_set_column_spacing(GTK_GRID(table), DIALOG_H_SPACING);

  label = gtk_label_new(_("<b>Text Properties</b>"));
  gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_set_valign(label, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

  // Replace GtkAlignment with GtkBox for padding
  alignment = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(vbox), alignment, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(alignment), table, FALSE, FALSE, DIALOG_INDENTATION);

  label = gtk_label_new(_("<b>Text Content</b>"));
  gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_set_valign(label, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

  label = gtk_label_new(_("Color:"));
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
  gtk_grid_attach(GTK_GRID(table), label, 0, 0, 1, 1);

  dialog->colorcb = x_colorcb_new();
  x_colorcb_set_index(dialog->colorcb, TEXT_COLOR);
  gtk_grid_attach(GTK_GRID(table), dialog->colorcb, 1, 0, 1, 1);

  label = gtk_label_new(_("Size:"));
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
  gtk_grid_attach(GTK_GRID(table), label, 0, 1, 1, 1);

  dialog->textsizecb = gschem_integer_combo_box_new();
  gschem_integer_combo_box_set_value(dialog->textsizecb, 12);
  gtk_grid_attach(GTK_GRID(table), dialog->textsizecb, 1, 1, 1, 1);

  label = gtk_label_new(_("Alignment:"));
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
  gtk_grid_attach(GTK_GRID(table), label, 0, 2, 1, 1);

  dialog->aligncb = gschem_alignment_combo_new();
  gschem_alignment_combo_set_align(dialog->aligncb, LOWER_LEFT);
  gtk_grid_attach(GTK_GRID(table), dialog->aligncb, 1, 2, 1, 1);

  label = gtk_label_new(_("Rotation:"));
  gtk_widget_set_halign(label, GTK_ALIGN_START);
  gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
  gtk_grid_attach(GTK_GRID(table), label, 0, 3, 1, 1);

  dialog->rotatecb = gschem_rotation_combo_new();
  gschem_rotation_combo_set_angle(dialog->rotatecb, 0);
  gtk_grid_attach(GTK_GRID(table), dialog->rotatecb, 1, 3, 1, 1);

  viewport1 = gtk_viewport_new(NULL, NULL);
  gtk_widget_show(viewport1);

  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(viewport1), scrolled_window);
  gtk_box_pack_start(GTK_BOX(vbox), viewport1, TRUE, TRUE, 0);

  dialog->text_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(dialog->text_view), TRUE);
  select_all_text_in_textview(GTK_TEXT_VIEW(dialog->text_view));

  /* Set the tab width, using pango tab array */
  /*! \bug FIXME: This doesn't work. Why? */
  tab_array = pango_tab_array_new(1, TRUE);
  real_tab_width = text_view_calculate_real_tab_width(GTK_TEXT_VIEW(dialog->text_view),
                                                     tab_in_chars);
  if (real_tab_width >= 0) {
    pango_tab_array_set_tab(tab_array, 0, PANGO_TAB_LEFT, real_tab_width);
    gtk_text_view_set_tabs(GTK_TEXT_VIEW(dialog->text_view),
                           tab_array);
  }
  else {
    g_warning("text_input_dialog: Impossible to set tab width.\n");
  }

  pango_tab_array_free(tab_array);
  gtk_container_add(GTK_CONTAINER(scrolled_window), dialog->text_view);
}



/*! \brief Get/register NewText type.
 */
GType newtext_get_type()
{
  static GType type = 0;

  if (type == 0) {
    static const GTypeInfo info = {
      sizeof(NewTextClass),
      NULL,                                   /* base_init */
      NULL,                                   /* base_finalize */
      (GClassInitFunc) newtext_class_init,
      NULL,                                   /* class_finalize */
      NULL,                                   /* class_data */
      sizeof(NewText),
      0,                                      /* n_preallocs */
      (GInstanceInitFunc) newtext_init,
    };

    type = g_type_register_static (GSCHEM_TYPE_DIALOG, "NewText", &info, 0);
  }

  return type;
}



/*! \brief Open the dialog box to add new text
 *
 *  \par Function Description
 *  This function creates or raises the modal text entry dialog
 *
 *  \param [in] w_current The gschem toplevel
 */
void
text_input_dialog (GschemToplevel *w_current)
{
  if (w_current->tiwindow == NULL) {
    /* dialog not created yet */
    w_current->tiwindow = g_object_new (TYPE_NEWTEXT,
                                        /* GtkContainer */
                                        "border-width",     DIALOG_BORDER_SPACING,
                                        /* GtkWindow */
                                        "title",            _("Text Entry..."),
                                        "default-width",    320,
                                        "default-height",   350,
                                        "window-position",  GTK_WIN_POS_MOUSE,
                                        "allow-grow",       TRUE,
                                        "allow-shrink",     FALSE,
                                        "modal",            FALSE,
                                        /* GtkDialog */
                                        "has-separator",    TRUE,
                                        /* GschemDialog */
                                        "settings-name",    "text-entry",
                                        "gschem-toplevel",  w_current,
                                        NULL);

    gtk_window_set_transient_for (GTK_WINDOW (w_current->tiwindow),
                                  GTK_WINDOW (w_current->main_window));

    gschem_integer_combo_box_set_model (NEWTEXT (w_current->tiwindow)->textsizecb,
                           gschem_toplevel_get_text_size_list_store (w_current));

    gtk_widget_show_all (w_current->tiwindow);
  }
  else {
    /* dialog already created */
    gtk_window_present (GTK_WINDOW(w_current->tiwindow));
  }

  /* always select the text in the entry */
  select_all_text_in_textview (GTK_TEXT_VIEW (NEWTEXT (w_current->tiwindow)->text_view));
  gtk_widget_grab_focus (NEWTEXT (w_current->tiwindow)->text_view);
}
