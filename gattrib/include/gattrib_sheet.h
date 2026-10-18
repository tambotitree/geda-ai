/* gEDA - GPL Electronic Design Automation
 * gattrib -- gEDA component and net attribute manipulation using spreadsheet.
 * Copyright (C) 2020-2026 gEDA developers
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

/*! \file gattrib_sheet.h
 *  \brief GTK3 attribute matrix, replacing the bundled GtkSheet 2.2 widget.
 *
 *  gattrib used to drive a vendored copy of GtkSheet (gtksheet_2_2.c), a GTK2
 *  widget that cannot be compiled against GTK3.  gattrib only ever needed a
 *  handful of things from it: a component-name row header, one column per
 *  attribute name, an editable text cell per (component, attribute) holding
 *  the attribute value, and a foreground colour per cell encoding that
 *  attribute's visibility.
 *
 *  GattribSheet provides exactly that on top of GtkTreeView.  The first column
 *  is the non-editable component name; each further column is an editable text
 *  attribute.  Cell colour is the visibility encoding, preserved from GtkSheet:
 *
 *      GREY   attribute is hidden
 *      BLUE   the attribute is shown as "name=value"
 *      RED    the attribute is shown as name only
 *      BLACK  the attribute is shown as value only
 *
 *  The colour constants are the ones in globals.h, which match the
 *  libgeda/include/colors.h scheme.
 *
 *  Selection state is exposed the way gattrib's visibility code needs it:
 *  gattrib_sheet_get_selection() reports the selected block of cells, and
 *  gattrib_sheet_get_selected_column() reports the single selected attribute
 *  column, or -1 when the selection is not exactly one whole column.
 */

#ifndef GATTRIB_SHEET_H
#define GATTRIB_SHEET_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GATTRIB_TYPE_SHEET            (gattrib_sheet_get_type ())
#define GATTRIB_SHEET(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GATTRIB_TYPE_SHEET, GattribSheet))
#define GATTRIB_SHEET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GATTRIB_TYPE_SHEET, GattribSheetClass))
#define GATTRIB_IS_SHEET(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GATTRIB_TYPE_SHEET))
#define GATTRIB_IS_SHEET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GATTRIB_TYPE_SHEET))

typedef struct _GattribSheet      GattribSheet;
typedef struct _GattribSheetClass GattribSheetClass;

/*! A block of selected cells.  Rows and columns are inclusive, and a single
 *  cell is expressed as row0 == rowi and col0 == coli.  All members are -1
 *  when nothing is selected. */
typedef struct {
  gint row0;
  gint rowi;
  gint col0;
  gint coli;
} GattribSheetSelection;

struct _GattribSheet {
  GtkBox parent_instance;

  /* Private; use the accessors. */
  GtkWidget *treeview;
  GtkListStore *store;
  GtkWidget *scrolled_window;

  gint n_rows;
  gint n_cols;          /* number of attribute columns, excluding the name */
  gboolean editable;
};

struct _GattribSheetClass {
  GtkBoxClass parent_class;

  /*! Emitted when the user edits a cell.  \a row and \a col are the attribute
   *  matrix coordinates, with \a col 0 being the first attribute (the
   *  component-name column is not addressable here). */
  void (* cell_edited) (GattribSheet *sheet, gint row, gint col,
                        const gchar *text);
};

GType gattrib_sheet_get_type (void) G_GNUC_CONST;

/* Construction ----------------------------------------------------------- */

/*! Create an empty sheet.  \a title is used as the accessible name. */
GtkWidget *gattrib_sheet_new (guint rows, guint cols, const gchar *title);

/* Contents --------------------------------------------------------------- */

/*! Set the component-name row labels from a NULL-terminated array.  Entries
 *  past \a count are ignored; missing entries become the empty string. */
void gattrib_sheet_set_row_labels (GattribSheet *sheet, gint count,
                                   gchar **labels);

/*! Set the attribute column titles from a NULL-terminated array.  This
 *  establishes the column count of the model. */
void gattrib_sheet_set_column_labels (GattribSheet *sheet, gint count,
                                      gchar **labels);

/*! Set one cell's text and colour.  \a colour is one of the gattrib colour
 *  constants (RED, BLUE, BLACK, GREY). */
void gattrib_sheet_set_cell (GattribSheet *sheet, gint row, gint col,
                             const gchar *text, gint colour);

/*! Retrieve a cell's text, or NULL if the cell is empty.  The result is owned
 *  by the sheet and must not be freed. */
const gchar *gattrib_sheet_get_cell_text (GattribSheet *sheet,
                                          gint row, gint col);

/*! Set a cell's foreground colour without touching its text. */
void gattrib_sheet_set_cell_colour (GattribSheet *sheet, gint row, gint col,
                                    gint colour);

/* Labels ---------------------------------------------------------------- */

/*! The title of attribute column \a col, or NULL when out of range.  The
 *  result is owned by the sheet. */
const gchar *gattrib_sheet_get_column_label (GattribSheet *sheet, gint col);

/*! The component-name label of \a row, or NULL when out of range. */
const gchar *gattrib_sheet_get_row_label (GattribSheet *sheet, gint row);

/* Dimensions ------------------------------------------------------------ */

gint gattrib_sheet_get_n_rows (GattribSheet *sheet);
gint gattrib_sheet_get_n_cols (GattribSheet *sheet);

/* Column management ----------------------------------------------------- */

/*! Append \a count attribute columns titled from \a labels. */
void gattrib_sheet_append_columns (GattribSheet *sheet, gint count,
                                   gchar **labels);

/*! Rename attribute column \a col. */
void gattrib_sheet_set_column_label (GattribSheet *sheet, gint col,
                                     const gchar *label);

/*! Remove the attribute column at \a col, shifting later columns left. */
void gattrib_sheet_delete_column (GattribSheet *sheet, gint col);

/* Enable / lock --------------------------------------------------------- */

/*! Enable or disable editing of every attribute cell. */
void gattrib_sheet_set_editable (GattribSheet *sheet, gboolean editable);

/* Selection ------------------------------------------------------------- */

/*! Fill \a sel with the selected block, or set every field to -1.  Returns
 *  \a sel for convenience. */
GattribSheetSelection *gattrib_sheet_get_selection (GattribSheet *sheet,
                                                    GattribSheetSelection *sel);

/*! The attribute column index if the whole selection is one column, else -1.
 *  This is the replacement for the old x_gtksheet_get_min_col() /
 *  x_gtksheet_get_max_col() pair. */
gint gattrib_sheet_get_selected_column (GattribSheet *sheet);

/*! The active (cursor) cell, or -1 in both fields when there is none. */
void gattrib_sheet_get_active_cell (GattribSheet *sheet, gint *row, gint *col);

/*! Select the whole attribute column \a col. */
void gattrib_sheet_select_column (GattribSheet *sheet, gint col);

/*! Drop the current selection. */
void gattrib_sheet_unselect_all (GattribSheet *sheet);

G_END_DECLS

#endif /* GATTRIB_SHEET_H */
