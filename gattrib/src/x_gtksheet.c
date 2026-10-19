/* gEDA - GPL Electronic Design Automation
 * gattrib -- gEDA component and net attribute manipulation using spreadsheet.
 * Copyright (C) 2003-2010 Stuart D. Brorson.
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

/*------------------------------------------------------------------*/
/*! \file
 * \brief Functions to interface to the spreadsheet widget.
 *
 * This file holds functions used to handle the spreadsheet widget.
 * Much of this was hacked from testgtksheet.c starting in Jan 2004 
 * by SDB.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*------------------------------------------------------------------
 * Includes required to run graphical widgets.
 *------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include <glib.h>
#include <glib-object.h>

#include <pango/pangocairo.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif


/*------------------------------------------------------------------
 * Gattrib specific includes
 *------------------------------------------------------------------*/
#include <libgeda/libgeda.h>       /* geda library fcns  */
#include "../include/struct.h"     /* typdef and struct declarations */
#include "../include/prototype.h"  /* function prototypes */
#include "../include/globals.h"
#include "../include/gettext.h"


/*! \brief Measure a text object without a page view
 *
 * libgeda calls back into the application to measure rendered text bounds.
 * gschem's implementation renders through an EdaRenderer on a live page view;
 * gattrib has neither, so the string is measured directly with a Pango layout.
 *
 * Only the extents are needed here, for the work that walks schematic text
 * (attribute extraction and net listing), never for drawing, so an
 * approximation based on Pango metrics is sufficient and avoids pulling in the
 * whole cairo rendering stack.
 *
 * \param [in]  user_data unused
 * \param [in]  object    text object to measure
 * \param [out] min_x      left world coord
 * \param [out] min_y      top world coord
 * \param [out] max_x      right world coord
 * \param [out] max_y      bottom world coord
 * \returns TRUE on success
 */
static int
x_gtksheet_text_bounds(void *user_data, OBJECT *object,
                       int *min_x, int *min_y, int *max_x, int *max_y)
{
  const gchar *text;
  PangoLayout *layout;
  PangoContext *context;
  gint width, height;
  gint origin_x, origin_y;

  /* The registered libgeda callback needs no application context. */
  (void) user_data;

  if ((object == NULL) || (object->text == NULL))
    return FALSE;
  if (object->type != OBJ_TEXT)
    return FALSE;

  text = object->text->disp_string != NULL ? object->text->disp_string
                                           : object->text->string;
  if (text == NULL)
    text = "";

  /* A context created from the default font map has one attached; a bare
   * pango_context_new() does not, and measuring with one raises
   * "context->font_map != NULL" criticals. */
  context = pango_font_map_create_context (pango_cairo_font_map_get_default ());
  layout = pango_layout_new (context);
  pango_layout_set_text (layout, text, -1);
  pango_layout_get_pixel_size (layout, &width, &height);

  /* Scale from the object's nominal font size, if one is set, to the size the
   * layout was measured at. */
  if (object->text->size > 0) {
    gdouble scale = (gdouble) object->text->size / 12.0;
    width = (gint) (width * scale);
    height = (gint) (height * scale);
  }

  origin_x = object->text->x;
  origin_y = object->text->y;

  if (min_x != NULL) *min_x = origin_x;
  if (min_y != NULL) *min_y = origin_y - height;
  if (max_x != NULL) *max_x = origin_x + width;
  if (max_y != NULL) *max_y = origin_y;

  g_object_unref (layout);
  g_object_unref (context);

  return TRUE;
}

/*! \brief Register the text-bounds callback with libgeda
 *
 * Must be called once the TOPLEVEL exists; world_get_text_bounds() asserts
 * that a callback is installed.
 * \param toplevel the TOPLEVEL to configure
 */
void
x_gtksheet_set_text_bounds_func(TOPLEVEL *toplevel)
{
  if (toplevel != NULL)
    o_text_set_rendered_bounds_func(toplevel, x_gtksheet_text_bounds, NULL);
}


/*! \brief Create the attribute sheet widget
 *
 * Creates and initializes the sheet widget used to display the attribute data.
 * Only the Components sheet is implemented; the Nets and Pins pages were never
 * finished, so nothing is created for them.
 */
void
x_gtksheet_init()
{
  gint i;
  const gchar *folder[]= {_("Components")};

  /* ---  Create the Components sheet.  Its slot was malloc'ed in x_window_init  --- */
  if ((sheet_head->comp_count > 0) && (sheet_head->comp_attrib_count > 0)) {
    sheets[0] = GATTRIB_SHEET (gattrib_sheet_new ((guint) sheet_head->comp_count,
                                                  (guint) sheet_head->comp_attrib_count,
                                                  _("Components")));
  } else {
    x_dialog_fatal_error(_("No components found in design.  Please check your schematic and try again!\n"), 1);
  }

  /* --- Finally stick the page label on the notebook holding the sheet. --- */
  for (i = 0; i < NUM_SHEETS; i++) {
    GtkWidget *label;

    if (sheets[i] == NULL) {
      continue;
    }

    scrolled_windows = (GtkWidget **) g_realloc (scrolled_windows,
                                                (i + 1) * sizeof (GtkWidget *));
    scrolled_windows[i] = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_windows[i]),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add (GTK_CONTAINER (scrolled_windows[i]),
                       GTK_WIDGET (sheets[i]));

    /* First remove old notebook page.  I should probably do some checking here. */
    if (notebook != NULL)
      gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), i);

    /* Then add new, updated notebook page */
    label = gtk_label_new (folder[i]);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
                             GTK_WIDGET (scrolled_windows[i]),
                             GTK_WIDGET (label));

    gtk_widget_show (GTK_WIDGET (sheets[i]));
    gtk_widget_show (GTK_WIDGET (scrolled_windows[i]));
    gtk_widget_show (GTK_WIDGET (notebook));  /* show updated notebook  */

    /* A GtkTreeView has no persistent in-cell GtkEntry, so the old "changed"
     * hook on gtk_sheet_get_entry() is gone.  Cell edits are delivered through
     * GattribSheet's "cell-edited" signal instead, which gattrib does not
     * currently subscribe to. */
  }
}


/*------------------------------------------------------------------*/
/*! \brief Add row labels to the attribute sheet
 *
 * Add component-name row labels to the sheet.
 * \param sheet sheet to add rows to
 * \param count number of labels
 * \param list_head pointer to top of STRING_LIST
 */
void
x_gtksheet_add_row_labels(GattribSheet *sheet, int count, STRING_LIST *list_head)
{
  STRING_LIST *string_list_item;
  gchar **labels;
  int j;

  /* Leave if no items to add are available */
  if ((count == 0) || (list_head == NULL)) return;

  labels = g_new0 (gchar *, count);

  string_list_item = list_head;
  for (j = 0; j < count && string_list_item != NULL; j++) {
    labels[j] = (gchar *) string_list_item->data;
    string_list_item = string_list_item->next;
  }

  gattrib_sheet_set_row_labels (sheet, count, labels);
  g_free (labels);
}


/*------------------------------------------------------------------*/
/*! \brief Add column labels to the attribute sheet
 *
 * Add attribute-name column labels to the sheet.
 * \param sheet sheet to add columns to
 * \param count number of labels
 * \param list_head pointer to top of STRING_LIST
 */
void
x_gtksheet_add_col_labels(GattribSheet *sheet, int count, STRING_LIST *list_head)
{
  STRING_LIST *string_list_item;
  gchar **labels;
  int j;

  /* Leave if no items to add are available */
  if ((count == 0) || (list_head == NULL)) return;

  labels = g_new0 (gchar *, count);

  string_list_item = list_head;
  for (j = 0; j < count && string_list_item != NULL; j++) {
    labels[j] = (gchar *) string_list_item->data;
    string_list_item = string_list_item->next;
  }

  gattrib_sheet_set_column_labels (sheet, count, labels);
  g_free (labels);
}


/*------------------------------------------------------------------*/
/*! \brief Add a cell item to the attribute sheet
 *
 * Add a cell item to the sheet, colouring it according to the attribute's
 * visibility state.
 * \param sheet sheet to add the cell item to
 * \param i row
 * \param j column
 * \param text cell text
 * \param visibility INVISIBLE or VISIBLE
 * \param show_name_value SHOW_NAME_VALUE, SHOW_NAME or SHOW_VALUE
 */
void
x_gtksheet_add_cell_item(GattribSheet *sheet, gint i, gint j,
                         gchar *text,
                         gint visibility,
                         gint show_name_value)
{
  gint color;

  /*  Should I do some sanity checking here?  */

  if (visibility == INVISIBLE) {
    color = GREY;
  } else {
    switch(show_name_value) {

    case(SHOW_NAME_VALUE):
      color = BLUE;
      break;

    case(SHOW_NAME):
      color = RED;
      break;

    case(SHOW_VALUE):
    default:
      color = BLACK;
      break;
    }
  } /* if (visibility == INVISIBLE) */

  gattrib_sheet_set_cell (sheet, i, j, text, color);

  /* Long attribute values are ellipsized by the column renderer rather than
   * overwriting adjacent cells. */
}


/*! \brief Get the first column selected in the sheet
 *
 * Returns the index of the selected column, or -1 if no single column is
 * selected.
 * \param sheet sheet to query
 * \returns index of the selected column, or -1
 */
int x_gtksheet_get_min_col(GattribSheet *sheet) {
  return gattrib_sheet_get_selected_column (sheet);
}


/*! \brief Get the last column selected in the sheet
 *
 * Returns the index of the selected column, or -1 if no single column is
 * selected.  A GtkTreeView selection is a set of rows rather than a rectangle,
 * so the first and last selected column are always the same.
 * \param sheet sheet to query
 * \returns index of the selected column, or -1
 */
int x_gtksheet_get_max_col(GattribSheet *sheet) {
  return gattrib_sheet_get_selected_column (sheet);
}


/*! \brief Set the text color of a cell
 *
 * Sets the color of a cell identified by row, col.
 * \param sheet sheet to operate on
 * \param row Row of cell to set
 * \param col Column of cell to set
 * \param color_name Color to set text to
 */
void x_gtksheet_set_cell_text_color(GattribSheet *sheet, gint row, gint col,
                                    gint color_name)
{
  gattrib_sheet_set_cell_colour (sheet, row, col, color_name);
}
