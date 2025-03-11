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
/*! \todo STILL NEED to clean up line lengths in aa and tr */
#include <config.h>

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) g_object_unref)



/***************** Start of help/keymapping dialog box **************/

/*! \brief Response function for the hotkey dialog
 *  \par Function Description
 *  This function destroys the hotkey dialog and does some cleanup.
 */
void x_dialog_hotkeys_response(GtkWidget *w, gint response,
                               GschemToplevel *w_current)
{
  switch(response) {
  case GTK_RESPONSE_REJECT:
  case GTK_RESPONSE_DELETE_EVENT:
    /* void */
    break;
  default:
    printf("x_dialog_hotkeys_response(): strange signal %d\n", response);
  }
  /* clean up */
  gtk_widget_destroy(w_current->hkwindow);
  w_current->hkwindow = NULL;
}

/*! \brief Fix up displaying icons in list of hotkeys.
 * In gschem, we use both GTK's stock icons and also our own icons
 * that we add to the icon theme search path.  We identify each icon
 * by a single icon name, which might either name a GTK stock icon or
 * a theme icon.  To determine which icon to show, we first check if
 * there's a matching stock icon, and if one doesn't exist, we fall
 * back to looking in the theme.
 *
 * The GtkCellRendererPixbuf doesn't provide this capability.  If its
 * "icon-name" property is set, it doesn't look at stock items, but if
 * its "stock-id" property is set, it ignores the "icon-name" even if
 * no matching stock item exists.
 *
 * This handler hooks into the "notify::stock-id" signal in order to
 * implement the desired fallback behaviour.
 */
static void
x_dialog_hotkeys_cell_stock_id_notify (GObject *gobject,
                                       GParamSpec *pspec,
                                       gpointer user_data)
{
  gchar *stock_id = NULL;
  const gchar *new_icon_name = NULL;
  const gchar *new_stock_id = NULL;
  GtkStockItem stock_info;

  /* Decide whether the requested stock ID actually matches a stock
   * item */
  g_object_get (gobject,
                "stock-id", &stock_id,
                NULL);
  new_stock_id = stock_id;

  if (stock_id != NULL && !gtk_stock_lookup (stock_id, &stock_info)) {
    new_icon_name = stock_id;
    new_stock_id = NULL;
  }

  /* Fix up the cell renderer, making sure that this function doesn't
   * get called recursively. */
  g_signal_handlers_block_by_func (gobject,
                                   x_dialog_hotkeys_cell_stock_id_notify,
                                   NULL);
  g_object_set (gobject,
                "icon-name", new_icon_name,
                "stock-id", new_stock_id,
                NULL);
  g_signal_handlers_unblock_by_func (gobject,
                                     x_dialog_hotkeys_cell_stock_id_notify,
                                     NULL);

  g_free (stock_id);
}

/*! \brief Creates the hotkeys dialog
 *  \par Function Description
 *  This function creates the hotkey dialog and puts the list of hotkeys
 *  into it.
 */
void x_dialog_hotkeys (GschemToplevel *w_current)
{
  GtkWidget *vbox, *scrolled_win;
  GtkTreeModel *store;
  GtkWidget *treeview;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  if (!w_current->hkwindow) {
    w_current->hkwindow = gschem_dialog_new_with_buttons(_("Hotkeys"),
                                                         GTK_WINDOW(w_current->main_window),
                                                         0, /* not modal */
                                                         "hotkeys", w_current,
                                                         GTK_STOCK_CLOSE,
                                                         GTK_RESPONSE_REJECT,
                                                         NULL);

    gtk_window_set_icon_name (GTK_WINDOW (w_current->hkwindow),
                              "preferences-desktop-keyboard-shortcuts");

    gtk_window_set_position (GTK_WINDOW (w_current->hkwindow), GTK_WIN_POS_NONE);

    g_signal_connect (G_OBJECT (w_current->hkwindow), "response",
                      G_CALLBACK (x_dialog_hotkeys_response),
                      w_current);

    gtk_dialog_set_default_response(GTK_DIALOG(w_current->hkwindow),
                                    GTK_RESPONSE_ACCEPT);

    gtk_container_set_border_width (GTK_CONTAINER (w_current->hkwindow),
                                    DIALOG_BORDER_SPACING);
    gtk_widget_set_size_request (w_current->hkwindow, 300, 300);

    vbox = GTK_DIALOG(w_current->hkwindow)->vbox;
    gtk_box_set_spacing(GTK_BOX(vbox), DIALOG_V_SPACING);

    scrolled_win = gtk_scrolled_window_new (NULL, NULL);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);

    /* the model */
    store = GTK_TREE_MODEL (gschem_hotkey_store_new ());

    /* the tree view */
    treeview = gtk_tree_view_new_with_model (store);
    gtk_container_add(GTK_CONTAINER(scrolled_win), treeview);

    /* the columns */
    /* The first column contains the action's icon (if one was set)
     * and its label. */
    renderer = gtk_cell_renderer_pixbuf_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Action"),
                                                       renderer,
                                                       "stock-id",
                                                       GSCHEM_HOTKEY_STORE_COLUMN_ICON,
                                                       NULL);
    /* Fix things up to show stock icons *and* theme icons. */
    g_signal_connect (renderer, "notify::stock-id",
                      G_CALLBACK (x_dialog_hotkeys_cell_stock_id_notify),
                      NULL);

    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_column_pack_start (column, renderer, FALSE);
    gtk_tree_view_column_set_attributes (column, renderer,
                                         "text", GSCHEM_HOTKEY_STORE_COLUMN_LABEL,
                                         NULL);

    /* The second column contains the action's keybinding */
    gtk_tree_view_append_column (GTK_TREE_VIEW(treeview), column);
    column = gtk_tree_view_column_new_with_attributes (_("Keystroke(s)"),
                                                       renderer,
                                                       "text",
                                                       GSCHEM_HOTKEY_STORE_COLUMN_KEYS,
                                                       NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(treeview), column);

    /* show all recursively */
    gtk_widget_show_all(w_current->hkwindow);
  }

  else { /* dialog already created */
    gtk_window_present(GTK_WINDOW(w_current->hkwindow));
  }
}

/***************** End of help/keymapping dialog box ****************/
