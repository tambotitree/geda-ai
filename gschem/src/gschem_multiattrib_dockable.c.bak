/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2020 gEDA Contributors (see ChangeLog for details)
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
#include <config.h>

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"
#include "../include/gschem_multiattrib_dockable.h"
#include <gdk/gdkkeysyms.h>

static void multiattrib_update(GschemMultiattribDockable *multiattrib);

static gboolean
snv_shows_name(int snv)
{
  return snv == SHOW_NAME_VALUE || snv == SHOW_NAME;
}

static gboolean
snv_shows_value(int snv)
{
  return snv == SHOW_NAME_VALUE || snv == SHOW_VALUE;
}

/*! \brief Update the multiattrib editor dialog for a GschemToplevel.
 *
 *  \par Function Description
 *
 *  If the GschemToplevel has an open multiattrib dialog, switch to
 *  watching the current page's SELECTION object for changes.
 *
 *  \param [in] w_current  The GschemToplevel object.
 */
void x_multiattrib_update(GschemToplevel *w_current)
{
  g_object_set(G_OBJECT(w_current->multiattrib_dockable), "object_list",
               w_current->toplevel->page_current->selection_list, NULL);
}

/*! \section celltextview-widget Cell TextView Widget Code.
 * This widget makes a 'GtkTextView' widget implements the 'GtkCellEditable'
 * interface. It can then be used to renderer multi-line texts inside
 * tree views ('GtkTreeView').
 */
static void celltextview_class_init(CellTextViewClass *klass);
static void celltextview_init(CellTextView *self);
static void celltextview_cell_editable_init(GtkCellEditableIface *iface);

enum
{
  PROP_EDIT_CANCELED = 1
};

static void
celltextview_set_property(GObject *object,
                          guint property_id,
                          const GValue *value,
                          GParamSpec *pspec)
{
  CellTextView *celltextview = (CellTextView *)object;

  switch (property_id)
  {
  case PROP_EDIT_CANCELED:
    celltextview->editing_canceled = g_value_get_boolean(value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void
celltextview_get_property(GObject *object,
                          guint property_id,
                          GValue *value,
                          GParamSpec *pspec)
{
  CellTextView *celltextview = (CellTextView *)object;

  switch (property_id)
  {
  case PROP_EDIT_CANCELED:
    g_value_set_boolean(value, celltextview->editing_canceled);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static gboolean
celltextview_key_press_event(GtkWidget *widget,
                             GdkEventKey *key_event,
                             gpointer data)
{
  CellTextView *celltextview = (CellTextView *)widget;

  /* If the Escape key is pressed, we flag the edit as canceled */
  if (key_event->keyval == GDK_KEY_Escape)
    celltextview->editing_canceled = TRUE;

  /* ends editing of cell if one of these keys are pressed or editing is canceled */
  if (celltextview->editing_canceled == TRUE ||
      /* the Enter key without the Control modifier */
      (!(key_event->state & GDK_CONTROL_MASK) &&
       (key_event->keyval == GDK_KEY_Return ||
        key_event->keyval == GDK_KEY_KP_Enter)))
  {
    gtk_cell_editable_editing_done(GTK_CELL_EDITABLE(celltextview));
    gtk_cell_editable_remove_widget(GTK_CELL_EDITABLE(celltextview));
    return TRUE;
  }

  return FALSE;
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
celltextview_start_editing(GtkCellEditable *cell_editable,
                           GdkEvent *event)
{
  g_signal_connect(cell_editable,
                   "key_press_event",
                   G_CALLBACK(celltextview_key_press_event),
                   NULL);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
GType celltextview_get_type()
{
  static GType celltextview_type = 0;

  if (!celltextview_type)
  {
    static const GTypeInfo celltextview_info = {
        sizeof(CellTextViewClass),
        NULL, /* base_init */
        NULL, /* base_finalize */
        (GClassInitFunc)celltextview_class_init,
        NULL, /* class_finalize */
        NULL, /* class_data */
        sizeof(CellTextView),
        0, /* n_preallocs */
        (GInstanceInitFunc)celltextview_init,
    };

    static const GInterfaceInfo cell_editable_info = {
        (GInterfaceInitFunc)celltextview_cell_editable_init,
        NULL, /* interface_finalize */
        NULL  /* interface_data */
    };

    celltextview_type = g_type_register_static(GTK_TYPE_TEXT_VIEW,
                                               "CellTextView",
                                               &celltextview_info, 0);
    g_type_add_interface_static(celltextview_type,
                                GTK_TYPE_CELL_EDITABLE,
                                &cell_editable_info);
  }

  return celltextview_type;
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
celltextview_class_init(CellTextViewClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  gobject_class->get_property = celltextview_get_property;
  gobject_class->set_property = celltextview_set_property;

  g_object_class_install_property(
      gobject_class,
      PROP_EDIT_CANCELED,
      g_param_spec_boolean("editing-canceled",
                           "",
                           "",
                           FALSE,
                           G_PARAM_READWRITE));
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
celltextview_init(CellTextView *celltextview)
{
  celltextview->editing_canceled = FALSE;
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
celltextview_cell_editable_init(GtkCellEditableIface *iface)
{
  iface->start_editing = celltextview_start_editing;
}

/*! \section multi-line-text-cell-renderer Multi-line Text Cell Renderer
 * GTK has no multi-line text cell renderer. This code adds one to be used
 * in gschem code. It is inspired by the 'GtkCellRendererCombo' renderer
 * of GTK 2.4 (LGPL).
 */
static void cellrenderermultilinetext_class_init(CellRendererMultiLineTextClass *klass);
static void cellrenderermultilinetext_editing_done(GtkCellEditable *cell_editable,
                                                   gpointer user_data);
static gboolean cellrenderermultilinetext_focus_out_event(GtkWidget *widget,
                                                          GdkEvent *event,
                                                          gpointer user_data);

#define CELL_RENDERER_MULTI_LINE_TEXT_PATH "cell-renderer-multi-line-text-path"

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static GtkCellEditable *
cellrenderermultilinetext_start_editing(GtkCellRenderer *cell,
                                        GdkEvent *event,
                                        GtkWidget *widget,
                                        const gchar *path,
                                        const GdkRectangle *background_area,
                                        const GdkRectangle *cell_area,
                                        GtkCellRendererState flags)
{
  GtkCellRendererText *cell_text;
  CellRendererMultiLineText *cell_mlt;
  GtkWidget *textview;
  GtkTextBuffer *textbuffer;

  gboolean editable = FALSE;
  gchar *text = NULL;

  cell_text = GTK_CELL_RENDERER_TEXT(cell);

  /* GTK 3: Must use g_object_get instead of direct struct access */
  g_object_get(cell_text,
               "editable", &editable,
               "text", &text,
               NULL);

  if (!editable) {
    g_free(text);  // just in case it was set
    return NULL;
  }

  cell_mlt = CELL_RENDERER_MULTI_LINE_TEXT(cell);

  textbuffer = GTK_TEXT_BUFFER(g_object_new(GTK_TYPE_TEXT_BUFFER, NULL));

  gtk_text_buffer_set_text(textbuffer, text, text ? strlen(text) : 0);
  g_free(text);  // always free what g_object_get allocated

  textview = GTK_WIDGET(g_object_new(TYPE_CELL_TEXT_VIEW,
                                     "buffer", textbuffer,
                                     "editable", TRUE,
                                     "height-request", cell_area->height,
                                     NULL));

  g_object_set_data_full(G_OBJECT(textview),
                         CELL_RENDERER_MULTI_LINE_TEXT_PATH,
                         g_strdup(path), g_free);

  gtk_widget_show(textview);

  g_signal_connect(GTK_CELL_EDITABLE(textview),
                   "editing_done",
                   G_CALLBACK(cellrenderermultilinetext_editing_done),
                   cell_mlt);

  cell_mlt->focus_out_id =
      g_signal_connect(textview,
                       "focus_out_event",
                       G_CALLBACK(cellrenderermultilinetext_focus_out_event),
                       cell_mlt);

  return GTK_CELL_EDITABLE(textview);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
cellrenderermultilinetext_editing_done(GtkCellEditable *cell_editable,
                                       gpointer user_data)
{
  CellRendererMultiLineText *cell = CELL_RENDERER_MULTI_LINE_TEXT(user_data);
  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  gchar *new_text;
  const gchar *path;

  if (cell->focus_out_id > 0)
  {
    g_signal_handler_disconnect(cell_editable,
                                cell->focus_out_id);
    cell->focus_out_id = 0;
  }

  if (CELL_TEXT_VIEW(cell_editable)->editing_canceled)
  {
    g_signal_emit_by_name(cell, "editing-canceled");
    return;
  }

  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(cell_editable));
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  new_text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);

  path = g_object_get_data(G_OBJECT(cell_editable),
                           CELL_RENDERER_MULTI_LINE_TEXT_PATH);
  g_signal_emit_by_name(cell, "edited", path, new_text);

  g_free(new_text);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static gboolean
cellrenderermultilinetext_focus_out_event(GtkWidget *widget,
                                          GdkEvent *event,
                                          gpointer user_data)
{
  //  cellrenderermultilinetext_editing_done (GTK_CELL_EDITABLE (widget),
  //                                          user_data);

  return FALSE;
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
GType cellrenderermultilinetext_get_type()
{
  static GType cellrenderermultilinetext_type = 0;

  if (!cellrenderermultilinetext_type)
  {
    static const GTypeInfo cellrenderermultilinetext_info = {
        sizeof(CellRendererMultiLineTextClass),
        NULL, /* base_init */
        NULL, /* base_finalize */
        (GClassInitFunc)cellrenderermultilinetext_class_init,
        NULL, /* class_finalize */
        NULL, /* class_data */
        sizeof(CellRendererMultiLineText),
        0,    /* n_preallocs */
        NULL, /* instance_init */
    };

    cellrenderermultilinetext_type = g_type_register_static(
        GTK_TYPE_CELL_RENDERER_TEXT,
        "CellRendererMultiLineText",
        &cellrenderermultilinetext_info, 0);
  }

  return cellrenderermultilinetext_type;
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
cellrenderermultilinetext_class_init(CellRendererMultiLineTextClass *klass)
{
  /*   GObjectClass *gobject_class = G_OBJECT_CLASS (klass); */
  GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS(klass);

  cell_class->start_editing = cellrenderermultilinetext_start_editing;
}

enum
{
  PROP_OBJECT_LIST = 1
};

enum
{
  COLUMN_INHERITED,
  COLUMN_NAME,
  COLUMN_VALUE,
  COLUMN_VISIBILITY,
  COLUMN_SHOW_NAME_VALUE,
  COLUMN_PRESENT_IN_ALL,
  COLUMN_IDENTICAL_VALUE,
  COLUMN_IDENTICAL_VISIBILITY,
  COLUMN_IDENTICAL_SHOW_NAME,
  COLUMN_IDENTICAL_SHOW_VALUE,
  COLUMN_ATTRIBUTE_GEDALIST,
  NUM_COLUMNS
};

static GObjectClass *multiattrib_parent_class = NULL;

static void multiattrib_class_init(GschemMultiattribDockableClass *class);
static GtkWidget *multiattrib_create_widget(GschemDockable *dockable);
static void multiattrib_set_property(GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec);
static void multiattrib_get_property(GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static void multiattrib_popup_menu(GschemMultiattribDockable *multiattrib,
                                   GdkEventButton *event);

/*!\brief Invoke the multi-attribute editor to edit a single attribute.
 */
void x_multiattrib_edit_attribute(GschemToplevel *w_current, OBJECT *object)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(w_current->multiattrib_dockable);
  GtkTreeIter iter;
  gboolean valid;
  GtkTreePath *path;

  /* present editor first to make sure the widget hierarchy exists */
  gschem_dockable_present(w_current->multiattrib_dockable);

  /* find tree iterator corresponding to the attribute */
  for (valid = gtk_tree_model_get_iter_first(multiattrib->store, &iter);
       valid;
       valid = gtk_tree_model_iter_next(multiattrib->store, &iter))
  {
    GedaList *attr_list;
    GList *a_iter;
    gtk_tree_model_get(multiattrib->store, &iter,
                       COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                       -1);
    for (a_iter = geda_list_get_glist(attr_list);
         a_iter != NULL; a_iter = a_iter->next)
      if ((OBJECT *)a_iter->data == object)
        /* found attribute in list */
        break;

    g_object_unref(attr_list);
    if (a_iter != NULL)
      break;
  }

  if (!valid)
  {
    /* can't find attribute--fall back to single-attribute editor */
    attrib_edit_dialog(w_current, object, FROM_MENU);
    return;
  }

  /* invoke the editor */
  path = gtk_tree_model_get_path(multiattrib->store, &iter);
  gtk_widget_grab_focus(GTK_WIDGET(multiattrib->treeview));
  gtk_tree_view_set_cursor(multiattrib->treeview, path,
                           multiattrib->column_value, TRUE);
  gtk_tree_path_free(path);
}

/*! \brief Returns TRUE/FALSE if the given object may have attributes attached.
 *
 *  \par Function Description
 *
 *  Returns TRUE/FALSE if the given object may have attributes attached.
 *
 *  \param [in] object  The OBJECT to test.
 *  \returns  TRUE/FALSE if the given object may have attributes attached.
 */
static gboolean is_multiattrib_object(OBJECT *object)
{
  if (object->type == OBJ_COMPLEX ||
      object->type == OBJ_PLACEHOLDER ||
      object->type == OBJ_NET ||
      object->type == OBJ_BUS ||
      object->type == OBJ_PIN)
  {
    return TRUE;
  }
  return FALSE;
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_action_add_attribute(GschemMultiattribDockable *multiattrib,
                                 const gchar *name,
                                 const gchar *value,
                                 gint visible,
                                 gint show_name_value)
{
  OBJECT *object;
  GtkWidget *parent_window;
  gchar *newtext;
  GList *iter;
  GschemToplevel *w_current = multiattrib->parent.w_current;

  switch (gschem_dockable_get_state(GSCHEM_DOCKABLE(multiattrib)))
  {
  case GSCHEM_DOCKABLE_STATE_DIALOG:
  case GSCHEM_DOCKABLE_STATE_WINDOW:
    parent_window = multiattrib->parent.window;
    break;
  default:
    parent_window = w_current->main_window;
  }

  newtext = g_strdup_printf("%s=%s", name, value);

  if (!x_dialog_validate_attribute(GTK_WINDOW(parent_window), newtext))
  {
    g_free(newtext);
    return;
  }

  for (iter = geda_list_get_glist(multiattrib->object_list);
       iter != NULL;
       iter = g_list_next(iter))
  {
    object = (OBJECT *)iter->data;

    if (is_multiattrib_object(object))
    {

      /* create a new attribute and link it */
      o_attrib_add_attrib(w_current, newtext,
                          visible, show_name_value, object);
    }
  }

  gschem_toplevel_page_content_changed(w_current, w_current->toplevel->page_current);
  o_undo_savestate_old(w_current, UNDO_ALL, _("Add Attribute"));

  g_free(newtext);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_action_duplicate_attributes(GschemMultiattribDockable *multiattrib,
                                        GList *attr_list)
{
  GschemToplevel *w_current = multiattrib->parent.w_current;
  GList *iter;

  for (iter = attr_list;
       iter != NULL;
       iter = g_list_next(iter))
  {
    OBJECT *o_attrib = (OBJECT *)iter->data;

    /* create a new attribute and link it */
    o_attrib_add_attrib(w_current,
                        o_text_get_string(w_current->toplevel, o_attrib),
                        o_is_visible(o_attrib),
                        o_attrib->show_name_value,
                        o_attrib->attached_to);
  }

  gschem_toplevel_page_content_changed(w_current, w_current->toplevel->page_current);
  o_undo_savestate_old(w_current, UNDO_ALL, _("Duplicate Attribute"));
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_action_promote_attributes(GschemMultiattribDockable *multiattrib,
                                      GList *attr_list)
{
  GschemToplevel *w_current = multiattrib->parent.w_current;
  TOPLEVEL *toplevel = w_current->toplevel;
  OBJECT *o_new;
  GList *iter;

  for (iter = attr_list;
       iter != NULL;
       iter = g_list_next(iter))
  {
    OBJECT *o_attrib = (OBJECT *)iter->data;

    if (o_is_visible(o_attrib))
    {
      /* If the attribute we're promoting is visible, don't clone its location */
      o_attrib_add_attrib(w_current,
                          o_text_get_string(w_current->toplevel, o_attrib),
                          VISIBLE,
                          o_attrib->show_name_value,
                          o_attrib->parent);
    }
    else
    {
      /* make a copy of the attribute object */
      o_new = o_object_copy(toplevel, o_attrib);
      o_set_visibility(toplevel, o_new, VISIBLE);
      s_page_append(toplevel, toplevel->page_current, o_new);
      /* add the attribute its parent */
      o_attrib_attach(toplevel, o_new, o_attrib->parent, TRUE);
      /* note: this object is unselected (not added to selection). */

      /* Call add-objects-hook */
      g_run_hook_object(w_current, "%add-objects-hook", o_new);
    }
  }

  gschem_toplevel_page_content_changed(w_current, w_current->toplevel->page_current);
  o_undo_savestate_old(w_current, UNDO_ALL, _("Promote Attribute"));
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_action_delete_attributes(GschemMultiattribDockable *multiattrib,
                                     GList *attr_list)
{
  GschemToplevel *w_current = multiattrib->parent.w_current;
  GList *a_iter;
  OBJECT *o_attrib;

  for (a_iter = attr_list; a_iter != NULL; a_iter = g_list_next(a_iter))
  {
    o_attrib = a_iter->data;
    /* actually deletes the attribute */
    o_delete(w_current, o_attrib);
  }

  gschem_toplevel_page_content_changed(w_current, w_current->toplevel->page_current);
  o_undo_savestate_old(w_current, UNDO_ALL, _("Delete Attribute"));
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_action_copy_attribute_to_all(GschemMultiattribDockable *multiattrib,
                                         GList *attr_list)
{
  GschemToplevel *w_current = multiattrib->parent.w_current;
  GList *iter;
  GList *objects_needing_add;

  objects_needing_add = g_list_copy(geda_list_get_glist(multiattrib->object_list));

  /* Remove objects which already have this attribute from the list */
  for (iter = attr_list;
       iter != NULL;
       iter = g_list_next(iter))
  {
    OBJECT *o_attrib = (OBJECT *)iter->data;

    objects_needing_add = g_list_remove(objects_needing_add, o_attrib->attached_to);
  }

  for (iter = objects_needing_add; iter != NULL; iter = g_list_next(iter))
  {
    OBJECT *object = iter->data;

    if (is_multiattrib_object(object))
    {

      /* Pick the first instance to copy from */
      OBJECT *attrib_to_copy = attr_list->data;

      int visibility = o_is_visible(attrib_to_copy) ? VISIBLE : INVISIBLE;

      /* create a new attribute and link it */
      o_attrib_add_attrib(w_current,
                          o_text_get_string(w_current->toplevel, attrib_to_copy),
                          visibility,
                          attrib_to_copy->show_name_value,
                          object);
    }
  }

  gschem_toplevel_page_content_changed(w_current, w_current->toplevel->page_current);
  o_undo_savestate_old(w_current, UNDO_ALL, _("Copy Attribute to All"));
}

/**
 * multiattrib_column_set_data_name:
 * @tree_column: The GtkTreeViewColumn containing this cell.
 * @cell:        The GtkCellRenderer being updated.
 * @tree_model:  The GtkTreeModel supplying row data.
 * @iter:        The iterator pointing to the current row.
 * @data:        Pointer to the GschemMultiattribDockable (dialog context).
 *
 * Sets the display properties for the 'name' column cell in the multi-attribute
 * dockable dialog. This sets the text, color, and editability of the attribute
 * name cell depending on whether the attribute is inherited and/or present in all
 * selected objects.
 *
 * GTK 3 COMPLIANCE:
 * - Uses "foreground-rgba" (not "foreground-gdk") for text color (requires GdkRGBA).
 */
static void
multiattrib_column_set_data_name(GtkTreeViewColumn *tree_column,
                                 GtkCellRenderer *cell,
                                 GtkTreeModel *tree_model,
                                 GtkTreeIter *iter,
                                 gpointer data)
{
  GschemMultiattribDockable *dialog = GSCHEM_MULTIATTRIB_DOCKABLE(data);
  gchar *name;
  gboolean present_in_all;
  int inherited;

  gtk_tree_model_get(tree_model, iter,
                     COLUMN_INHERITED, &inherited,
                     COLUMN_NAME, &name,
                     COLUMN_PRESENT_IN_ALL, &present_in_all,
                     -1);

  g_object_set(cell,
               "text", name,
               "foreground-rgba", inherited ? &dialog->insensitive_text_color :
                                 (!present_in_all ? &dialog->not_present_in_all_text_color : NULL),
               "editable", !inherited,
               NULL);
  g_free(name);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_column_set_data_value(GtkTreeViewColumn *tree_column,
                                  GtkCellRenderer *cell,
                                  GtkTreeModel *tree_model,
                                  GtkTreeIter *iter,
                                  gpointer data)
{
  GschemMultiattribDockable *dialog = GSCHEM_MULTIATTRIB_DOCKABLE(data);
  gchar *value;
  gboolean identical_value;
  int inherited;

  gtk_tree_model_get(tree_model, iter,
                     COLUMN_INHERITED, &inherited,
                     COLUMN_VALUE, &value,
                     COLUMN_IDENTICAL_VALUE, &identical_value,
                     -1);

  g_object_set(cell,
               "text", identical_value ? value : _("<various>"),
               "foreground-gdk", inherited ? &dialog->insensitive_text_color : (!identical_value ? &dialog->not_identical_value_text_color : NULL),
               "editable", !inherited,
               NULL);
  g_free(value);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_column_set_data_visible(GtkTreeViewColumn *tree_column,
                                    GtkCellRenderer *cell,
                                    GtkTreeModel *tree_model,
                                    GtkTreeIter *iter,
                                    gpointer data)
{
  gboolean visibility;
  gboolean identical_visibility;
  int inherited;

  gtk_tree_model_get(tree_model, iter,
                     COLUMN_INHERITED, &inherited,
                     COLUMN_VISIBILITY, &visibility,
                     COLUMN_IDENTICAL_VISIBILITY, &identical_visibility,
                     -1);

  g_object_set(cell,
               "active", visibility,
               "sensitive", !inherited,
               "activatable", !inherited,
               "inconsistent", !identical_visibility,
               NULL);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_column_set_data_show_name(GtkTreeViewColumn *tree_column,
                                      GtkCellRenderer *cell,
                                      GtkTreeModel *tree_model,
                                      GtkTreeIter *iter,
                                      gpointer data)
{
  int show_name_value;
  gboolean identical_show_name;
  int inherited;

  gtk_tree_model_get(tree_model, iter,
                     COLUMN_INHERITED, &inherited,
                     COLUMN_SHOW_NAME_VALUE, &show_name_value,
                     COLUMN_IDENTICAL_SHOW_NAME, &identical_show_name,
                     -1);

  g_object_set(cell,
               "active", snv_shows_name(show_name_value),
               "sensitive", !inherited,
               "activatable", !inherited,
               "inconsistent", !identical_show_name,
               NULL);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_column_set_data_show_value(GtkTreeViewColumn *tree_column,
                                       GtkCellRenderer *cell,
                                       GtkTreeModel *tree_model,
                                       GtkTreeIter *iter,
                                       gpointer data)
{
  int show_name_value;
  gboolean identical_show_value;
  int inherited;

  gtk_tree_model_get(tree_model, iter,
                     COLUMN_INHERITED, &inherited,
                     COLUMN_SHOW_NAME_VALUE, &show_name_value,
                     COLUMN_IDENTICAL_SHOW_VALUE, &identical_show_value,
                     -1);

  g_object_set(cell,
               "active", snv_shows_value(show_name_value),
               "sensitive", !inherited,
               "activatable", !inherited,
               "inconsistent", !identical_show_value,
               NULL);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_callback_edited_name(GtkCellRendererText *cellrenderertext,
                                 gchar *arg1,
                                 gchar *new_name,
                                 gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  GtkTreeIter iter;
  GedaList *attr_list;
  GList *a_iter;
  OBJECT *o_attrib;
  GschemToplevel *w_current;
  GtkWidget *parent_window;
  gchar *value, *newtext;
  int visibility;

  w_current = multiattrib->parent.w_current;

  if (!gtk_tree_model_get_iter_from_string(multiattrib->store, &iter, arg1))
  {
    return;
  }

  switch (gschem_dockable_get_state(GSCHEM_DOCKABLE(multiattrib)))
  {
  case GSCHEM_DOCKABLE_STATE_DIALOG:
  case GSCHEM_DOCKABLE_STATE_WINDOW:
    parent_window = multiattrib->parent.window;
    break;
  default:
    parent_window = w_current->main_window;
  }

  if (g_ascii_strcasecmp(new_name, "") == 0)
  {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(parent_window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        _("Attributes with empty name are not allowed. Please set a name."));

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return;
  }

  gtk_tree_model_get(multiattrib->store, &iter,
                     COLUMN_VALUE, &value,
                     COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                     -1);

  newtext = g_strdup_printf("%s=%s", new_name, value);

  if (!x_dialog_validate_attribute(GTK_WINDOW(parent_window), newtext))
  {
    g_free(value);
    g_free(newtext);
    return;
  }

  for (a_iter = geda_list_get_glist(attr_list);
       a_iter != NULL;
       a_iter = g_list_next(a_iter))
  {
    o_attrib = a_iter->data;

    visibility = o_is_visible(o_attrib) ? VISIBLE : INVISIBLE;

    /* actually modifies the attribute */
    o_text_change(w_current, o_attrib,
                  newtext, visibility, o_attrib->show_name_value);
  }

  g_object_unref(attr_list);
  g_free(value);
  g_free(newtext);

  gschem_toplevel_page_content_changed(w_current, w_current->toplevel->page_current);
  o_undo_savestate_old(w_current, UNDO_ALL, _("Edit Attribute Name"));

  /* NB: We don't fix up the model to reflect the edit, we're about to nuke it below... */

  /* Refresh the whole model.. some attribute names may consolidate into one row */
  multiattrib_update(multiattrib);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_callback_edited_value(GtkCellRendererText *cell_renderer,
                                  gchar *arg1,
                                  gchar *new_value,
                                  gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  GtkTreeIter iter;
  GedaList *attr_list;
  GList *a_iter;
  OBJECT *o_attrib;
  GschemToplevel *w_current;
  char *name;
  char *old_value;
  GtkWidget *parent_window;
  char *newtext;
  int visibility;

  w_current = multiattrib->parent.w_current;

  if (!gtk_tree_model_get_iter_from_string(multiattrib->store, &iter, arg1))
  {
    return;
  }

  gtk_tree_model_get(multiattrib->store, &iter,
                     COLUMN_NAME, &name,
                     COLUMN_VALUE, &old_value,
                     COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                     -1);

  /* If the edit didn't change anything, don't adjust any attributes */
  if (strcmp(old_value, new_value) == 0)
    return;

  switch (gschem_dockable_get_state(GSCHEM_DOCKABLE(multiattrib)))
  {
  case GSCHEM_DOCKABLE_STATE_DIALOG:
  case GSCHEM_DOCKABLE_STATE_WINDOW:
    parent_window = multiattrib->parent.window;
    break;
  default:
    parent_window = w_current->main_window;
  }

  newtext = g_strdup_printf("%s=%s", name, new_value);

  if (!x_dialog_validate_attribute(GTK_WINDOW(parent_window), newtext))
  {
    g_free(name);
    g_free(newtext);
    return;
  }

  for (a_iter = geda_list_get_glist(attr_list);
       a_iter != NULL;
       a_iter = g_list_next(a_iter))
  {
    o_attrib = (OBJECT *)a_iter->data;

    visibility = o_is_visible(o_attrib) ? VISIBLE : INVISIBLE;

    /* actually modifies the attribute */
    o_text_change(w_current, o_attrib,
                  newtext, visibility, o_attrib->show_name_value);
  }

  g_object_unref(attr_list);

  g_free(name);
  g_free(newtext);

  gschem_toplevel_page_content_changed(w_current, w_current->toplevel->page_current);
  o_undo_savestate_old(w_current, UNDO_ALL, _("Edit Attribute"));

  /* Fixup the model to reflect the edit */
  gtk_list_store_set(GTK_LIST_STORE(multiattrib->store), &iter,
                     COLUMN_VALUE, new_value,
                     COLUMN_IDENTICAL_VALUE, TRUE,
                     -1);
}

/**
 * multiattrib_callback_toggled_visible:
 * @cell_renderer: The GtkCellRendererToggle that was toggled.
 * @path:          The path string for the row that was toggled.
 * @user_data:     Pointer to the GschemMultiattribDockable (editor state).
 *
 * Callback invoked when the "visible" toggle in the multiattrib dockable's
 * tree view is clicked. Updates the visibility state of all attributes
 * represented in the row, updates the schematic and marks the undo state.
 *
 * GTK 3 COMPLIANCE: Uses GtkCellRendererToggle, GtkTreeModel, and
 * GtkListStore in a manner fully compatible with GTK 3.0–3.24+.
 * No deprecated API usage.
 */
static void
multiattrib_callback_toggled_visible(GtkCellRendererToggle *cell_renderer,
                                    gchar *path,
                                    gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  GtkTreeIter iter;
  OBJECT *o_attrib;
  GschemToplevel *w_current;
  gboolean new_visibility;
  GedaList *attr_list;
  GList *a_iter;

  w_current = multiattrib->parent.w_current;

  if (!gtk_tree_model_get_iter_from_string(multiattrib->store, &iter, path))
  {
    return;
  }

  gtk_tree_model_get(multiattrib->store, &iter,
                     COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                     -1);

  /* Toggle logic: the new state is the opposite of the current state */
  new_visibility = !gtk_cell_renderer_toggle_get_active(cell_renderer);

  for (a_iter = geda_list_get_glist(attr_list);
       a_iter != NULL;
       a_iter = g_list_next(a_iter))
  {
    o_attrib = (OBJECT *)a_iter->data;

    /* Actually modifies the attribute's visibility */
    o_invalidate(w_current, o_attrib);
    o_set_visibility(w_current->toplevel, o_attrib, new_visibility ? VISIBLE : INVISIBLE);
    o_text_recreate(w_current->toplevel, o_attrib);
  }

  g_object_unref(attr_list);

  gschem_toplevel_page_content_changed(w_current, w_current->toplevel->page_current);
  o_undo_savestate_old(w_current, UNDO_ALL, _("Toggle Attribute Visibility"));

  /* Update the model to reflect the new state */
  gtk_list_store_set(GTK_LIST_STORE(multiattrib->store), &iter,
                     COLUMN_VISIBILITY, new_visibility,
                     COLUMN_IDENTICAL_VISIBILITY, TRUE,
                     -1);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_callback_toggled_show_name(GtkCellRendererToggle *cell_renderer,
                                       gchar *path,
                                       gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  GtkTreeIter iter;
  GschemToplevel *w_current;
  gboolean new_name_visible;
  GedaList *attr_list;
  GList *a_iter;
  gint new_snv;

  w_current = multiattrib->parent.w_current;

  if (!gtk_tree_model_get_iter_from_string(multiattrib->store, &iter, path))
  {
    return;
  }

  gtk_tree_model_get(multiattrib->store, &iter,
                     COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                     -1);

  new_name_visible = !gtk_cell_renderer_toggle_get_active(cell_renderer);

  for (a_iter = geda_list_get_glist(attr_list);
       a_iter != NULL;
       a_iter = g_list_next(a_iter))
  {
    OBJECT *o_attrib = (OBJECT *)a_iter->data;

    gboolean value_visible = snv_shows_value(o_attrib->show_name_value);

    /* If we switch off the name visibility, but the value was not previously visible, make it so now */
    if (new_name_visible)
      new_snv = value_visible ? SHOW_NAME_VALUE : SHOW_NAME;
    else
      new_snv = SHOW_VALUE;

    o_invalidate(w_current, o_attrib);

    /* actually modifies the attribute */
    o_attrib->show_name_value = new_snv;
    o_text_recreate(w_current->toplevel, o_attrib);
  }

  g_object_unref(attr_list);

  gschem_toplevel_page_content_changed(w_current, w_current->toplevel->page_current);
  o_undo_savestate_old(w_current, UNDO_ALL, _("Toggle Show Attribute Name"));

  /* NB: We don't fix up the model to reflect the edit, we're about to nuke it below... */

  /* request an update of display for this row */
  /* Recompute the whole model as the consistency for the show value column may be affected above */
  multiattrib_update(multiattrib);
}

/*! \todo Finish function documentation
 *  \brief
 *  \par Function Description
 *
 */
static void
multiattrib_callback_toggled_show_value(GtkCellRendererToggle *cell_renderer,
                                        gchar *path,
                                        gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  GtkTreeIter iter;
  GschemToplevel *w_current;
  gboolean new_value_visible;
  GedaList *attr_list;
  GList *a_iter;
  gint new_snv;

  w_current = multiattrib->parent.w_current;

  if (!gtk_tree_model_get_iter_from_string(multiattrib->store, &iter, path))
  {
    return;
  }

  gtk_tree_model_get(multiattrib->store, &iter,
                     COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                     -1);

  new_value_visible = !gtk_cell_renderer_toggle_get_active(cell_renderer);

  for (a_iter = geda_list_get_glist(attr_list);
       a_iter != NULL;
       a_iter = g_list_next(a_iter))
  {
    OBJECT *o_attrib = (OBJECT *)a_iter->data;

    gboolean name_visible = snv_shows_name(o_attrib->show_name_value);

    /* If we switch off the name visibility, but the value was not previously visible, make it so now */
    if (new_value_visible)
      new_snv = name_visible ? SHOW_NAME_VALUE : SHOW_VALUE;
    else
      new_snv = SHOW_NAME;

    o_invalidate(w_current, o_attrib);

    /* actually modifies the attribute */
    o_attrib->show_name_value = new_snv;
    o_text_recreate(w_current->toplevel, o_attrib);
  }

  g_object_unref(attr_list);

  gschem_toplevel_page_content_changed(w_current, w_current->toplevel->page_current);
  o_undo_savestate_old(w_current, UNDO_ALL, _("Toggle Show Attribute Value"));

  /* NB: We don't fix up the model to reflect the edit, we're about to nuke it below... */

  /* request an update of display for this row */
  /* Recompute the whole model as the consistency for the show name column may be affected above */
  multiattrib_update(multiattrib);
}

/**
 * multiattrib_callback_key_pressed:
 * @widget:    The GtkWidget (treeview) receiving the key event.
 * @event:     The GdkEventKey describing the key press.
 * @user_data: Pointer to the GschemMultiattribDockable instance.
 *
 * Handles key press events on the attribute editor's treeview.
 * If "Delete" (without modifiers) is pressed and the current selection is
 * not inherited, deletes the selected attribute(s) by calling
 * multiattrib_action_delete_attributes().
 *
 * Returns: TRUE if an action was taken and the event is handled, FALSE otherwise.
 *
 * GTK 3.x COMPLIANT: Uses only standard GDK/GTK 3 event and tree model APIs.
 */
static gboolean
multiattrib_callback_key_pressed(GtkWidget *widget,
                                 GdkEventKey *event,
                                 gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);

  if ((event->state & gtk_accelerator_get_default_mod_mask()) == 0 &&
      (event->keyval == GDK_KEY_Delete || event->keyval == GDK_KEY_KP_Delete))
  {
    GtkTreeModel *model;
    GtkTreeIter iter;
    GedaList *attr_list;
    int inherited;
    /* delete the currently selected attribute */

    if (!gtk_tree_selection_get_selected(
            gtk_tree_view_get_selection(multiattrib->treeview),
            &model, &iter))
    {
      /* nothing selected, nothing to do */
      return FALSE;
    }

    gtk_tree_model_get(model, &iter,
                       COLUMN_INHERITED, &inherited,
                       COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                       -1);

    /* We can't delete inherited attribtes */
    if (inherited)
      return FALSE;

    multiattrib_action_delete_attributes(multiattrib,
                                         geda_list_get_glist(attr_list));

    g_object_unref(attr_list);

    /* update the treeview contents */
    multiattrib_update(multiattrib);
  }

  return FALSE;
}

/**
 * find_row:
 * @tree_model:    The model to search.
 * @iter_return:   Output location for the iterator if a matching row is found.
 * @name:          The attribute name to match.
 * @inherited:     The inherited flag to match.
 *
 * Searches for the first row in @tree_model where both the attribute name and
 * the inherited flag match the provided values. If found, sets @iter_return
 * to the iterator for that row.
 *
 * Returns: TRUE if a matching row is found, FALSE otherwise.
 *
 * GTK 3.x COMPLIANT: All APIs are unchanged and valid through 3.26+.
 */
static gboolean
find_row(GtkTreeModel *tree_model, GtkTreeIter *iter_return,
         gchar *name, gboolean inherited)
{
  GtkTreeIter iter;
  gboolean valid;

  for (valid = gtk_tree_model_get_iter_first(tree_model, &iter);
       valid;
       valid = gtk_tree_model_iter_next(tree_model, &iter))
  {
    gchar *iter_name;
    gboolean iter_inherited;
    gboolean matches;
    gtk_tree_model_get(tree_model, &iter,
                       COLUMN_NAME, &iter_name,
                       COLUMN_INHERITED, &iter_inherited,
                       -1);
    matches = iter_inherited == inherited && strcmp(iter_name, name) == 0;
    g_free(iter_name);

    if (matches)
    {
      *iter_return = iter;
      return TRUE;
    }
  }

  return FALSE;
}

/*! \brief Move edit focus to the cell pointed to by a mouse event.
 *  \par Function Description
 *  Uses the X and Y coordinates of a mouse event, to move edit focus
 *  to the cell at those coords.
 *
 *  If the cell represents the value of an inherited attribute, edits
 *  the value of the attached attribute with that name.  If there's no
 *  attached attribute with that name, promotes the attribute first.
 *
 * NB: The coordinates must be relative to the tree view's bin window, IE.. have
 *     come from en event where event->window == gtk_tree_view_get_bin_window ().
 *
 *  \param [in] multiattrib  The GschemMultiattribDockable object.
 *  \param [in] x            The x coordinate of the mouse event.
 *  \param [in] y            The y coordinate of the mouse event.
 */
static void
multiattrib_edit_cell_at_pos(GschemMultiattribDockable *multiattrib,
                             gint x, gint y)
{
  GtkTreePath *path;
  GtkTreeViewColumn *column;
  GtkTreeIter iter;
  gboolean inherited;
  gchar *name;
  GedaList *attr_list;

  if (!gtk_tree_view_get_path_at_pos(multiattrib->treeview,
                                     x, y, &path, &column, NULL, NULL))
    return;
  if (!gtk_tree_model_get_iter(multiattrib->store, &iter, path))
  {
    gtk_tree_path_free(path);
    return;
  }

  gtk_tree_model_get(multiattrib->store, &iter,
                     COLUMN_INHERITED, &inherited,
                     -1);
  if (!inherited)
  {
    /* row is editable--just edit it */
    gtk_tree_view_set_cursor_on_cell(multiattrib->treeview,
                                     path, column, NULL, TRUE);
    gtk_tree_path_free(path);
    return;
  }
  gtk_tree_path_free(path);

  /* don't promote attributes when trying to edit columns other than "value" */
  if (column != multiattrib->column_value)
    return;

  /* see if there's already a matching attached attribute */
  gtk_tree_model_get(multiattrib->store, &iter,
                     COLUMN_NAME, &name,
                     -1);
  if (!find_row(multiattrib->store, &iter, name, FALSE))
  {
    /* promote attribute */
    gtk_tree_model_get(multiattrib->store, &iter,
                       COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                       -1);
    multiattrib_action_promote_attributes(multiattrib,
                                          geda_list_get_glist(attr_list));
    g_object_unref(attr_list);
    multiattrib_update(multiattrib);

    /* find tree iterator corresponding to the promoted attribute */
    if (!find_row(multiattrib->store, &iter, name, FALSE))
    {
      g_free(name);
      return;
    }
  }
  g_free(name);

  /* get new path and invoke editor */
  path = gtk_tree_model_get_path(multiattrib->store, &iter);
  gtk_tree_view_set_cursor_on_cell(multiattrib->treeview,
                                   path, column, NULL, TRUE);
  gtk_tree_path_free(path);
}
/*
* multiattrib_callback_button_pressed:
* @widget:    The treeview widget.
* @event:     The GdkEventButton describing the mouse event.
* @user_data: The GschemMultiattribDockable pointer.
*
* Handles mouse button presses on the treeview. Pops up the context menu
* on right-click. On double left-click, moves edit focus to the clicked cell,
* allowing for more intuitive editing behavior compared to the GTK default.
*
* Returns: TRUE if the event was handled, FALSE otherwise.
*
* GTK 3.x COMPLIANT: All GDK and GTK APIs here are stable in 3.10–3.26+.
*/
static gboolean
multiattrib_callback_button_pressed(GtkWidget *widget,
                                    GdkEventButton *event,
                                    gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  gboolean ret = FALSE;

  /* popup menu on right click */
  if (event->type == GDK_BUTTON_PRESS && event->button == 3)
  {
    multiattrib_popup_menu(multiattrib, event);
    ret = TRUE;
  }

  /* edit cell on double (left) click */
  /* (Normally, edit focus by click is handled for us, but this function is useful
   * for overriding the default behavior of treating a double-click the same as a
   * single-click, with edit focus needing two consecutive double or single clicks
   * with a pause in between.  This can be unintuitive and time-wasting) */
  else if (event->type == GDK_2BUTTON_PRESS && event->button == 1)
  {
    multiattrib_edit_cell_at_pos(multiattrib, event->x, event->y);
    ret = TRUE;
  }

  return ret;
}

/**
 * multiattrib_callback_query_tooltip:
 * @widget: The GtkTreeView widget.
 * @x: X coordinate (event or keyboard navigation).
 * @y: Y coordinate (event or keyboard navigation).
 * @keyboard_mode: TRUE if activated by keyboard, else by mouse.
 * @tooltip: The GtkTooltip to set.
 * @user_data: The GschemMultiattribDockable pointer.
 *
 * Handles dynamic tooltips for the multiattrib treeview.
 * - If hovering over a column header, provides context-sensitive descriptions for visibility/name/value columns.
 * - If hovering over a value cell, shows the full value as a tooltip.
 *
 * Returns: TRUE if a tooltip was set, FALSE to use default tooltip handling.
 *
 * GTK 3.x COMPLIANT: Uses GtkTreeView and GtkTooltip APIs valid in 3.10–3.26+.
 */
static gboolean
multiattrib_callback_query_tooltip(GtkWidget *widget,
                                   gint x, gint y, gboolean keyboard_mode,
                                   GtkTooltip *tooltip, gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);

  GtkTreeView *tree_view = GTK_TREE_VIEW(widget);
  GtkTreePath *path = NULL;
  GtkTreeViewColumn *column = NULL;

  if (keyboard_mode)
    gtk_tree_view_get_cursor(tree_view, &path, &column);
  else
  {
    gtk_tree_view_convert_widget_to_bin_window_coords(tree_view, x, y,
                                                      &x, &y);
    if (y < 0 ? !gtk_tree_view_get_path_at_pos(tree_view, x, 0,
                                               NULL, &column, NULL, NULL)
              : !gtk_tree_view_get_path_at_pos(tree_view, x, y,
                                               &path, &column, NULL, NULL))
      return FALSE;
  }

  if (path == NULL)
  {
    /* show tooltip for column header */

    if (column == multiattrib->column_visible)
      gtk_tooltip_set_markup(tooltip, _("Is the attribute visible?"));
    else if (column == multiattrib->column_show_name)
      gtk_tooltip_set_markup(tooltip, _("Show attribute name?"));
    else if (column == multiattrib->column_show_value)
      gtk_tooltip_set_markup(tooltip, _("Show attribute value?"));
    else
      return FALSE;
  }
  else
  {
    /* show tooltip for cell */

    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *value;

    if (column != multiattrib->column_value)
      return FALSE;

    model = gtk_tree_view_get_model(tree_view);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, COLUMN_VALUE, &value, -1);

    gtk_tooltip_set_markup(tooltip, value);
    gtk_tree_view_set_tooltip_row(tree_view, tooltip, path);

    g_free(value);
    gtk_tree_path_free(path);
  }

  return TRUE;
}

/**
 * multiattrib_callback_popup_menu:
 * @widget:    The GtkTreeView widget.
 * @user_data: The GschemMultiattribDockable pointer.
 *
 * Handles the "popup-menu" event (triggered by keyboard or mouse on the treeview).
 * Invokes the attribute popup menu for the current selection.
 *
 * Returns: TRUE to indicate the menu was shown and the event handled.
 *
 * GTK 3.x COMPLIANT: Uses stable signal and menu invocation.
 */
static gboolean
multiattrib_callback_popup_menu(GtkWidget *widget,
                                gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);

  multiattrib_popup_menu(multiattrib, NULL);

  return TRUE;
}

/**
 * multiattrib_callback_popup_duplicate:
 * @menuitem:  The menu item triggering the duplicate action.
 * @user_data: The GschemMultiattribDockable pointer.
 *
 * Duplicates the selected attribute(s) by calling multiattrib_action_duplicate_attributes().
 * If no attribute is selected, does nothing.
 * Refreshes the treeview contents after duplicating.
 *
 * GTK 3.x COMPLIANT: Tree model, menu, and signal logic are unchanged in 3.10–3.26+.
 */
static void
multiattrib_callback_popup_duplicate(GtkMenuItem *menuitem,
                                     gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  GtkTreeModel *model;
  GtkTreeIter iter;
  GedaList *attr_list;

  if (!gtk_tree_selection_get_selected(
          gtk_tree_view_get_selection(multiattrib->treeview),
          &model, &iter))
  {
    /* nothing selected, nothing to do */
    return;
  }

  gtk_tree_model_get(model, &iter,
                     COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                     -1);
  multiattrib_action_duplicate_attributes(multiattrib, geda_list_get_glist(attr_list));
  g_object_unref(attr_list);

  /* update the treeview contents */
  multiattrib_update(multiattrib);
}

/**
 * multiattrib_callback_popup_promote:
 * @menuitem:   The menu item triggering the promote action.
 * @user_data:  The GschemMultiattribDockable pointer.
 *
 * Promotes (copies) the selected attribute(s) from inherited to directly attached.
 * If no attribute is selected, does nothing.
 * After promoting, refreshes the treeview contents.
 *
 * GTK 3.x COMPLIANT: Uses GtkTreeView, GtkTreeSelection, and GObject APIs valid in 3.10–3.26+.
 */
static void
multiattrib_callback_popup_promote(GtkMenuItem *menuitem,
                                   gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  GtkTreeModel *model;
  GtkTreeIter iter;
  GedaList *attr_list;

  if (!gtk_tree_selection_get_selected(
          gtk_tree_view_get_selection(multiattrib->treeview),
          &model, &iter))
  {
    /* nothing selected, nothing to do */
    return;
  }

  gtk_tree_model_get(model, &iter,
                     COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                     -1);
  multiattrib_action_promote_attributes(multiattrib, geda_list_get_glist(attr_list));
  g_object_unref(attr_list);

  /* update the treeview contents */
  multiattrib_update(multiattrib);
}

/**
 * multiattrib_callback_popup_delete:
 * @menuitem:   The menu item triggering the delete action.
 * @user_data:  The GschemMultiattribDockable pointer.
 *
 * Deletes the selected attribute(s).
 * If no attribute is selected, does nothing.
 * After deletion, refreshes the treeview contents.
 *
 * GTK 3.x COMPLIANT: All APIs are stable through 3.26+.
 */
static void
multiattrib_callback_popup_delete(GtkMenuItem *menuitem,
                                  gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  GtkTreeModel *model;
  GtkTreeIter iter;
  GedaList *attr_list;

  if (!gtk_tree_selection_get_selected(
          gtk_tree_view_get_selection(multiattrib->treeview),
          &model, &iter))
  {
    /* nothing selected, nothing to do */
    return;
  }

  gtk_tree_model_get(model, &iter,
                     COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                     -1);
  multiattrib_action_delete_attributes(multiattrib, geda_list_get_glist(attr_list));
  g_object_unref(attr_list);

  /* update the treeview contents */
  multiattrib_update(multiattrib);
}

/**
 * multiattrib_callback_popup_copy_to_all:
 * @menuitem:   The menu item triggering the copy-to-all action.
 * @user_data:  The GschemMultiattribDockable pointer.
 *
 * Copies the selected attribute to all compatible objects that do not already have it.
 * If no attribute is selected, does nothing.
 * After copying, refreshes the treeview contents.
 *
 * GTK 3.x COMPLIANT: Uses only standard, supported API.
 */
static void
multiattrib_callback_popup_copy_to_all(GtkMenuItem *menuitem,
                                       gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  GtkTreeModel *model;
  GtkTreeIter iter;
  GedaList *attr_list;

  if (!gtk_tree_selection_get_selected(
          gtk_tree_view_get_selection(multiattrib->treeview),
          &model, &iter))
  {
    /* nothing selected, nothing to do */
    return;
  }

  gtk_tree_model_get(model, &iter,
                     COLUMN_ATTRIBUTE_GEDALIST, &attr_list,
                     -1);
  multiattrib_action_copy_attribute_to_all(multiattrib, geda_list_get_glist(attr_list));
  g_object_unref(attr_list);

  /* update the treeview contents */
  multiattrib_update(multiattrib);
}

/**
 * multiattrib_callback_value_key_pressed:
 * @widget:      The GtkTextView being edited.
 * @event:       The key event.
 * @user_data:   The GschemMultiattribDockable pointer.
 *
 * Handles Return, Tab, and Shift-Tab for ending cell editing in the value field.
 * If Ctrl is held, inserts a literal character (newline or tab).
 * Otherwise, moves focus appropriately and stops further event propagation.
 *
 * GTK 3.x COMPLIANT: No deprecated calls used.
 */
static gboolean
multiattrib_callback_value_key_pressed(GtkWidget *widget,
                                       GdkEventKey *event,
                                       gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  gboolean retval = FALSE;

  /* ends editing of cell if one of these keys are pressed: */
  /*  - the Return key without the Control modifier */
  /*  - the Tab key without the Control modifier */
  if ((event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) ||
      (event->keyval == GDK_KEY_Tab || event->keyval == GDK_KEY_KP_Tab ||
       event->keyval == GDK_KEY_ISO_Left_Tab))
  {
    /* Control modifier activated? */
    if (event->state & GDK_CONTROL_MASK)
    {
      /* yes the modifier in event structure and let event propagate */
      event->state ^= GDK_CONTROL_MASK;
      retval = FALSE;
    }
    else
    {
      /* change focus and stop propagation */
      g_signal_emit_by_name(GSCHEM_DOCKABLE(multiattrib)->widget,
                            "move_focus",
                            (event->state & GDK_SHIFT_MASK) ? GTK_DIR_TAB_BACKWARD : GTK_DIR_TAB_FORWARD);
      retval = TRUE;
    }
  }

  return retval;
}

/**
 * multiattrib_callback_value_grab_focus:
 * @widget:      The GtkTextView widget.
 * @user_data:   Not used.
 *
 * Selects all text in the GtkTextView so it can be quickly replaced by typing.
 *
 * GTK 3.x COMPLIANT (but `gtk_text_buffer_get_iter_at_offset` with -1 is *technically* not best practice; use `gtk_text_buffer_get_end_iter` instead).
 */
static void
multiattrib_callback_value_grab_focus(GtkWidget *widget, gpointer user_data)
{
    GtkTextView *textview = GTK_TEXT_VIEW(widget);
    GtkTextBuffer *textbuffer;
    GtkTextIter startiter, enditer;

    textbuffer = gtk_text_view_get_buffer(textview);
    gtk_text_buffer_get_start_iter(textbuffer, &startiter);
    gtk_text_buffer_get_end_iter(textbuffer, &enditer);
    gtk_text_buffer_select_range(textbuffer, &startiter, &enditer);
}

/*!
 * \brief Callback for the "Add" button in the multiattrib dock.
 *
 * \details
 * This function retrieves user input from the "Add Attribute" frame and
 * attempts to add a new attribute to the object list managed by the
 * GschemMultiattribDockable.
 *
 * The following UI components are read:
 *  - The attribute name from the GtkComboBoxText (entry text)
 *  - The attribute value from the GtkTextView
 *  - Visibility checkbox state
 *  - Visibility type (name/value display mode) from GtkComboBoxText
 *
 * After validation, the attribute is added via `multiattrib_action_add_attribute()`,
 * followed by an update of the UI through `multiattrib_update()`.
 *
 * \param button     The GTK button that was clicked (unused).
 * \param user_data  Pointer to the GschemMultiattribDockable structure.
 */
static void
multiattrib_callback_button_add(GtkButton *button, gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);

  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  const gchar *name;
  gchar *value;
  gboolean visible;
  gint shownv;

  // Get value text from GtkTextView
  buffer = gtk_text_view_get_buffer(multiattrib->textview_value);
  gtk_text_buffer_get_bounds(buffer, &start, &end);
  value = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

  // Get attribute name from GtkComboBoxText entry
  name = gtk_combo_box_text_get_active_text(multiattrib->combo_name);
  if (!name || name[0] == '\0' || name[0] == ' ')
  {
    g_free(value);
    return; // invalid name
  }

  // Get visibility checkbox state
  visible = gtk_toggle_button_get_active(
      (GtkToggleButton *)multiattrib->button_visible);

  // Get selected visibility type (0=Name+Value, 1=Value, 2=Name)
  shownv = gtk_combo_box_get_active(
      GTK_COMBO_BOX(multiattrib->combobox_visible_types));

  // Add the new attribute
  multiattrib_action_add_attribute(multiattrib, name, value, visible, shownv);

  // Clean up and refresh
  g_free(value);
  multiattrib_update(multiattrib);
}

/**
 * Initialize the attribute name combo box with available attribute names.
 * @combobox: GtkComboBoxText widget (with entry enabled)
 *
 * This function populates the combo box with all attribute names returned by
 * s_attrib_get(i), as well as enabling free-text entry.
 *
 * GTK 3.x: Uses GtkComboBoxText, replacing old GtkCombo.
 */
static void
multiattrib_init_attrib_names(GtkComboBoxText *combobox)
{
    const gchar *string;
    gint i;

    for (i = 0, string = s_attrib_get(i);
         string != NULL;
         i++, string = s_attrib_get(i))
    {
        gtk_combo_box_text_append_text(combobox, string);
    }
    // Default: no pre-selection, allow user to type or pick
}

/**
 * \brief Initialize the visibility type combo box with options.
 *
 * This function sets up the options for displaying attributes in the
 * multiattrib dockable (e.g., "Show Name & Value", "Show Value only", etc.).
 *
 * \param combobox A GtkComboBoxText widget where the options will be inserted.
 */
static void
multiattrib_init_visible_types(GtkComboBoxText *combobox)
{
  gtk_combo_box_text_append_text(combobox, _("Show Name & Value"));
  gtk_combo_box_text_append_text(combobox, _("Show Value only"));
  gtk_combo_box_text_append_text(combobox, _("Show Name only"));

  gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), 0); // Default to first option
}

/*! \brief Popup a context-sensitive menu.
 *  \par Function Description
 *  Pops up a context-sensitive menu.
 *  <B>event</B> can be NULL if the popup is triggered by a key binding
 *  instead of a mouse click.
 *
 *  \param [in] multiattrib  The GschemMultiattribDockable object.
 *  \param [in] event        Mouse event.
 */
static void
multiattrib_popup_menu(GschemMultiattribDockable *multiattrib,
                       GdkEventButton *event)
{
  GtkTreePath *path;
  GtkWidget *menu;
  struct menuitem_t
  {
    gchar *label;
    GCallback callback;
  };

  struct menuitem_t menuitems_inherited[] = {
      {N_("Promote"), G_CALLBACK(multiattrib_callback_popup_promote)},
      {NULL, NULL}};

  struct menuitem_t menuitems_noninherited[] = {
      {N_("Duplicate"), G_CALLBACK(multiattrib_callback_popup_duplicate)},
      {N_("Delete"), G_CALLBACK(multiattrib_callback_popup_delete)},
      {N_("Copy to all"), G_CALLBACK(multiattrib_callback_popup_copy_to_all)},
      {NULL, NULL}};

  struct menuitem_t *item_list;
  struct menuitem_t *tmp;
  int inherited;
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;

  selection = gtk_tree_view_get_selection(multiattrib->treeview);

  if (event != NULL &&
      gtk_tree_view_get_path_at_pos(multiattrib->treeview,
                                    (gint)event->x,
                                    (gint)event->y,
                                    &path, NULL, NULL, NULL))
  {
    gtk_tree_selection_unselect_all(selection);
    gtk_tree_selection_select_path(selection, path);
    gtk_tree_path_free(path);
  }

  /* if nothing is selected, nothing to do */
  if (!gtk_tree_selection_get_selected(selection, &model, &iter))
    return;

  gtk_tree_model_get(model, &iter,
                     COLUMN_INHERITED, &inherited,
                     -1);

  item_list = inherited ? menuitems_inherited : menuitems_noninherited;

  /* create the context menu */
  menu = gtk_menu_new();
  for (tmp = item_list; tmp->label != NULL; tmp++)
  {
    GtkWidget *menuitem;
    if (strcmp(tmp->label, "-") == 0)
    {
      menuitem = gtk_separator_menu_item_new();
    }
    else
    {
      menuitem = gtk_menu_item_new_with_label(_(tmp->label));
      g_signal_connect(menuitem,
                       "activate",
                       tmp->callback,
                       multiattrib);
    }
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
  }
  gtk_widget_show_all(menu);
  /* make menu a popup menu */
  gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
                 (event != NULL) ? event->button : 0,
                 gdk_event_get_time((GdkEvent *)event));
}

/*! \brief GschemDockable "save_internal_geometry" class method handler
 *
 *  \par Function Description
 *  Save the dockable's current internal geometry.
 *
 *  \param [in] dockable   The GschemDockable to save the geometry of.
 *  \param [in] key_file   The GKeyFile to save the geometry data to.
 *  \param [in] group_name The group name in the key file to store the data under.
 */
static void
multiattrib_save_internal_geometry(GschemDockable *dockable,
                                   EdaConfig *cfg,
                                   gchar *group_name)
{
  gboolean show_inherited;

  show_inherited =
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(GSCHEM_MULTIATTRIB_DOCKABLE(dockable)->show_inherited));
  eda_config_set_boolean(cfg, group_name, "show_inherited", show_inherited);
}

/*! \brief GschemDockable "restore_internal_geometry" class method handler
 *
 *  \par Function Description
 *  Restore the dockable's current internal geometry.
 *
 *  \param [in] dockable   The GschemDockable to restore the geometry of.
 *  \param [in] key_file   The GKeyFile to save the geometry data to.
 *  \param [in] group_name The group name in the key file to store the data under.
 */
static void
multiattrib_restore_internal_geometry(GschemDockable *dockable,
                                      EdaConfig *cfg,
                                      gchar *group_name)
{
  gboolean show_inherited;
  GError *error = NULL;

  show_inherited = eda_config_get_boolean(cfg, group_name, "show_inherited", &error);
  if (error != NULL)
  {
    show_inherited = TRUE;
    g_error_free(error);
  }
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(GSCHEM_MULTIATTRIB_DOCKABLE(dockable)->show_inherited), show_inherited);
}

/*! \brief Function to retrieve GschemMultiattribDockable's GType identifier.
 *
 *  \par Function Description
 *
 *  Function to retrieve GschemMultiattribDockable's GType identifier.
 *  Upon first call, this registers GschemMultiattribDockable in the GType system.
 *  Subsequently it returns the saved value from its first execution.
 *
 *  \return the GType identifier associated with GschemMultiattribDockable.
 */
GType gschem_multiattrib_dockable_get_type()
{
  static GType multiattrib_type = 0;

  if (!multiattrib_type)
  {
    static const GTypeInfo multiattrib_info = {
        sizeof(GschemMultiattribDockableClass),
        NULL, /* base_init */
        NULL, /* base_finalize */
        (GClassInitFunc)multiattrib_class_init,
        NULL, /* class_finalize */
        NULL, /* class_data */
        sizeof(GschemMultiattribDockable),
        0,    /* n_preallocs */
        NULL, /* multiattrib_init */
    };

    multiattrib_type = g_type_register_static(GSCHEM_TYPE_DOCKABLE,
                                              "GschemMultiattribDockable",
                                              &multiattrib_info, 0);
  }

  return multiattrib_type;
}

/*! \brief Update the multiattrib editor dialog when its object list changes.
 *
 *  \par Function Description
 *
 *  \param [in] selection    The GedaList object of we are watching/
 *  \param [in] multiattrib  The multi-attribute editor dialog.
 */
static void
object_list_changed_cb(GedaList *object_list,
                       GschemMultiattribDockable *multiattrib)
{
  multiattrib_update(multiattrib);
}

/*! \brief Update the dialog when the current object GedaList object is destroyed
 *
 *  \par Function Description
 *
 *  This handler is called when the g_object_weak_ref() on the GedaList object
 *  we're watching expires. We reset our multiattrib->object_list pointer to NULL
 *  to avoid attempting to access the destroyed object. NB: Our signal handlers
 *  were automatically disconnected during the destruction process.
 *
 *  \param [in] data                  Pointer to the multi-attrib dialog
 *  \param [in] where_the_object_was  Pointer to where the object was just destroyed
 */
static void
object_list_weak_ref_cb(gpointer data, GObject *where_the_object_was)
{
  GschemMultiattribDockable *multiattrib = GSCHEM_MULTIATTRIB_DOCKABLE(data);

  multiattrib->object_list = NULL;
  multiattrib_update(multiattrib);
}

/*! \brief Connect signal handler and weak_ref on the GedaList object
 *
 *  \par Function Description
 *
 *  Connect the "changed" signal and add a weak reference
 *  on the GedaList object we are going to watch.
 *
 *  \param [in] multiattrib  The GschemMultiattribDockable dialog.
 *  \param [in] object_list  The GedaList object to watch.
 */
static void
connect_object_list(GschemMultiattribDockable *multiattrib,
                    GedaList *object_list)
{
  multiattrib->object_list = object_list;
  if (multiattrib->object_list != NULL)
  {
    g_object_weak_ref(G_OBJECT(multiattrib->object_list),
                      object_list_weak_ref_cb,
                      multiattrib);
    multiattrib->object_list_changed_id =
        g_signal_connect(G_OBJECT(multiattrib->object_list),
                         "changed",
                         G_CALLBACK(object_list_changed_cb),
                         multiattrib);
    /* Synthesise a object_list changed update to refresh the view */
    object_list_changed_cb(multiattrib->object_list, multiattrib);
  }
  else
  {
    /* Call an update to set the sensitivities */
    multiattrib_update(multiattrib);
  }
}

/*! \brief Disconnect signal handler and weak_ref on the GedaList object
 *
 *  \par Function Description
 *
 *  If the dialog is watching a GedaList object, disconnect the
 *  "changed" signal and remove our weak reference on the object.
 *
 *  \param [in] multiattrib  The GschemMultiattribDockable dialog.
 */
static void
disconnect_object_list(GschemMultiattribDockable *multiattrib)
{
  if (multiattrib->object_list != NULL)
  {
    g_signal_handler_disconnect(multiattrib->object_list,
                                multiattrib->object_list_changed_id);
    g_object_weak_unref(G_OBJECT(multiattrib->object_list),
                        object_list_weak_ref_cb,
                        multiattrib);
  }
}

/*! \brief GObject finalise handler
 *
 *  \par Function Description
 *
 *  Just before the GschemMultiattribDockable GObject is finalized, disconnect from
 *  the GedaList object being watched and then chain up to the parent
 *  class's finalize handler.
 *
 *  \param [in] object  The GObject being finalized.
 */
static void
multiattrib_finalize(GObject *object)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(object);

  disconnect_object_list(multiattrib);
  G_OBJECT_CLASS(multiattrib_parent_class)->finalize(object);
}

/*! \brief GType class initialiser for GschemMultiattribDockable
 *
 *  \par Function Description
 *
 *  GType class initialiser for GschemMultiattribDockable. We override our parent
 *  virtual class methods as needed and register our GObject properties.
 *
 *  \param [in]  klass       The GschemMultiattribDockableClass we are initialising
 */
static void
multiattrib_class_init(GschemMultiattribDockableClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GschemDockableClass *gschem_dockable_class = GSCHEM_DOCKABLE_CLASS(klass);

  gschem_dockable_class->create_widget = multiattrib_create_widget;

  gschem_dockable_class->save_internal_geometry =
      multiattrib_save_internal_geometry;
  gschem_dockable_class->restore_internal_geometry =
      multiattrib_restore_internal_geometry;

  gobject_class->set_property = multiattrib_set_property;
  gobject_class->get_property = multiattrib_get_property;
  gobject_class->finalize = multiattrib_finalize;

  multiattrib_parent_class = g_type_class_peek_parent(klass);

  g_object_class_install_property(
      gobject_class, PROP_OBJECT_LIST,
      g_param_spec_pointer("object_list",
                           "",
                           "",
                           G_PARAM_READWRITE));
}

/*! \brief Regenerate the attribute list when the visibility
 *         setting for inherited attributes changes
 */
static void
multiattrib_show_inherited_toggled(GtkToggleButton *button,
                                   gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);

  /* update the treeview contents */
  multiattrib_update(multiattrib);
}

/*! \brief Determine the index of \a name in the list of attributes.
 *
 * \returns the index, or \c -1 if the attribute name wasn't found
 */
static gint
lookup_attrib_index(const gchar *name)
{
  const char *ref;
  if (name == NULL)
    return -1;
  for (int i = 0; (ref = s_attrib_get(i)) != NULL; i++)
    if (strcmp(name, ref) == 0)
      return i;
  return -1;
}

/*! \brief Determine the order in which two rows should be sorted.
 *
 * Takes the order of attributes set with \c attribute-name into
 * account and sorts all attached attributes before all inherited
 * attributes.
 *
 * \returns \c -1 , \c 0 or \c 1 depending on whether \a iter0 sorts
 *          before, with or after \a iter1
 */
static gint
attribute_sort_func(GtkTreeModel *model,
                    GtkTreeIter *iter0, GtkTreeIter *iter1,
                    gpointer user_data)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(user_data);
  gboolean inherited0, inherited1;
  gchar *name0, *name1;
  gint result;

  gtk_tree_model_get(multiattrib->store, iter0,
                     COLUMN_INHERITED, &inherited0,
                     COLUMN_NAME, &name0,
                     -1);
  gtk_tree_model_get(multiattrib->store, iter1,
                     COLUMN_INHERITED, &inherited1,
                     COLUMN_NAME, &name1,
                     -1);

  if (inherited1 && !inherited0)
    result = -1;
  else if (inherited0 && !inherited1)
    result = 1;
  else if (name0 == NULL)
  {
    if (name1 == NULL)
      result = 0;
    else
      result = -1;
  }
  else if (name1 == NULL)
    result = 1;
  else
  {
    gint index0 = lookup_attrib_index(name0),
         index1 = lookup_attrib_index(name1);

    if (index0 != -1 && (index1 == -1 || index1 > index0))
      result = -1;
    else if (index1 != -1 && (index0 == -1 || index0 > index1))
      result = 1;
    else
      result = strcmp(name0, name1);
  }

  g_free(name0);
  g_free(name1);
  return result;
}

/*! \brief Create widgets for GschemMultiattribDockable
 *
 *  \par Function Description
 *
 *  Create and setup the widgets which make up the dockable.
 *
 *  \param [in] dockable The GschemMultiattribDockable we are initialising
 */
static GtkWidget *
multiattrib_create_widget(GschemDockable *dockable)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(dockable);
  GtkWidget *vbox;
  GtkWidget *label, *scrolled_win, *treeview;
  GtkWidget *table, *textview, *combo, *optionm, *button;
  GtkWidget *attrib_vbox, *show_inherited;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection *selection;
  GtkStyle *style;
  GtkStyleContext *context;

  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, DIALOG_V_SPACING);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), DIALOG_BORDER_SPACING);

  /* create the attribute list frame */
  multiattrib->list_frame = GTK_WIDGET(g_object_new(GTK_TYPE_FRAME,
                                                    /* GtkFrame */
                                                    "shadow", GTK_SHADOW_NONE,
                                                    NULL));
  /*   - create the model for the treeview */
  multiattrib->store = GTK_TREE_MODEL(
      gtk_list_store_new(NUM_COLUMNS,
                         G_TYPE_BOOLEAN,  /* COLUMN_INHERITED */
                         G_TYPE_STRING,   /* COLUMN_NAME */
                         G_TYPE_STRING,   /* COLUMN_VALUE */
                         G_TYPE_BOOLEAN,  /* COLUMN_VISIBILITY */
                         G_TYPE_INT,      /* COLUMN_SHOW_NAME_VALUE */
                         G_TYPE_BOOLEAN,  /* COLUMN_PRESENT_IN_ALL */
                         G_TYPE_BOOLEAN,  /* COLUMN_IDENTICAL_VALUE */
                         G_TYPE_BOOLEAN,  /* COLUMN_IDENTICAL_VISIBILITY */
                         G_TYPE_BOOLEAN,  /* COLUMN_IDENTICAL_SHOW_NAME */
                         G_TYPE_BOOLEAN,  /* COLUMN_IDENTICAL_SHOW_VALUE */
                         G_TYPE_OBJECT)); /* COLUMN_ATTRIBUTE_GEDALIST */
  gtk_tree_sortable_set_default_sort_func(
      GTK_TREE_SORTABLE(multiattrib->store),
      attribute_sort_func, multiattrib, NULL);
  gtk_tree_sortable_set_sort_column_id(
      GTK_TREE_SORTABLE(multiattrib->store),
      GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, GTK_SORT_ASCENDING);

  /*   - create a scrolled window for the treeview */
  scrolled_win = GTK_WIDGET(
      g_object_new(GTK_TYPE_SCROLLED_WINDOW,
                   /* GtkContainer */
                   "border-width", 0,
                   /* GtkScrolledWindow */
                   "hscrollbar-policy",
                   GTK_POLICY_AUTOMATIC,
                   "vscrollbar-policy",
                   GTK_POLICY_AUTOMATIC,
                   "shadow-type",
                   GTK_SHADOW_ETCHED_IN,
                   NULL));
  /*   - create the treeview */
  treeview = GTK_WIDGET(g_object_new(GTK_TYPE_TREE_VIEW,
                                     /* GtkTreeView */
                                     "model", multiattrib->store,
                                     "rules-hint", TRUE,
                                     NULL));
  g_signal_connect(treeview,
                   "key-press-event",
                   G_CALLBACK(multiattrib_callback_key_pressed),
                   multiattrib);
  g_signal_connect(treeview,
                   "button-press-event",
                   G_CALLBACK(multiattrib_callback_button_pressed),
                   multiattrib);
  g_signal_connect(treeview,
                   "popup-menu",
                   G_CALLBACK(multiattrib_callback_popup_menu),
                   multiattrib);
  g_signal_connect(treeview,
                   "query-tooltip",
                   G_CALLBACK(multiattrib_callback_query_tooltip),
                   multiattrib);
  gtk_widget_set_has_tooltip(GTK_WIDGET(treeview), TRUE);
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gtk_tree_selection_set_mode(selection,
                              GTK_SELECTION_SINGLE);

  /*   - and now the columns of the treeview */
  /*       - column 1: visibility */
  renderer = GTK_CELL_RENDERER(
      g_object_new(GTK_TYPE_CELL_RENDERER_TOGGLE,
                   NULL));
  g_signal_connect(renderer,
                   "toggled",
                   G_CALLBACK(multiattrib_callback_toggled_visible),
                   multiattrib);
  multiattrib->column_visible =
      column = GTK_TREE_VIEW_COLUMN(
          g_object_new(GTK_TYPE_TREE_VIEW_COLUMN,
                       /* GtkTreeViewColumn */
                       "title", _("Vis"),
                       NULL));
  gtk_tree_view_column_pack_start(column, renderer, TRUE);
  gtk_tree_view_column_set_cell_data_func(column, renderer,
                                          multiattrib_column_set_data_visible,
                                          multiattrib, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
  /*       - column 2: show name */
  renderer = GTK_CELL_RENDERER(
      g_object_new(GTK_TYPE_CELL_RENDERER_TOGGLE,
                   NULL));
  g_signal_connect(renderer,
                   "toggled",
                   G_CALLBACK(multiattrib_callback_toggled_show_name),
                   multiattrib);
  multiattrib->column_show_name =
      column = GTK_TREE_VIEW_COLUMN(
          g_object_new(GTK_TYPE_TREE_VIEW_COLUMN,
                       /* GtkTreeViewColumn */
                       "title", _("N"),
                       NULL));
  gtk_tree_view_column_pack_start(column, renderer, TRUE);
  gtk_tree_view_column_set_cell_data_func(column, renderer,
                                          multiattrib_column_set_data_show_name,
                                          NULL, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
  /*       - column 3: show value */
  renderer = GTK_CELL_RENDERER(
      g_object_new(GTK_TYPE_CELL_RENDERER_TOGGLE,
                   NULL));
  g_signal_connect(renderer,
                   "toggled",
                   G_CALLBACK(multiattrib_callback_toggled_show_value),
                   multiattrib);
  multiattrib->column_show_value =
      column = GTK_TREE_VIEW_COLUMN(
          g_object_new(GTK_TYPE_TREE_VIEW_COLUMN,
                       /* GtkTreeViewColumn */
                       "title", _("V"),
                       NULL));
  gtk_tree_view_column_pack_start(column, renderer, TRUE);
  gtk_tree_view_column_set_cell_data_func(column, renderer,
                                          multiattrib_column_set_data_show_value,
                                          NULL, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
  /*       - column 4: attribute name */
  renderer = GTK_CELL_RENDERER(
    g_object_new(GTK_TYPE_CELL_RENDERER_TEXT,
                 "ellipsize", PANGO_ELLIPSIZE_END,
                 NULL));
  g_signal_connect(renderer,
                   "edited",
                   G_CALLBACK(multiattrib_callback_edited_name),
                   multiattrib);
  multiattrib->column_name =
      column = GTK_TREE_VIEW_COLUMN(
          g_object_new(GTK_TYPE_TREE_VIEW_COLUMN,
                       /* GtkTreeViewColumn */
                       "title", _("Name"),
                       "min-width", 100,
                       "resizable", TRUE,
                       NULL));
  gtk_tree_view_column_pack_start(column, renderer, TRUE);
  gtk_tree_view_column_set_cell_data_func(column, renderer,
                                          multiattrib_column_set_data_name,
                                          multiattrib, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
  /*       - column 5: attribute value */
  renderer = GTK_CELL_RENDERER(
    g_object_new(TYPE_CELL_RENDERER_MULTI_LINE_TEXT,
                 "ellipsize", PANGO_ELLIPSIZE_END,
                 NULL));
  g_signal_connect(renderer,
                   "edited",
                   G_CALLBACK(multiattrib_callback_edited_value),
                   multiattrib);
  multiattrib->column_value =
      column = GTK_TREE_VIEW_COLUMN(
          g_object_new(GTK_TYPE_TREE_VIEW_COLUMN,
                       /* GtkTreeViewColumn */
                       "title", _("Value"),
                       "min-width", 140,
                       "resizable", TRUE,
                       NULL));
  gtk_tree_view_column_pack_start(column, renderer, TRUE);
  gtk_tree_view_column_set_cell_data_func(column, renderer,
                                          multiattrib_column_set_data_value,
                                          multiattrib, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

  /* add the treeview to the scrolled window */
  gtk_container_add(GTK_CONTAINER(scrolled_win), treeview);
  /* set treeview of multiattrib */
  multiattrib->treeview = GTK_TREE_VIEW(treeview);

  attrib_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);

  /* Pack the vbox into the frame */
  gtk_container_add(GTK_CONTAINER(multiattrib->list_frame), attrib_vbox);

  multiattrib->status_label = gtk_label_new(NULL);
  // New (GTK 3)
  gtk_label_set_xalign(GTK_LABEL(multiattrib->status_label), 0.0);
  // Y alignment for single-line label is handled by vbox packing/margins
  gtk_widget_set_margin_start(multiattrib->status_label, 3);
  gtk_widget_set_margin_end(multiattrib->status_label, 0);
  gtk_widget_set_margin_top(multiattrib->status_label, 1);
  gtk_widget_set_margin_bottom(multiattrib->status_label, 0);
  gtk_label_set_ellipsize(GTK_LABEL(multiattrib->status_label),
                          PANGO_ELLIPSIZE_END);
  // depracated in GTK 3.26 and ellipsize is already being used, comment out and hope!
  // gtk_label_set_single_line_mode(GTK_LABEL(multiattrib->status_label), TRUE);
  /*
    gtk_box_pack_start (GTK_BOX (attrib_vbox),
                        multiattrib->status_label, FALSE, FALSE, 0);
    gtk_widget_show (multiattrib->status_label);
  */

  /* add the scrolled window to box */
  gtk_box_pack_start(GTK_BOX(attrib_vbox), scrolled_win, TRUE, TRUE, 0);

  /* create the show inherited button */
  show_inherited = gtk_check_button_new_with_label(_("Show inherited attributes"));
  multiattrib->show_inherited = show_inherited;
  gtk_box_pack_start(GTK_BOX(attrib_vbox), show_inherited, FALSE, FALSE, 0);

  g_signal_connect(show_inherited,
                   "toggled",
                   G_CALLBACK(multiattrib_show_inherited_toggled),
                   multiattrib);

  /* pack the frame */
  gtk_box_pack_start(GTK_BOX(vbox),
                     multiattrib->list_frame,
                     TRUE, TRUE, 0);
  gtk_widget_show_all(multiattrib->list_frame);

  /* create the add/edit frame */
  /* create the add/edit frame */
  multiattrib->add_frame = gtk_frame_new(_("Add Attribute"));

  table = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(table), 4);
  gtk_grid_set_column_spacing(GTK_GRID(table), 6);

  /* - the name entry: a GtkComboBoxText with entry enabled (replaces GtkCombo) */
  label = gtk_label_new(_("Name:"));
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  combo = gtk_combo_box_text_new_with_entry();
  multiattrib_init_attrib_names(GTK_COMBO_BOX_TEXT(combo));
  multiattrib->combo_name = GTK_COMBO_BOX_TEXT(combo);

  /* Attach label and combo to grid (row 0) */
  gtk_grid_attach(GTK_GRID(table), label, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(table), combo, 1, 0, 1, 1);
  gtk_widget_set_hexpand(combo, TRUE);
  gtk_widget_set_size_request(combo, 0, -1);

  /* - the value entry: a GtkTextView inside a scrolled window (row 1) */
  GtkWidget *value_label = gtk_label_new(_("Value:"));
  gtk_label_set_xalign(GTK_LABEL(value_label), 0.0);

  scrolled_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_win),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);

  textview = gtk_text_view_new();
  gtk_widget_set_tooltip_text(textview,
                              _("Ctrl+Enter inserts new line; Ctrl+Tab inserts Tab"));
  g_signal_connect(textview,
                   "key_press_event",
                   G_CALLBACK(multiattrib_callback_value_key_pressed),
                   multiattrib);
  g_signal_connect(textview,
                   "grab-focus",
                   G_CALLBACK(multiattrib_callback_value_grab_focus),
                   multiattrib);
  /* Save the GTK_STATE_NORMAL color for later workaround */
  context = gtk_widget_get_style_context(textview);
  gtk_style_context_get_color(context, GTK_STATE_FLAG_NORMAL, &multiattrib->value_normal_text_color);

  /* Save this one for inherited attributes (treeview is previously created) */
  context = gtk_widget_get_style_context(GTK_WIDGET(multiattrib->treeview));
  gtk_style_context_get_color(context, GTK_STATE_FLAG_INSENSITIVE, &multiattrib->insensitive_text_color);

  // Old
  // gdk_color_parse("grey", &multiattrib->not_identical_value_text_color);
  // gdk_color_parse("red", &multiattrib->not_present_in_all_text_color);

  // New (GTK 3.26+):
  multiattrib->not_identical_value_text_color.red   = 0.5;
  multiattrib->not_identical_value_text_color.green = 0.5;
  multiattrib->not_identical_value_text_color.blue  = 0.5;
  multiattrib->not_identical_value_text_color.alpha = 1.0;

  multiattrib->not_present_in_all_text_color.red   = 1.0;
  multiattrib->not_present_in_all_text_color.green = 0.0;
  multiattrib->not_present_in_all_text_color.blue  = 0.0;
  multiattrib->not_present_in_all_text_color.alpha = 1.0;

  gtk_container_add(GTK_CONTAINER(scrolled_win), textview);
  multiattrib->textview_value = GTK_TEXT_VIEW(textview);

  /* Attach value label and scrolled window to grid (row 1) */
  gtk_grid_attach(GTK_GRID(table), value_label, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(table), scrolled_win, 1, 1, 1, 1);
  gtk_widget_set_hexpand(scrolled_win, TRUE);

  /* - the visible status checkbox (row 2, left column) */
  GtkWidget *vis_button = gtk_check_button_new_with_label(_("Visible"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(vis_button), TRUE);
  multiattrib->button_visible = GTK_CHECK_BUTTON(vis_button);
  gtk_grid_attach(GTK_GRID(table), vis_button, 0, 2, 1, 1);

  /* - the add button (row 0-2, rightmost column) */
  GtkWidget *add_button = gtk_button_new_with_label(_("Add"));
  g_signal_connect(add_button,
                   "clicked",
                   G_CALLBACK(multiattrib_callback_button_add),
                   multiattrib);
  /* If you want the add button to span rows, use height=3 */
  gtk_grid_attach(GTK_GRID(table), add_button, 2, 0, 1, 3);

  /* add the grid to the frame */
  gtk_container_add(GTK_CONTAINER(multiattrib->add_frame), table);
  /* pack the frame in the dialog */
  gtk_box_pack_start(GTK_BOX(vbox), multiattrib->add_frame, FALSE, TRUE, 0);
  gtk_widget_show_all(multiattrib->add_frame);

  gtk_widget_show(vbox);

  multiattrib_update(multiattrib);

  return vbox;
}

/*! \brief GObject property setter function
 *
 *  \par Function Description
 *  Setter function for GschemMultiattribDockable's GObject property, "object_list".
 *
 *  \param [in]  object       The GObject whose properties we are setting
 *  \param [in]  property_id  The numeric id. under which the property was
 *                            registered with g_object_class_install_property()
 *  \param [in]  value        The GValue the property is being set from
 *  \param [in]  pspec        A GParamSpec describing the property being set
 */

static void
multiattrib_set_property(GObject *object,
                         guint property_id,
                         const GValue *value,
                         GParamSpec *pspec)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(object);

  switch (property_id)
  {
  case PROP_OBJECT_LIST:
    disconnect_object_list(multiattrib);
    connect_object_list(multiattrib, g_value_get_pointer(value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

/*! \brief GObject property getter function
 *
 *  \par Function Description
 *  Getter function for GschemMultiattribDockable's GObject property, "object_list".
 *
 *  \param [in]  object       The GObject whose properties we are getting
 *  \param [in]  property_id  The numeric id. under which the property was
 *                            registered with g_object_class_install_property()
 *  \param [out] value        The GValue in which to return the value of the property
 *  \param [in]  pspec        A GParamSpec describing the property being got
 */
static void
multiattrib_get_property(GObject *object,
                         guint property_id,
                         GValue *value,
                         GParamSpec *pspec)
{
  GschemMultiattribDockable *multiattrib =
      GSCHEM_MULTIATTRIB_DOCKABLE(object);

  switch (property_id)
  {
  case PROP_OBJECT_LIST:
    g_value_set_pointer(value, (gpointer)multiattrib->object_list);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

typedef struct
{
  gboolean inherited;
  char *name;
  int nth_with_name;
  char *value;
  gboolean visibility;
  int show_name_value;

  gboolean present_in_all;
  gboolean identical_value;
  gboolean identical_visibility;
  gboolean identical_show_name;
  gboolean identical_show_value;

  GedaList *attribute_gedalist;
} MODEL_ROW;

/*! \brief For a given OBJECT, produce a GList of MODEL_ROW records
 *
 *  \par Function Description
 *
 *  The main purpose of this function is to provide the "nth_with_name"
 *  count which we need to merge the attribute lists of various objects
 *  together.
 *
 *  \param [in] multiattrib  The multi-attribute editor dialog (For libgeda API which needs a TOPLEVEL)
 *  \param [in] object       The OBJECT * whos attributes we are processing
 *  \returns  A GList of MODEL_ROW records detailing object's attributes.
 */
static GList *
object_attributes_to_model_rows(GschemMultiattribDockable *multiattrib,
                                OBJECT *object)
{
  GList *model_rows = NULL;
  GList *a_iter;
  GList *object_attribs = o_attrib_return_attribs(object);

  for (a_iter = object_attribs; a_iter != NULL;
       a_iter = g_list_next(a_iter))
  {

    OBJECT *a_current = a_iter->data;
    MODEL_ROW *m_row = g_new0(MODEL_ROW, 1);
    GList *m_iter;

    m_row->inherited = o_attrib_is_inherited(a_current);
    o_attrib_get_name_value(a_current, &m_row->name, &m_row->value);
    m_row->visibility = o_is_visible(a_current);
    m_row->show_name_value = a_current->show_name_value;
    m_row->nth_with_name = 0; /* Provisional value until we check below */

    /* The following fields are always true for a single OBJECT */
    m_row->present_in_all = TRUE;
    m_row->identical_value = TRUE;
    m_row->identical_visibility = TRUE;
    m_row->identical_show_name = TRUE;
    m_row->identical_show_value = TRUE;

    m_row->attribute_gedalist = geda_list_new();
    geda_list_add(m_row->attribute_gedalist, a_current);

    /* Search already processed attributes to see if we need to bump m_row->nth_with_name */
    for (m_iter = model_rows;
         m_iter != NULL;
         m_iter = g_list_next(m_iter))
    {
      MODEL_ROW *m_compare = m_iter->data;
      if (strcmp(m_compare->name, m_row->name) == 0 &&
          m_compare->inherited == m_row->inherited)
      {
        m_row->nth_with_name = m_row->nth_with_name + 1;
      }
    }

    model_rows = g_list_append(model_rows, m_row);
  }

  g_list_free(object_attribs);

  return model_rows;
}

/*! \brief Produce a GList of MODEL_ROW records for all attribute objects in our GedaList
 *
 *  \par Function Description
 *
 *  This function produces a GList of MODEL_ROWs to the user can edit unattached
 *  attributes, or attributes which are selected separately from their owning
 *  object.
 *
 *  It is not expected this will be called when the GedaList the dialog is watching
 *  contains any higher level objects on which we could edit attributes.
 *
 *  \param [in] multiattrib  The multi-attribute editor dialog
 *  \returns  A GList of MODEL_ROW records detailing all lone selected attributes.
 */
static GList *
lone_attributes_to_model_rows(GschemMultiattribDockable *multiattrib)
{
  GList *o_iter;
  GList *model_rows = NULL;

  /* populate the store with attributes */
  for (o_iter = multiattrib->object_list == NULL ? NULL : geda_list_get_glist(multiattrib->object_list);
       o_iter != NULL;
       o_iter = g_list_next(o_iter))
  {
    OBJECT *object = o_iter->data;
    MODEL_ROW *m_row;

    /* Consider a selected text object might be an attribute */
    if (object->type != OBJ_TEXT ||
        !o_attrib_get_name_value(object, NULL, NULL))
      continue;

    /* We have an OBJ_TEXT which libgeda can parse as an attribute */

    multiattrib->num_lone_attribs_in_list++;

    m_row = g_new0(MODEL_ROW, 1);
    m_row->inherited = o_attrib_is_inherited(object);
    o_attrib_get_name_value(object, &m_row->name, &m_row->value);
    m_row->visibility = o_is_visible(object);
    m_row->show_name_value = object->show_name_value;
    m_row->nth_with_name = 0; /* All selected attributes are treated individually */

    /* The following fields are always true for a single attribute */
    m_row->present_in_all = TRUE;
    m_row->identical_value = TRUE;
    m_row->identical_visibility = TRUE;
    m_row->identical_show_name = TRUE;
    m_row->identical_show_value = TRUE;

    m_row->attribute_gedalist = geda_list_new();
    geda_list_add(m_row->attribute_gedalist, object);

    model_rows = g_list_append(model_rows, m_row);
  }

  return model_rows;
}

/*! \brief Populate the multiattrib editor dialog's liststore
 *
 *  \par Function Description
 *
 *  Consumes the GList of MODEL_ROW data, populating the dialog's liststore.
 *  The function frees / consumes the GList and MODEL_ROW data.
 *
 *  \param [in] multiattrib  The multi-attribute editor dialog.
 *  \param [in] model_rows   A GList of MODEL_ROW data.
 */
static void
multiattrib_populate_liststore(GschemMultiattribDockable *multiattrib,
                               GList *model_rows)
{
  GtkListStore *liststore;
  GtkTreeIter tree_iter;
  GList *m_iter;

  /* Clear the existing list of attributes */
  liststore = GTK_LIST_STORE(multiattrib->store);
  gtk_list_store_clear(liststore);

  for (m_iter = model_rows;
       m_iter != NULL;
       m_iter = g_list_next(m_iter))
  {

    MODEL_ROW *model_row = m_iter->data;

    model_row->present_in_all =
        (g_list_length(geda_list_get_glist(model_row->attribute_gedalist)) == multiattrib->total_num_in_list);

    gtk_list_store_append(liststore, &tree_iter);
    gtk_list_store_set(liststore,
                       &tree_iter,
                       COLUMN_INHERITED, model_row->inherited,
                       COLUMN_NAME, model_row->name,
                       COLUMN_VALUE, model_row->value,
                       COLUMN_VISIBILITY, model_row->visibility,
                       COLUMN_SHOW_NAME_VALUE, model_row->show_name_value,
                       COLUMN_PRESENT_IN_ALL, model_row->present_in_all,
                       COLUMN_IDENTICAL_VALUE, model_row->identical_value,
                       COLUMN_IDENTICAL_VISIBILITY, model_row->identical_visibility,
                       COLUMN_IDENTICAL_SHOW_NAME, model_row->identical_show_name,
                       COLUMN_IDENTICAL_SHOW_VALUE, model_row->identical_show_value,
                       COLUMN_ATTRIBUTE_GEDALIST, model_row->attribute_gedalist,
                       -1);

    /* Drop our ref on the GedaList so it is freed when the model has done with it */
    g_object_unref(model_row->attribute_gedalist);
  }

  g_list_foreach(model_rows, (GFunc)g_free, NULL);
  g_list_free(model_rows);
}

static void
append_dialog_title_extra(GString *title_string,
                          int *num_title_extras,
                          char *text,
                          ...)
{
  va_list args;

  va_start(args, text);
  if ((*num_title_extras)++ != 0)
    g_string_append(title_string, ", ");
  g_string_append_vprintf(title_string, text, args);
  va_end(args);
}

static void
update_dialog_title(GschemMultiattribDockable *multiattrib,
                    char *complex_title_name)
{
  GString *title_string = g_string_new(NULL);
  int num_title_extras = 0;

  if (multiattrib->num_complex_in_list > 0)
  {
    append_dialog_title_extra(title_string, &num_title_extras,
                              ngettext("%i component (%s)", "%i components (%s)", multiattrib->num_complex_in_list),
                              multiattrib->num_complex_in_list, complex_title_name);
  }

  if (multiattrib->num_pins_in_list > 0)
  {
    append_dialog_title_extra(title_string, &num_title_extras,
                              ngettext("%i pin", "%i pins", multiattrib->num_pins_in_list),
                              multiattrib->num_pins_in_list);
  }

  if (multiattrib->num_nets_in_list > 0)
  {
    append_dialog_title_extra(title_string, &num_title_extras,
                              ngettext("%i net", "%i nets", multiattrib->num_nets_in_list),
                              multiattrib->num_nets_in_list);
  }

  if (multiattrib->num_buses_in_list > 0)
  {
    append_dialog_title_extra(title_string, &num_title_extras,
                              ngettext("%i bus", "%i buses", multiattrib->num_buses_in_list),
                              multiattrib->num_buses_in_list);
  }

  if (multiattrib->num_lone_attribs_in_list > 0)
  {
    append_dialog_title_extra(title_string, &num_title_extras,
                              ngettext("%i attribute", "%i attributes", multiattrib->num_lone_attribs_in_list),
                              multiattrib->num_lone_attribs_in_list);
  }

  char *title = g_string_free(title_string, FALSE);
  if (title != NULL && *title != '\0')
    gtk_label_set_text(GTK_LABEL(multiattrib->status_label), title);
  else
    gtk_label_set_text(GTK_LABEL(multiattrib->status_label),
                       _("Select a component, net, or attribute"));
  g_free(title);
}

/*! \brief Update the multiattrib editor dialog's interface
 *
 *  \par Function Description
 *
 *  Update the dialog to reflect the attributes of the currently selected
 *  object. If no (or multiple) objects are selected, the dialog's controls
 *  are set insensitive.
 *
 *  \param [in] multiattrib  The multi-attribute editor dialog.
 */
static void
multiattrib_update(GschemMultiattribDockable *multiattrib)
{
  GList *o_iter;
  GtkStyle *style;
  gboolean show_inherited;
  gboolean list_sensitive;
  gboolean add_sensitive;
  GList *model_rows = NULL;
  char *complex_title_name = NULL;

  if (multiattrib->parent.widget == NULL)
    /* ignore if widgets haven't been created yet */
    return;

  show_inherited =
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(multiattrib->show_inherited));

  multiattrib->total_num_in_list = 0;
  multiattrib->num_complex_in_list = 0;
  multiattrib->num_pins_in_list = 0;
  multiattrib->num_nets_in_list = 0;
  multiattrib->num_buses_in_list = 0;
  multiattrib->num_lone_attribs_in_list = 0;

  /* populate the store with attributes */
  for (o_iter = multiattrib->object_list == NULL ? NULL : geda_list_get_glist(multiattrib->object_list);
       o_iter != NULL;
       o_iter = g_list_next(o_iter))
  {
    OBJECT *object = o_iter->data;

    GList *object_rows;
    GList *or_iter;

    if (!is_multiattrib_object(object))
      continue;

    /* Count the different objects we are editing */
    multiattrib->total_num_in_list++;

    if (object->type == OBJ_COMPLEX ||
        object->type == OBJ_PLACEHOLDER)
    {
      multiattrib->num_complex_in_list++;

      if (complex_title_name == NULL)
        complex_title_name = object->complex_basename;
      else if (strcmp(complex_title_name, object->complex_basename) != 0)
        complex_title_name = _("<various>");
    }

    if (object->type == OBJ_PIN)
      multiattrib->num_pins_in_list++;

    if (object->type == OBJ_NET)
      multiattrib->num_nets_in_list++;

    if (object->type == OBJ_BUS)
      multiattrib->num_buses_in_list++;

    /* populate the store with any attributes from this object */
    object_rows = object_attributes_to_model_rows(multiattrib, object);

    for (or_iter = object_rows;
         or_iter != NULL;
         or_iter = g_list_next(or_iter))
    {

      MODEL_ROW *object_row = or_iter->data;
      MODEL_ROW *model_row;
      GList *mr_iter;
      gboolean found = FALSE;

      /* Skip over inherited attributes if we don't want to show them */
      if (!show_inherited && object_row->inherited)
        continue;

      /* Search our list of attributes to see if we already encountered */
      for (mr_iter = model_rows;
           mr_iter != NULL && found == FALSE;
           mr_iter = g_list_next(mr_iter))
      {
        model_row = mr_iter->data;
        if (strcmp(model_row->name, object_row->name) == 0 &&
            model_row->nth_with_name == object_row->nth_with_name &&
            model_row->inherited == object_row->inherited)
          found = TRUE;
      }

      if (found)
      {
        /* Name matches a previously found attribute */
        /* Check if the rest of its properties match the one we have stored... */

        if (strcmp(model_row->value, object_row->value) != 0)
          model_row->identical_value = FALSE;

        if (model_row->visibility != object_row->visibility)
          model_row->identical_visibility = FALSE;

        if (snv_shows_name(model_row->show_name_value) !=
            snv_shows_name(object_row->show_name_value))
          model_row->identical_show_name = FALSE;

        if (snv_shows_value(model_row->show_name_value) !=
            snv_shows_value(object_row->show_name_value))
          model_row->identical_show_value = FALSE;

        /* Add the underlying attribute to the row's GedaList of attributes */
        geda_list_add_glist(model_row->attribute_gedalist,
                            geda_list_get_glist(object_row->attribute_gedalist));

        g_object_unref(object_row->attribute_gedalist);
        g_free(object_row);
      }
      else
      {
        /*
         * The attribute name doesn't match any previously found
         * attribute, so add the model row entry describing it to the list.
         */
        model_rows = g_list_append(model_rows, object_row);
      }
    }

    /* delete the list of attribute objects */
    g_list_free(object_rows);
  }

  if (multiattrib->total_num_in_list == 0)
  {

    /* If the selection contains no high level objects we can edit,
     * take a look and see whether there are any lone attributes
     * selected we can edit directly.
     */
    model_rows = lone_attributes_to_model_rows(multiattrib);
    list_sensitive = (multiattrib->num_lone_attribs_in_list > 0);
    add_sensitive = FALSE;
  }
  else
  {
    list_sensitive = TRUE;
    add_sensitive = TRUE;
  }

  multiattrib_populate_liststore(multiattrib, model_rows);

  /* Update window title to describe the objects we are editing. */
  update_dialog_title(multiattrib, complex_title_name);

  /* Update sensitivities */
  gtk_widget_set_sensitive(GTK_WIDGET(multiattrib->list_frame), list_sensitive);
  gtk_widget_set_sensitive(GTK_WIDGET(multiattrib->add_frame), add_sensitive);

  /* Work around GtkTextView's stubborn indifference
   * to GTK_STATE_INSENSITIVE when rendering its text. */
  style = gtk_widget_get_style(GTK_WIDGET(multiattrib->textview_value));
  gtk_widget_modify_text(GTK_WIDGET(multiattrib->textview_value),
                         GTK_STATE_NORMAL,
                         add_sensitive ? &multiattrib->value_normal_text_color
                                       : &style->text[GTK_STATE_INSENSITIVE]);
}
