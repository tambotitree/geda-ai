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

/*! \file gattrib_sheet.c
 *  \brief GTK3 implementation of the attribute matrix.
 *
 *  The model is a GtkListStore with one column per attribute, which keeps row
 *  and column indices identical to the old GtkSheet coordinates: row \a i,
 *  attribute \a j is list-store row \a i, model column \a j+1 (column 0 holds
 *  the component name).  Nothing outside this file needs to know that offset.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtk/gtk.h>
#include <string.h>

#include "gattrib_sheet.h"
#include "gettext.h"

/* Model layout.  Each attribute j occupies two model columns: its text at
 * ATTR_TEXT(j) and its foreground colour at ATTR_COLOUR(j), so that the
 * "foreground" property can come from the model without overwriting the text.
 * ATTR_TEXT(0) == COL_ATTR_BASE, so the text-column arithmetic stays simple. */
enum {
  COL_NAME = 0,
  COL_ATTR_BASE = 1
};

#define ATTR_TEXT(j)   (COL_ATTR_BASE + 2 * (j))
#define ATTR_COLOUR(j) (COL_ATTR_BASE + 2 * (j) + 1)
#define MODEL_N_COLUMNS(n_cols) (COL_ATTR_BASE + 2 * (n_cols))

/* These mirror the colour constants in globals.h.  That header cannot be
 * included here because it depends on the libgeda and gattrib data types, and
 * this widget deliberately stays independent of both. */
#define RED   2
#define BLUE  4
#define GREY  7
#define BLACK 0

/* Forward declarations for handlers referenced before their definitions. */
static void on_column_clicked (GtkTreeViewColumn *column, gpointer user_data);
static gboolean get_row_iter (GattribSheet *sheet, gint row, GtkTreeIter *iter);


G_DEFINE_TYPE (GattribSheet, gattrib_sheet, GTK_TYPE_BOX)

G_DEFINE_QUARK (gattrib-sheet-column-index, gattrib_sheet_column_index)

/*! \brief Map a gattrib colour constant to the string handed to
 *  GtkCellRendererText's "foreground" property.
 *
 *  Returns NULL for BLACK so that the renderer falls back to the theme's normal
 *  text colour: hard-coding "#000000" would be unreadable in a dark theme, and
 *  BLACK means "no special state" here.
 */
static const gchar *
colour_to_string (gint colour)
{
  switch (colour) {
  case RED:   return "#cc0000";
  case BLUE:  return "#1a4fbf";
  case GREY:  return "#888888";
  case BLACK:
  default:    return NULL;
  }
}

static void
on_renderer_edited (GtkCellRendererText *renderer,
                    gchar               *path,
                    gchar               *new_text,
                    gpointer             user_data)
{
  GattribSheet *sheet = GATTRIB_SHEET (user_data);
  gint col = GPOINTER_TO_INT (g_object_get_qdata (G_OBJECT (renderer),
                                                 gattrib_sheet_column_index_quark ()));
  GtkTreeIter iter;
  gint row = -1;

  if (col < 0 || !sheet->editable)
    return;

  if (!gtk_tree_model_get_iter_from_string (GTK_TREE_MODEL (sheet->store),
                                            &iter, path))
    return;

  /* The list store is flat, so the row index is the model path index. */
  {
    GtkTreePath *tpath = gtk_tree_model_get_path (GTK_TREE_MODEL (sheet->store),
                                                 &iter);
    if (tpath != NULL) {
      row = gtk_tree_path_get_indices (tpath)[0];
      gtk_tree_path_free (tpath);
    }
  }
  if (row < 0)
    return;

  gtk_list_store_set (sheet->store, &iter,
                      ATTR_TEXT (col), new_text ? new_text : "",
                      -1);

  g_signal_emit_by_name (sheet, "cell-edited", row, col,
                         new_text ? new_text : "");
}

static void
gattrib_sheet_class_init (GattribSheetClass *class)
{
  /*! Emitted when the user commits an edit.  See the header for the
   *  coordinate convention. */
  g_signal_new ("cell-edited",
                G_TYPE_FROM_CLASS (class),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (GattribSheetClass, cell_edited),
                NULL, NULL, NULL,
                G_TYPE_NONE, 3,
                G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING);
}

static void
gattrib_sheet_init (GattribSheet *sheet)
{
  sheet->n_rows = 0;
  sheet->n_cols = 0;
  sheet->editable = TRUE;

  gtk_orientable_set_orientation (GTK_ORIENTABLE (sheet),
                                  GTK_ORIENTATION_VERTICAL);

  sheet->store = gtk_list_store_new (COL_ATTR_BASE, G_TYPE_STRING);
  sheet->scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sheet->scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  gtk_widget_set_vexpand (sheet->scrolled_window, TRUE);
  gtk_container_add (GTK_CONTAINER (sheet), sheet->scrolled_window);

  sheet->treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (sheet->store));
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (sheet->treeview), TRUE);
  gtk_tree_view_set_grid_lines (GTK_TREE_VIEW (sheet->treeview),
                                GTK_TREE_VIEW_GRID_LINES_BOTH);
  gtk_container_add (GTK_CONTAINER (sheet->scrolled_window), sheet->treeview);
}

GtkWidget *
gattrib_sheet_new (guint rows, guint cols, const gchar *title)
{
  GattribSheet *sheet;
  GtkTreeSelection *selection;

  sheet = g_object_new (GATTRIB_TYPE_SHEET, NULL);

  if (title != NULL) {
    AtkObject *aobj = gtk_widget_get_accessible (GTK_WIDGET (sheet));
    if (aobj != NULL)
      atk_object_set_name (aobj, title);
  }

  gattrib_sheet_set_column_labels (sheet, (gint) cols, NULL);
  gattrib_sheet_set_row_labels (sheet, (gint) rows, NULL);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (sheet->treeview));
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_MULTIPLE);

  return GTK_WIDGET (sheet);
}

/*! \brief (Re)build the column layout: component name plus one editable
 *  attribute column per model column.
 *
 *  The model is recreated because its column count changes, and the columns
 *  are rebuilt because each one needs its own GtkCellRendererText with its own
 *  column index attached.
 */
static void
rebuild_columns (GattribSheet *sheet, gchar **labels)
{
  GType *types;
  GtkTreeViewColumn *column;
  GtkCellRenderer *renderer;
  gint i;

  gtk_tree_view_set_model (GTK_TREE_VIEW (sheet->treeview), NULL);
  g_clear_object (&sheet->store);

  types = g_new0 (GType, MODEL_N_COLUMNS (sheet->n_cols));
  for (i = 0; i < MODEL_N_COLUMNS (sheet->n_cols); i++)
    types[i] = G_TYPE_STRING;

  sheet->store = gtk_list_store_newv (MODEL_N_COLUMNS (sheet->n_cols), types);
  g_free (types);

  gtk_tree_view_set_model (GTK_TREE_VIEW (sheet->treeview),
                           GTK_TREE_MODEL (sheet->store));

  /* Remove every existing column. */
  {
    GList *cols = gtk_tree_view_get_columns (GTK_TREE_VIEW (sheet->treeview));
    GList *iter;
    for (iter = cols; iter != NULL; iter = iter->next)
      gtk_tree_view_remove_column (GTK_TREE_VIEW (sheet->treeview),
                                   GTK_TREE_VIEW_COLUMN (iter->data));
    g_list_free (cols);
  }

  /* Component name: read-only, and the row header. */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes (_("Component"),
                                                     renderer,
                                                     "text", COL_NAME,
                                                     NULL);
  gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (sheet->treeview), column);

  /* One editable attribute column each. */
  for (i = 0; i < sheet->n_cols; i++) {
    const gchar *label = (labels != NULL && labels[i] != NULL) ? labels[i] : "";

    renderer = gtk_cell_renderer_text_new ();
    g_object_set (renderer,
                  "editable", sheet->editable,
                  "ellipsize", PANGO_ELLIPSIZE_END,
                  NULL);

    /* Attach the attribute index to the renderer so the shared "edited"
     * handler knows which model column was changed. */
    g_object_set_qdata (G_OBJECT (renderer),
                        gattrib_sheet_column_index_quark (),
                        GINT_TO_POINTER (i));
    g_signal_connect (renderer, "edited",
                      G_CALLBACK (on_renderer_edited), sheet);

    column = gtk_tree_view_column_new_with_attributes (label, renderer,
                                                       "text",
                                                       ATTR_TEXT (i),
                                                       "foreground",
                                                       ATTR_COLOUR (i),
                                                       NULL);
    gtk_tree_view_column_set_clickable (column, TRUE);
    g_signal_connect (column, "clicked", G_CALLBACK (on_column_clicked), sheet);
    gtk_tree_view_column_set_sizing (column, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
    gtk_tree_view_column_set_resizable (column, TRUE);
    gtk_tree_view_column_set_expand (column, TRUE);
    gtk_tree_view_append_column (GTK_TREE_VIEW (sheet->treeview), column);
  }
}

/*! \brief Programmatically select every row of attribute column \a col.
 *
 *  Used when a column header is clicked, so that the whole-column selection
 *  gattrib's delete/visibility actions expect is easy to produce. */
static void
select_whole_column (GattribSheet *sheet, gint col)
{
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GtkTreeIter iter;
  gboolean valid;

  if (col < 0 || col >= sheet->n_cols)
    return;

  model = GTK_TREE_MODEL (sheet->store);
  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (sheet->treeview));

  /* Order matters: gtk_tree_view_set_cursor() collapses the selection down to
   * the cursor row, so the cursor must be placed before the rows are selected.
   * Selecting first and moving the cursor afterwards leaves a single-row
   * selection. */
  if (gtk_tree_model_get_iter_first (model, &iter)) {
    GList *columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (sheet->treeview));
    GtkTreeViewColumn *column = g_list_nth_data (columns, col + 1);
    GtkTreePath *path = gtk_tree_model_get_path (model, &iter);

    g_list_free (columns);

    if (column != NULL) {
      gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (sheet->treeview),
                                    path, column, FALSE, 0.0f, 0.0f);
      gtk_tree_view_set_cursor (GTK_TREE_VIEW (sheet->treeview),
                                path, column, FALSE);
    }
    gtk_tree_path_free (path);
  }

  gtk_tree_selection_unselect_all (selection);
  for (valid = gtk_tree_model_get_iter_first (model, &iter);
       valid;
       valid = gtk_tree_model_iter_next (model, &iter)) {
    gtk_tree_selection_select_iter (selection, &iter);
  }
}

static void
on_column_clicked (GtkTreeViewColumn *column, gpointer user_data)
{
  GattribSheet *sheet = GATTRIB_SHEET (user_data);
  GList *columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (sheet->treeview));
  gint index = g_list_index (columns, column);

  g_list_free (columns);

  /* Column 0 is the component name, which is not an attribute. */
  if (index > 0)
    select_whole_column (sheet, index - 1);
}

void
gattrib_sheet_set_column_labels (GattribSheet *sheet, gint count, gchar **labels)
{
  gint old_rows, old_cols;
  gchar ***saved_text;
  gchar ***saved_colour;
  gchar **saved_names;
  gint i, j;

  g_return_if_fail (GATTRIB_IS_SHEET (sheet));
  g_return_if_fail (count >= 0);

  old_rows = sheet->n_rows;
  old_cols = sheet->n_cols;

  if (count == old_cols) {
    /* Same width: just retitle the columns, leaving all data untouched. */
    for (j = 0; j < count; j++) {
      const gchar *label = (labels != NULL && labels[j] != NULL) ? labels[j] : "";
      gattrib_sheet_set_column_label (sheet, j, label);
    }
    return;
  }

  /* The store width changes, so it has to be rebuilt.  Preserve whatever is
   * already in the cells: callers are not required to set labels before data,
   * and x_window.c populates rows before columns. */
  saved_text = g_new0 (gchar **, old_rows);
  saved_colour = g_new0 (gchar **, old_rows);
  saved_names = g_new0 (gchar *, old_rows);

  for (i = 0; i < old_rows; i++) {
    GtkTreeIter iter;
    saved_text[i] = g_new0 (gchar *, old_cols);
    saved_colour[i] = g_new0 (gchar *, old_cols);
    if (get_row_iter (sheet, i, &iter)) {
      gchar *name = NULL;
      gtk_tree_model_get (GTK_TREE_MODEL (sheet->store), &iter,
                          COL_NAME, &name, -1);
      /* Take owned copies: rebuild_columns() destroys the store these strings
       * belong to, so the raw pointers would dangle. */
      saved_names[i] = g_strdup (name);
      g_free (name);
      for (j = 0; j < old_cols; j++) {
        gchar *t = NULL, *cl = NULL;
        gtk_tree_model_get (GTK_TREE_MODEL (sheet->store), &iter,
                            ATTR_TEXT (j), &t,
                            ATTR_COLOUR (j), &cl,
                            -1);
        saved_text[i][j] = g_strdup (t);
        saved_colour[i][j] = g_strdup (cl);
        g_free (t);
        g_free (cl);
      }
    }
  }

  sheet->n_cols = count;
  rebuild_columns (sheet, labels);

  /* Create the rows first, then lay the saved content into them: the restore
   * loop needs the rows to exist, and set_row_labels() clears the store. */
  gattrib_sheet_set_row_labels (sheet, old_rows, saved_names);

  for (i = 0; i < old_rows; i++) {
    GtkTreeIter iter;
    gint keep = old_cols < count ? old_cols : count;

    if (!get_row_iter (sheet, i, &iter))
      continue;
    for (j = 0; j < keep; j++)
      gtk_list_store_set (sheet->store, &iter,
                          ATTR_TEXT (j), saved_text[i][j],
                          ATTR_COLOUR (j), saved_colour[i][j],
                          -1);
  }

  for (i = 0; i < old_rows; i++) {
    for (j = 0; j < old_cols; j++) {
      g_free (saved_text[i][j]);
      g_free (saved_colour[i][j]);
    }
    g_free (saved_text[i]);
    g_free (saved_colour[i]);
    g_free (saved_names[i]);
  }
  g_free (saved_text);
  g_free (saved_colour);
  g_free (saved_names);
}

void
gattrib_sheet_set_row_labels (GattribSheet *sheet, gint count, gchar **labels)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  gboolean valid;
  gint row;
  gchar **own_labels = NULL;
  gint present;

  g_return_if_fail (GATTRIB_IS_SHEET (sheet));
  g_return_if_fail (count >= 0);

  /* Count the rows actually present; n_rows alone is not trustworthy after a
   * store rebuild. */
  model = GTK_TREE_MODEL (sheet->store);
  {
    gint actual = 0;
    for (valid = gtk_tree_model_get_iter_first (model, &iter);
         valid;
         valid = gtk_tree_model_iter_next (model, &iter))
      actual++;
    present = actual;
  }

  if (count != present) {
    /* Resize the store.  When the caller supplies no labels, carry the existing
     * component names across so that a resize alone never loses identity. */
    gint k = 0;
    /* Nobody asked to change the names, so capture them for re-application. */
    if (labels == NULL && present > 0) {
      own_labels = g_new0 (gchar *, present + 1);
      model = GTK_TREE_MODEL (sheet->store);
      for (valid = gtk_tree_model_get_iter_first (model, &iter);
           valid && k < present;
           valid = gtk_tree_model_iter_next (model, &iter)) {
        gchar *nm = NULL;
        gtk_tree_model_get (model, &iter, COL_NAME, &nm, -1);
        own_labels[k++] = nm;
      }
      labels = own_labels;
    }

    sheet->n_rows = count;
    gtk_list_store_clear (sheet->store);
    for (row = 0; row < count; row++)
      gtk_list_store_append (sheet->store, &iter);
  }

  /* Only the names change here; cell contents must survive. */
  if (labels != NULL) {
    model = GTK_TREE_MODEL (sheet->store);
    row = 0;
    for (valid = gtk_tree_model_get_iter_first (model, &iter);
         valid && row < count;
         valid = gtk_tree_model_iter_next (model, &iter)) {
      const gchar *name = labels[row] != NULL ? labels[row] : "";
      gtk_list_store_set (sheet->store, &iter, COL_NAME, name, -1);
      row++;
    }
  }

  /* The captured names are ours to release, now that they have been copied. */
  if (own_labels != NULL) {
    gint k;
    for (k = 0; own_labels[k] != NULL; k++)
      g_free (own_labels[k]);
    g_free (own_labels);
  }
}

/*! \brief Fetch the iterator for \a row, returning FALSE when out of range. */
static gboolean
get_row_iter (GattribSheet *sheet, gint row, GtkTreeIter *iter)
{
  if (row < 0 || row >= sheet->n_rows)
    return FALSE;
  return gtk_tree_model_iter_nth_child (GTK_TREE_MODEL (sheet->store), iter,
                                        NULL, row);
}

void
gattrib_sheet_set_cell (GattribSheet *sheet, gint row, gint col,
                        const gchar *text, gint colour)
{
  GtkTreeIter iter;

  g_return_if_fail (GATTRIB_IS_SHEET (sheet));
  if (col < 0 || col >= sheet->n_cols)
    return;
  if (!get_row_iter (sheet, row, &iter))
    return;

  gtk_list_store_set (sheet->store, &iter,
                      ATTR_TEXT (col), text ? text : "",
                      ATTR_COLOUR (col), colour_to_string (colour),
                      -1);
}

const gchar *
gattrib_sheet_get_cell_text (GattribSheet *sheet, gint row, gint col)
{
  GtkTreeIter iter;
  gchar *text = NULL;
  const gchar *result;

  g_return_val_if_fail (GATTRIB_IS_SHEET (sheet), NULL);
  if (col < 0 || col >= sheet->n_cols)
    return NULL;
  if (!get_row_iter (sheet, row, &iter))
    return NULL;

  gtk_tree_model_get (GTK_TREE_MODEL (sheet->store), &iter,
                      ATTR_TEXT (col), &text, -1);

  /* Callers historically treated the result as borrowed (gtk_sheet_cell_get_text
   * returned a pointer into the widget), so hand back an interned copy rather
   * than leaking a freshly duplicated string per read.
   *
   * An empty cell is reported as NULL, which is what the header promises and
   * what callers use to tell "no attribute" from "attribute with an empty
   * value".  The store itself holds "" because GtkTreeView renderers want a
   * string. */
  if (text == NULL || text[0] == '\0') {
    g_free (text);
    return NULL;
  }

  result = g_intern_string (text);
  g_free (text);
  return result;
}

void
gattrib_sheet_set_cell_colour (GattribSheet *sheet, gint row, gint col,
                               gint colour)
{
  GtkTreeIter iter;

  g_return_if_fail (GATTRIB_IS_SHEET (sheet));
  if (col < 0 || col >= sheet->n_cols)
    return;
  if (!get_row_iter (sheet, row, &iter))
    return;

  gtk_list_store_set (sheet->store, &iter,
                      ATTR_COLOUR (col), colour_to_string (colour),
                      -1);
}

const gchar *
gattrib_sheet_get_column_label (GattribSheet *sheet, gint col)
{
  GtkTreeViewColumn *column;
  GList *columns;

  g_return_val_if_fail (GATTRIB_IS_SHEET (sheet), NULL);
  if (col < 0 || col >= sheet->n_cols)
    return NULL;

  columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (sheet->treeview));
  column = g_list_nth_data (columns, col + 1); /* +1 skips the name column */
  g_list_free (columns);

  return column != NULL ? gtk_tree_view_column_get_title (column) : NULL;
}

const gchar *
gattrib_sheet_get_row_label (GattribSheet *sheet, gint row)
{
  static const gchar *empty = "";
  GtkTreeIter iter;
  gchar *text = NULL;
  const gchar *result;

  g_return_val_if_fail (GATTRIB_IS_SHEET (sheet), empty);
  if (!get_row_iter (sheet, row, &iter))
    return empty;

  gtk_tree_model_get (GTK_TREE_MODEL (sheet->store), &iter, COL_NAME, &text, -1);
  result = text ? g_intern_string (text) : empty;
  g_free (text);
  return result;
}

gint
gattrib_sheet_get_n_rows (GattribSheet *sheet)
{
  g_return_val_if_fail (GATTRIB_IS_SHEET (sheet), 0);
  return sheet->n_rows;
}

gint
gattrib_sheet_get_n_cols (GattribSheet *sheet)
{
  g_return_val_if_fail (GATTRIB_IS_SHEET (sheet), 0);
  return sheet->n_cols;
}

/* Column management ----------------------------------------------------- */

void
gattrib_sheet_append_columns (GattribSheet *sheet, gint count, gchar **labels)
{
  gchar **titles;
  gint old_cols, new_cols, j;

  g_return_if_fail (GATTRIB_IS_SHEET (sheet));
  g_return_if_fail (count >= 0);

  if (count == 0)
    return;

  old_cols = sheet->n_cols;
  new_cols = old_cols + count;

  /* Build the full title list: existing titles plus the new ones.
   * set_column_labels() preserves existing cell contents across the rebuild. */
  titles = g_new0 (gchar *, new_cols + 1);

  for (j = 0; j < old_cols; j++) {
    const gchar *t = gattrib_sheet_get_column_label (sheet, j);
    titles[j] = g_strdup (t ? t : "");
  }
  for (j = 0; j < count; j++) {
    const gchar *t = (labels != NULL && labels[j] != NULL) ? labels[j] : "";
    titles[old_cols + j] = g_strdup (t);
  }

  gattrib_sheet_set_column_labels (sheet, new_cols, titles);

  for (j = 0; j < new_cols; j++)
    g_free (titles[j]);
  g_free (titles);
}

void
gattrib_sheet_set_column_label (GattribSheet *sheet, gint col, const gchar *label)
{
  GtkTreeViewColumn *column;
  GList *columns;

  g_return_if_fail (GATTRIB_IS_SHEET (sheet));
  if (col < 0 || col >= sheet->n_cols)
    return;

  columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (sheet->treeview));
  column = g_list_nth_data (columns, col + 1);
  g_list_free (columns);

  if (column != NULL)
    gtk_tree_view_column_set_title (column, label ? label : "");
}

void
gattrib_sheet_delete_column (GattribSheet *sheet, gint col)
{
  GtkTreeViewColumn *column;
  GList *columns;

  g_return_if_fail (GATTRIB_IS_SHEET (sheet));
  if (col < 0 || col >= sheet->n_cols)
    return;

  columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (sheet->treeview));
  column = g_list_nth_data (columns, col + 1);
  g_list_free (columns);

  /* Removing the view column also drops the model column's visible data.  The
   * store keeps its width until the next rebuild, which is harmless because
   * n_cols no longer exposes the removed column. */
  if (column != NULL)
    gtk_tree_view_remove_column (GTK_TREE_VIEW (sheet->treeview), column);

  sheet->n_cols--;
}

/* Enable / lock --------------------------------------------------------- */

void
gattrib_sheet_set_editable (GattribSheet *sheet, gboolean editable)
{
  GList *columns;
  GList *iter;

  g_return_if_fail (GATTRIB_IS_SHEET (sheet));

  sheet->editable = editable;

  columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (sheet->treeview));
  for (iter = columns; iter != NULL; iter = iter->next) {
    GtkTreeViewColumn *column = GTK_TREE_VIEW_COLUMN (iter->data);
    GList *cells = gtk_cell_layout_get_cells (GTK_CELL_LAYOUT (column));
    GList *c;

    for (c = cells; c != NULL; c = c->next) {
      if (GTK_IS_CELL_RENDERER_TEXT (c->data))
        g_object_set (c->data, "editable", editable, NULL);
    }
    g_list_free (cells);
  }
  g_list_free (columns);
}

/* Selection ------------------------------------------------------------- */

GattribSheetSelection *
gattrib_sheet_get_selection (GattribSheet *sheet, GattribSheetSelection *sel)
{
  GtkTreeSelection *selection;
  GtkTreeModel *model;
  GList *rows;
  GtkTreePath *cursor;
  GtkTreeViewColumn *focus = NULL;
  gboolean have = FALSE;

  g_return_val_if_fail (sel != NULL, NULL);

  sel->row0 = sel->rowi = sel->col0 = sel->coli = -1;

  if (!GATTRIB_IS_SHEET (sheet))
    return sel;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (sheet->treeview));
  model = GTK_TREE_MODEL (sheet->store);
  rows = gtk_tree_selection_get_selected_rows (selection, &model);

  if (rows != NULL) {
    GList *iter;
    gint first = G_MAXINT, last = -1;

    for (iter = rows; iter != NULL; iter = iter->next) {
      gint *indices = gtk_tree_path_get_indices ((GtkTreePath *) iter->data);
      gint r = indices != NULL ? indices[0] : -1;
      if (r >= 0) {
        if (r < first) first = r;
        if (r > last)  last = r;
      }
    }
    g_list_free_full (rows, (GDestroyNotify) gtk_tree_path_free);

    if (last >= 0) {
      sel->row0 = first;
      sel->rowi = last;
      have = TRUE;
    }
  }

  /* A tree view has no notion of a selected column, so derive it from the
   * focused cell.  When the selection came from a column-header click or
   * gattrib_sheet_select_column(), every row is selected and the focused
   * column is the one the user asked about. */
  gtk_tree_view_get_cursor (GTK_TREE_VIEW (sheet->treeview), &cursor, &focus);
  if (cursor != NULL)
    gtk_tree_path_free (cursor);

  if (have && focus != NULL) {
    GList *columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (sheet->treeview));
    gint index = g_list_index (columns, focus);
    g_list_free (columns);
    if (index > 0) {
      sel->col0 = index - 1;
      sel->coli = index - 1;
    }
  }

  return sel;
}

gint
gattrib_sheet_get_selected_column (GattribSheet *sheet)
{
  GtkTreeSelection *selection;
  GtkTreePath *cursor;
  GtkTreeViewColumn *focus = NULL;
  GList *rows;
  gint count, index;

  g_return_val_if_fail (GATTRIB_IS_SHEET (sheet), -1);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (sheet->treeview));
  rows = gtk_tree_selection_get_selected_rows (selection, NULL);
  count = g_list_length (rows);
  g_list_free_full (rows, (GDestroyNotify) gtk_tree_path_free);

  /* Require the whole column to be selected, matching the old behaviour where
   * a column had to be selected as a unit before gattrib would act on it. */
  if (count == 0 || count != sheet->n_rows)
    return -1;

  gtk_tree_view_get_cursor (GTK_TREE_VIEW (sheet->treeview), &cursor, &focus);
  if (cursor != NULL)
    gtk_tree_path_free (cursor);

  if (focus == NULL)
    return -1;

  {
    GList *columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (sheet->treeview));
    index = g_list_index (columns, focus);
    g_list_free (columns);
  }

  return index > 0 ? index - 1 : -1;
}

void
gattrib_sheet_get_active_cell (GattribSheet *sheet, gint *row, gint *col)
{
  GtkTreePath *path = NULL;
  GtkTreeViewColumn *focus = NULL;

  if (row != NULL) *row = -1;
  if (col != NULL) *col = -1;

  g_return_if_fail (GATTRIB_IS_SHEET (sheet));

  gtk_tree_view_get_cursor (GTK_TREE_VIEW (sheet->treeview), &path, &focus);

  if (path != NULL) {
    gint *indices = gtk_tree_path_get_indices (path);
    if (row != NULL && indices != NULL)
      *row = indices[0];
    gtk_tree_path_free (path);
  }

  if (col != NULL && focus != NULL) {
    GList *columns = gtk_tree_view_get_columns (GTK_TREE_VIEW (sheet->treeview));
    gint index = g_list_index (columns, focus);
    g_list_free (columns);
    *col = index > 0 ? index - 1 : -1;
  }
}

void
gattrib_sheet_select_column (GattribSheet *sheet, gint col)
{
  g_return_if_fail (GATTRIB_IS_SHEET (sheet));
  select_whole_column (sheet, col);
}

void
gattrib_sheet_unselect_all (GattribSheet *sheet)
{
  g_return_if_fail (GATTRIB_IS_SHEET (sheet));
  gtk_tree_selection_unselect_all (
      gtk_tree_view_get_selection (GTK_TREE_VIEW (sheet->treeview)));
}


