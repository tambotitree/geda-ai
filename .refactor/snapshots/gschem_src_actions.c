/**
 * \brief Register all gschem core actions.
 *
 * This function registers every C-defined action (e.g., menu items and toolbar buttons)
 * into the global action registry. It replaces the old macro-based initialization.
 */

#include "config.h"            // Always first
#include <glib/gi18n.h>        // For _() gettext macro
#include "gschem_action.h"     // For gschem_action_register
#include "actions.h"           // For extern action_* pointers and callbacks

void gschem_register_all_actions(void)
{
    /* File menu actions */
    action_file_new = gschem_action_register("file-new", "gtk-new", _("New File"),
                                             _("New"), _("_New"), NULL, ACTUATE, action_callback_file_new);

    action_file_open = gschem_action_register("file-open", "gtk-open", _("Open File"),
                                              _("Open..."), _("_Open..."), NULL, ACTUATE, action_callback_file_open);

    action_file_open_recent = gschem_action_register("file-open-recent", NULL, _("Open Recent File"),
                                                     _("Open Recent"), _("Open Recen_t"), NULL, ACTUATE, action_callback_file_open_recent);

    action_file_save = gschem_action_register("file-save", "gtk-save", _("Save"),
                                              _("Save"), _("_Save"), NULL, ACTUATE, action_callback_file_save);

    action_file_save_as = gschem_action_register("file-save-as", "gtk-save-as", _("Save As"),
                                                 _("Save As..."), _("Save _As..."), NULL, ACTUATE, action_callback_file_save_as);

    action_file_save_all = gschem_action_register("file-save-all", "gtk-save", _("Save All"),
                                                  _("Save All"), _("Save All"), NULL, ACTUATE, action_callback_file_save_all);

    action_page_revert = gschem_action_register("page-revert", "gtk-revert-to-saved", _("Revert Changes"),
                                                _("Revert"), _("_Revert"), NULL, ACTUATE, action_callback_page_revert);

    action_page_close = gschem_action_register("page-close", "gtk-close", _("Close Page"),
                                               _("Close"), _("_Close"), NULL, ACTUATE, action_callback_page_close);

    action_file_print = gschem_action_register("file-print", "gtk-print", _("Print"),
                                               _("Print..."), _("_Print..."), NULL, ACTUATE, action_callback_file_print);

    action_file_write_png = gschem_action_register("file-image", NULL, _("Export Image"),
                                                   _("Export Image..."), _("Export _Image..."), NULL, ACTUATE, action_callback_file_write_png);

    action_file_new_window = gschem_action_register("file-new-window", "window-new", _("New Window"),
                                                    _("New Window"), _("New Window"), NULL, ACTUATE, action_callback_file_new_window);

    action_file_close = gschem_action_register("file-close-window", "gtk-close", _("Close Window"),
                                               _("Close Window"), _("Close Window"), NULL, ACTUATE, action_callback_file_close);

    action_file_quit = gschem_action_register("file-quit", "gtk-quit", _("Quit"),
                                              _("Quit"), _("_Quit"), NULL, ACTUATE, action_callback_file_quit);

    /* Edit menu actions */
    action_edit_undo = gschem_action_register("edit-undo", "gtk-undo", _("Undo"),
                                              _("Undo"), _("_Undo"), NULL, ACTUATE, action_callback_edit_undo);

    action_edit_redo = gschem_action_register("edit-redo", "gtk-redo", _("Redo"),
                                              _("Redo"), _("_Redo"), NULL, ACTUATE, action_callback_edit_redo);

    action_clipboard_cut = gschem_action_register("clipboard-cut", "gtk-cut", _("Cut"),
                                                  _("Cut"), _("Cu_t"), NULL, ACTUATE, action_callback_clipboard_cut);

    action_clipboard_copy = gschem_action_register("clipboard-copy", "gtk-copy", _("Copy"),
                                                   _("Copy"), _("_Copy"), NULL, ACTUATE, action_callback_clipboard_copy);

    action_clipboard_paste = gschem_action_register("clipboard-paste", "gtk-paste", _("Paste"),
                                                    _("Paste"), _("_Paste"), NULL, ACTUATE, action_callback_clipboard_paste);

    action_edit_delete = gschem_action_register("edit-delete", "gtk-delete", _("Delete"),
                                                _("Delete"), _("_Delete"), NULL, ACTUATE, action_callback_edit_delete);

    action_edit_select_all = gschem_action_register("edit-select-all", "gtk-select-all", _("Select All"),
                                                    _("Select All"), _("Select _All"), NULL, ACTUATE, action_callback_edit_select_all);

    action_edit_deselect = gschem_action_register("edit-deselect", "deselect", _("Deselect"),
                                                  _("Deselect"), _("Deselect"), NULL, ACTUATE, action_callback_edit_deselect);

    action_edit_find = gschem_action_register("edit-find-text", "gtk-find", _("Find Text"),
                                              _("Find..."), _("_Find..."), NULL, ACTUATE, action_callback_edit_find);

    /* Add menu actions (partial list shown here) */
    action_add_component = gschem_action_register("add-component", "insert-symbol", _("Component Library"),
                                                  _("Component Library..."), _("_Component..."), NULL, TOGGLE_PLAIN, action_callback_add_component);

    action_add_last_component = gschem_action_register("add-last-component", "insert-symbol", _("Add Last Component"),
                                                       _("Add Last Component"), _("La_st Component"), NULL, TOGGLE_PLAIN, action_callback_add_last_component);

    action_add_net = gschem_action_register("add-net", "insert-net", _("Add Net"),
                                            _("Add Net"), _("_Net"), NULL, TOGGLE_PLAIN, action_callback_add_net);

    action_add_bus = gschem_action_register("add-bus", "insert-bus", _("Add Bus"),
                                            _("Add Bus"), _("B_us"), NULL, TOGGLE_PLAIN, action_callback_add_bus);

    action_add_attribute = gschem_action_register("add-attribute", "insert-attribute", _("Add Attribute"),
                                                  _("Add Attribute..."), _("_Attribute..."), NULL, ACTUATE, action_callback_add_attribute);

    action_add_text = gschem_action_register("add-text", "insert-text", _("Add Text"),
                                             _("Add Text..."), _("_Text..."), NULL, TOGGLE_PLAIN, action_callback_add_text);

    action_add_line = gschem_action_register("add-line", "insert-line", _("Add Line"),
                                             _("Add Line"), _("_Line"), NULL, TOGGLE_PLAIN, action_callback_add_line);

    /* Additional actions would go here... */

    /* Register missing actions after File, Edit, and Add menu action registrations */
    action_file_repl = gschem_action_register("file-repl", NULL, _("REPL"),
                                              _("REPL"), _("_REPL"), NULL, ACTUATE, action_callback_file_repl);

    action_view_menubar = gschem_action_register("view-menubar", NULL, _("Show/Hide Menubar"),
                                                 _("Menubar"), _("Menubar"), NULL, TOGGLE_CHECK, action_callback_view_menubar);

    action_view_toolbar = gschem_action_register("view-toolbar", NULL, _("Show/Hide Toolbar"),
                                                 _("Toolbar"), _("Toolbar"), NULL, TOGGLE_CHECK, action_callback_view_toolbar);

    action_view_scrollbars = gschem_action_register("view-scrollbars", NULL, _("Show/Hide Scrollbars"),
                                                    _("Scrollbars"), _("Scrollbars"), NULL, TOGGLE_CHECK, action_callback_view_scrollbars);

    action_page_prev = gschem_action_register("page-prev", "go-previous", _("Previous Page"),
                                              _("Previous"), _("_Previous"), NULL, ACTUATE, action_callback_page_prev);

    action_page_next = gschem_action_register("page-next", "go-next", _("Next Page"),
                                              _("Next"), _("_Next"), NULL, ACTUATE, action_callback_page_next);

    action_hierarchy_up = gschem_action_register("hierarchy-up", "go-up", _("Hierarchy Up"),
                                                 _("Up"), _("_Up"), NULL, ACTUATE, action_callback_hierarchy_up);
}
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

/*! \file actions.c
 *
 * \brief Action definitions.
 *
 * This file contains the definitions for gschem's menu and toolbar
 * actions.  It is specially processed, resulting in three separate
 * pieces of code for each instance of the DEFINE_ACTION macro:
 *
 * - a global variable "action_<cid>"
 *
 * - code to initialize this variable with the appropriate action
 *   object and export it to the Guile module (gschem core builtins)
 *   as "action-<id>"
 *
 * - the callback procedure associated with the action.
 *
 * The metadata of an action (name, icon, etc.) is initialized from
 * the parameters to the DEFINE_ACTION macro and stored in the action
 * object.  The fields are, in this order:
 *
 * - cid:
 *     The C identifier of the action.
 *
 *     Prefixed with `action_', this is the name of the global
 *     variable under which the action can be referenced by C code.
 *
 *     Example: add_component
 *
 * - id (string):
 *     The identifier of the action, as a string.
 *
 *     This should be identical to the C identifier, except for
 *     underscores, which should be replaced by dashes.
 *
 *     Example: "add-component"
 *
 * - icon_name (string or NULL):
 *     The name of the icon to be used for the action.
 *
 *     This can be either a GTK stock identifier, in which case the
 *     GTK stock icon is used, or an icon name in the current icon
 *     theme.
 *
 *     Example: "insert-symbol"
 *
 * - name (localized string):
 *     The generic name of the action.
 *
 *     This is displayed as a tooltip for toolbar buttons, and in the
 *     hotkey list.  It should be understandable without any context
 *     and not contain a mnemonic or ellipsis.
 *
 *     Example: _("Add Component")
 *
 * - label (localized string):
 *     The generic label of the action.
 *
 *     This is displayed in the context menu.  It should be suitable
 *     to be used as a menu label but be understandable without any
 *     context and not contain a mnemonic.
 *
 *     Example: _("Add Component...")
 *
 * - menu_label (localized string):
 *     The label for the action when shown in the main menu.
 *
 *     This should be identical to the generic label except for the
 *     mnemonic (a character preceded by an underscore for easier
 *     keyboard access) and omissions based on the menu name.
 *
 *     Example: _("_Component...")
 *
 * - tooltip (localized string or NULL):
 *     A string explaining the action.
 *
 *     This is displayed as an additional information when the user
 *     hovers the mouse cursor over the action.  For tool buttons, it
 *     is displayed in addition to the action name.  GTK tends to
 *     display tooltips quite aggressively, so use with care.
 *
 * - type:
 *     How to display menu items or tool buttons.
 *
 *     For regular actions, this is ACTUATE.  If it takes any other
 *     value, the action is considered stateful, and a tool button
 *     associated with the action will be displayed either toggled or
 *     not toggled depending on the action's state.  An associated
 *     menu item will be rendered depending on the type:
 *
 *       TOGGLE_PLAIN: regular menu item (state won't be visible)
 *       TOGGLE_CHECK: check-box menu item
 *       TOGGLE_RADIO: radio check-box menu item
 *
 *     This field only determines how the widgets are rendered; the
 *     action state won't automatically change when the action is
 *     activated.  You will have to do this explicitly:
 *
 *       gschem_action_set_active (action, FALSE / TRUE, w_current);
 *
 *
 * The DEFINE_ACTION macro is followed by the body of the callback
 * function for the action.  The callback function has two parameters:
 *
 * - GschemAction *action:
 *     The associated action object.
 *
 * - GschemToplevel *w_current:
 *     The toplevel object for which the action was called.
 *
 * The callback function is only invoked by gschem_action_activate
 * which checks for a non-null toplevel object and enters a dynamic
 * Guile context for this toplevel object.
 *
 *
 * \note This file only contains actions implemented in C.
 *       Actions implemented in Guile are defined in
 *       gschem/scheme/gschem/builtins.scm.
 */


/* right now, all callbacks except for the ones on the File menu have
 * the middle button shortcut. Let me (Ales) know if we should also
 * shortcut the File button */

/*! \section file-menu File Menu Callback Functions */

GschemAction *action_file_new;

void action_callback_file_new(GschemAction *action, GschemToplevel *w_current)
{
               "file-new",
               "gtk-new",
               _("New File"),
               _("New"),
               _("_New"),
               NULL,
               ACTUATE)
{
  /*! \todo Perhaps this should be renamed to page_new... */
  PAGE *page;

  /* create a new page */
  page = x_highlevel_new_page (w_current, NULL);
  g_return_if_fail (page != NULL);

  s_log_message (_("New page created [%s]\n"), page->page_filename);
}

action_file_new = gschem_action_register(, , , , , , , action_callback_file_new);


GschemAction *action_file_open;

void action_callback_file_open(GschemAction *action, GschemToplevel *w_current)
{
               "file-open",
               "gtk-open",
               _("Open File"),
               _("Open..."),
               _("_Open..."),
               NULL,
               ACTUATE)
{
  /*! \todo Perhaps this should be renamed to page_open... */
  x_fileselect_open (w_current);
}

action_file_open = gschem_action_register(, , , , , , , action_callback_file_open);


GschemAction *action_file_open_recent;

void action_callback_file_open_recent(GschemAction *action, GschemToplevel *w_current)
{
               "file-open-recent",
               NULL,
               _("Open Recent File"),
               _("Open Recent"),
               _("Open Recen_t"),
               NULL,
               ACTUATE)
{
  /* submenu */
}

action_file_open_recent = gschem_action_register(, , , , , , , action_callback_file_open_recent);


GschemAction *action_file_save;

void action_callback_file_save(GschemAction *action, GschemToplevel *w_current)
{
               "file-save",
               "gtk-save",
               _("Save"),
               _("Save"),
               _("_Save"),
               NULL,
               ACTUATE)
{
  x_highlevel_save_page (w_current, NULL);
}

action_file_save = gschem_action_register(, , , , , , , action_callback_file_save);


GschemAction *action_file_save_as;

void action_callback_file_save_as(GschemAction *action, GschemToplevel *w_current)
{
               "file-save-as",
               "gtk-save-as",
               _("Save As"),
               _("Save As..."),
               _("Save _As..."),
               NULL,
               ACTUATE)
{
  x_fileselect_save (w_current);
}

action_file_save_as = gschem_action_register(, , , , , , , action_callback_file_save_as);


GschemAction *action_file_save_all;

void action_callback_file_save_all(GschemAction *action, GschemToplevel *w_current)
{
               "file-save-all",
               "gtk-save",
               _("Save All"),
               _("Save All"),
               _("Save All"),
               NULL,
               ACTUATE)
{
  x_highlevel_save_all (w_current);
}

action_file_save_all = gschem_action_register(, , , , , , , action_callback_file_save_all);


GschemAction *action_page_revert;

void action_callback_page_revert(GschemAction *action, GschemToplevel *w_current)
{
               "page-revert",
               "gtk-revert-to-saved",
               _("Revert Changes"),
               _("Revert"),
               _("_Revert"),
               NULL,
               ACTUATE)
{
  x_highlevel_revert_page (w_current, NULL);
}

action_page_revert = gschem_action_register(, , , , , , , action_callback_page_revert);


GschemAction *action_page_close;

void action_callback_page_close(GschemAction *action, GschemToplevel *w_current)
{
               "page-close",
               "gtk-close",
               _("Close Page"),
               _("Close"),
               _("_Close"),
               NULL,
               ACTUATE)
{
  x_highlevel_close_page (w_current, NULL);
}

action_page_close = gschem_action_register(, , , , , , , action_callback_page_close);


GschemAction *action_file_print;

void action_callback_file_print(GschemAction *action, GschemToplevel *w_current)
{
               "file-print",
               "gtk-print",
               _("Print"),
               _("Print..."),
               _("_Print..."),
               NULL,
               ACTUATE)
{
  x_print (w_current);
}

action_file_print = gschem_action_register(, , , , , , , action_callback_file_print);


GschemAction *action_file_write_png;

void action_callback_file_write_png(GschemAction *action, GschemToplevel *w_current)
{
               "file-image",
               NULL,
               _("Export Image"),
               _("Export Image..."),
               _("Export _Image..."),
               NULL,
               ACTUATE)
{
  x_image_setup(w_current);
}

action_file_write_png = gschem_action_register(, , , , , , , action_callback_file_write_png);


GschemAction *action_file_new_window;

void action_callback_file_new_window(GschemAction *action, GschemToplevel *w_current)
{
               "file-new-window",
               "window-new",
               _("New Window"),
               _("New Window"),
               _("New Window"),
               NULL,
               ACTUATE)
{
  GschemToplevel *w_current_new = NULL;
  PAGE *page = NULL;

  w_current_new = x_window_new (NULL);
  g_return_if_fail (w_current_new != NULL);

  page = x_highlevel_new_page (w_current_new, NULL);
  g_return_if_fail (page != NULL);

  s_log_message (_("New Window created [%s]\n"), page->page_filename);
}

action_file_new_window = gschem_action_register(, , , , , , , action_callback_file_new_window);


/*! \brief Close a window. */

GschemAction *action_file_close;

void action_callback_file_close(GschemAction *action, GschemToplevel *w_current)
{
               "file-close-window",
               "gtk-close",
               _("Close Window"),
               _("Close Window"),
               _("Close Window"),
               NULL,
               ACTUATE)
{
  s_log_message(_("Closing Window\n"));
  x_window_close(w_current);
}

action_file_close = gschem_action_register(, , , , , , , action_callback_file_close);


GschemAction *action_file_quit;

void action_callback_file_quit(GschemAction *action, GschemToplevel *w_current)
{
               "file-quit",
               "gtk-quit",
               _("Quit"),
               _("Quit"),
               _("_Quit"),
               NULL,
               ACTUATE)
{
  x_window_close_all(w_current);
}

action_file_quit = gschem_action_register(, , , , , , , action_callback_file_quit);


/*! \section edit-menu Edit Menu Callback Functions */

GschemAction *action_edit_undo;

void action_callback_edit_undo(GschemAction *action, GschemToplevel *w_current)
{
               "edit-undo",
               "gtk-undo",
               _("Undo"),
               _("Undo"),
               _("_Undo"),
               NULL,
               ACTUATE)
{
  /* If we're cancelling from a move action, re-wind the
   * page contents back to their state before we started.
   *
   * It "might" be nice to sub-undo rotates / zoom changes
   * made whilst moving components, but when the undo code
   * hits s_page_delete(), the place list objects are free'd.
   * Since they are also contained in the schematic page, a
   * crash occurs when the page objects are free'd.
   * */
  if (w_current->inside_action) {
    i_cancel (w_current);
  } else {
    GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
    g_return_if_fail (page_view != NULL);

    PAGE *page = gschem_page_view_get_page (page_view);

    if (page != NULL) {
      o_undo_callback (w_current, page, UNDO_ACTION);
    }
  }
}

action_edit_undo = gschem_action_register(, , , , , , , action_callback_edit_undo);


GschemAction *action_edit_redo;

void action_callback_edit_redo(GschemAction *action, GschemToplevel *w_current)
{
               "edit-redo",
               "gtk-redo",
               _("Redo"),
               _("Redo"),
               _("_Redo"),
               NULL,
               ACTUATE)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  PAGE *page = gschem_page_view_get_page (page_view);

  if (page != NULL) {
    o_undo_callback (w_current, page, REDO_ACTION);
  }
}

action_edit_redo = gschem_action_register(, , , , , , , action_callback_edit_redo);


/*! \brief Cut selection to clipboard, via buffer 0. */

GschemAction *action_clipboard_cut;

void action_callback_clipboard_cut(GschemAction *action, GschemToplevel *w_current)
{
               "clipboard-cut",
               "gtk-cut",
               _("Cut"),
               _("Cut"),
               _("Cu_t"),
               NULL,
               ACTUATE)
{
  if (!o_select_selected (w_current)) return;

  i_update_middle_button (w_current, action, _("Cut to clipboard"));

  o_redraw_cleanstates(w_current);
  o_buffer_cut (w_current, CLIPBOARD_BUFFER);
}

action_clipboard_cut = gschem_action_register(, , , , , , , action_callback_clipboard_cut);


/*! \brief Copy selection to clipboard, via buffer 0. */

GschemAction *action_clipboard_copy;

void action_callback_clipboard_copy(GschemAction *action, GschemToplevel *w_current)
{
               "clipboard-copy",
               "gtk-copy",
               _("Copy"),
               _("Copy"),
               _("_Copy"),
               NULL,
               ACTUATE)
{
  if (!o_select_selected (w_current)) return;

  i_update_middle_button (w_current, action, _("Copy to clipboard"));

  o_buffer_copy (w_current, CLIPBOARD_BUFFER);
}

action_clipboard_copy = gschem_action_register(, , , , , , , action_callback_clipboard_copy);


/*! \brief Start pasting clipboard contents, via buffer 0. */

GschemAction *action_clipboard_paste;

void action_callback_clipboard_paste(GschemAction *action, GschemToplevel *w_current)
{
               "clipboard-paste",
               "gtk-paste",
               _("Paste"),
               _("Paste"),
               _("_Paste"),
               NULL,
               ACTUATE)
{
  int empty;

  /* Choose a default position to start pasting. This is required to
   * make pasting when the cursor is outside the screen or pasting via
   * menu work as expected. */
  gint wx = 0, wy = 0;

  i_update_middle_button (w_current, action, _("Paste from clipboard"));

  g_action_get_position (TRUE, &wx, &wy);

  o_redraw_cleanstates(w_current);
  empty = o_buffer_paste_start (w_current, wx, wy, CLIPBOARD_BUFFER);

  if (empty) {
    i_set_state_msg (w_current, SELECT, _("Empty clipboard"));
  }
}

action_clipboard_paste = gschem_action_register(, , , , , , , action_callback_clipboard_paste);


GschemAction *action_edit_delete;

void action_callback_edit_delete(GschemAction *action, GschemToplevel *w_current)
{
               "edit-delete",
               "gtk-delete",
               _("Delete"),
               _("Delete"),
               _("_Delete"),
               NULL,
               ACTUATE)
{
  i_update_middle_button (w_current, action, _("Delete"));

  if (o_select_return_first_object(w_current)) {
    o_redraw_cleanstates(w_current);
    o_delete_selected (w_current, _("Delete"));
    /* if you delete the objects you must go into select
     * mode after the delete */
    i_action_stop (w_current);
    i_set_state(w_current, SELECT);
    i_update_menus(w_current);
  }
}

action_edit_delete = gschem_action_register(, , , , , , , action_callback_edit_delete);


/*! \brief Select all objects on page. */

GschemAction *action_edit_select_all;

void action_callback_edit_select_all(GschemAction *action, GschemToplevel *w_current)
{
               "edit-select-all",
               "gtk-select-all",
               _("Select All"),
               _("Select All"),
               _("Select _All"),
               NULL,
               ACTUATE)
{
  o_redraw_cleanstates (w_current);

  o_select_visible_unlocked (w_current);

  i_set_state (w_current, SELECT);
  i_action_stop (w_current);
  i_update_menus (w_current);
}

action_edit_select_all = gschem_action_register(, , , , , , , action_callback_edit_select_all);


/*! \brief Deselect all objects on page. */

GschemAction *action_edit_deselect;

void action_callback_edit_deselect(GschemAction *action, GschemToplevel *w_current)
{
               "edit-deselect",
               "deselect",
               _("Deselect"),
               _("Deselect"),
               _("Deselect"),
               NULL,
               ACTUATE)
{
  o_redraw_cleanstates (w_current);

  o_select_unselect_all (w_current);

  i_set_state (w_current, SELECT);
  i_action_stop (w_current);
  i_update_menus (w_current);
}

action_edit_deselect = gschem_action_register(, , , , , , , action_callback_edit_deselect);


GschemAction *action_edit_find;

void action_callback_edit_find(GschemAction *action, GschemToplevel *w_current)
{
               "edit-find-text",
               "gtk-find",
               _("Find Text"),
               _("Find..."),
               _("_Find..."),
               NULL,
               ACTUATE)
{
  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action)
    return;

  find_text_dialog(w_current);
}

action_edit_find = gschem_action_register(, , , , , , , action_callback_edit_find);


/*! \section add-menu Add Menu Callback Functions */

GschemAction *action_add_component;

void action_callback_add_component(GschemAction *action, GschemToplevel *w_current)
{
               "add-component",
               "insert-symbol",
               _("Component Library"),
               _("Component Library..."),
               _("_Component..."),
               NULL,
               TOGGLE_PLAIN)
{
  o_redraw_cleanstates (w_current);

  i_set_state(w_current, COMPMODE);
  gschem_dockable_present (w_current->compselect_dockable);

  i_update_middle_button (w_current, action, _("Component"));

  i_set_state(w_current, SELECT);
}

action_add_component = gschem_action_register(, , , , , , , action_callback_add_component);


GschemAction *action_add_last_component;

void action_callback_add_last_component(GschemAction *action, GschemToplevel *w_current)
{
               "add-last-component",
               "insert-symbol",
               _("Add Last Component"),
               _("Add Last Component"),
               _("La_st Component"),
               NULL,
               TOGGLE_PLAIN)
{
  GschemPageView *page_view =
    gschem_toplevel_get_current_page_view (w_current);

  x_compselect_select_previous_symbol (w_current);
  x_event_faked_motion (page_view, NULL);
}

action_add_last_component = gschem_action_register(, , , , , , , action_callback_add_last_component);


GschemAction *action_add_net;

void action_callback_add_net(GschemAction *action, GschemToplevel *w_current)
{
               "add-net",
               "insert-net",
               _("Add Net"),
               _("Add Net"),
               _("_Net"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;

  o_redraw_cleanstates(w_current);

  i_set_state(w_current, NETMODE);
  i_update_middle_button (w_current, action, _("Net"));

  if (g_action_get_position (TRUE, &wx, &wy)) {
    o_net_reset(w_current);
    o_net_start(w_current, wx, wy);
  }
}

action_add_net = gschem_action_register(, , , , , , , action_callback_add_net);


GschemAction *action_add_bus;

void action_callback_add_bus(GschemAction *action, GschemToplevel *w_current)
{
               "add-bus",
               "insert-bus",
               _("Add Bus"),
               _("Add Bus"),
               _("B_us"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;

  o_redraw_cleanstates(w_current);
  o_invalidate_rubber (w_current);

  i_set_state(w_current, BUSMODE);
  i_update_middle_button (w_current, action, _("Bus"));

  if (g_action_get_position (TRUE, &wx, &wy)) {
    o_bus_start(w_current, wx, wy);
  }
}

action_add_bus = gschem_action_register(, , , , , , , action_callback_add_bus);


GschemAction *action_add_attribute;

void action_callback_add_attribute(GschemAction *action, GschemToplevel *w_current)
{
               "add-attribute",
               "insert-attribute",
               _("Add Attribute"),
               _("Add Attribute..."),
               _("_Attribute..."),
               NULL,
               ACTUATE)
{
  attrib_edit_dialog(w_current, NULL,
                     g_action_get_position (TRUE, NULL, NULL) ? FROM_HOTKEY : FROM_MENU);
  i_update_middle_button (w_current, action, _("Attribute"));

  i_set_state(w_current, SELECT);
}

action_add_attribute = gschem_action_register(, , , , , , , action_callback_add_attribute);


GschemAction *action_add_text;

void action_callback_add_text(GschemAction *action, GschemToplevel *w_current)
{
               "add-text",
               "insert-text",
               _("Add Text"),
               _("Add Text..."),
               _("_Text..."),
               NULL,
               TOGGLE_PLAIN)
{
  o_redraw_cleanstates(w_current);
  o_invalidate_rubber (w_current);

  i_action_stop (w_current);
  i_set_state(w_current, SELECT);
  i_update_middle_button (w_current, action, _("Text"));

  text_input_dialog(w_current);
}

action_add_text = gschem_action_register(, , , , , , , action_callback_add_text);


GschemAction *action_add_line;

void action_callback_add_line(GschemAction *action, GschemToplevel *w_current)
{
               "add-line",
               "insert-line",
               _("Add Line"),
               _("Add Line"),
               _("_Line"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;

  o_redraw_cleanstates(w_current);
  o_invalidate_rubber (w_current);

  i_set_state(w_current, LINEMODE);
  i_update_middle_button (w_current, action, _("Line"));

  if (g_action_get_position (TRUE, &wx, &wy)) {
    o_line_start(w_current, wx, wy);
  }
}

action_add_line = gschem_action_register(, , , , , , , action_callback_add_line);


GschemAction *action_add_path;

void action_callback_add_path(GschemAction *action, GschemToplevel *w_current)
{
               "add-path",
               "insert-path",
               _("Add Path"),
               _("Add Path"),
               _("Pat_h"),
               NULL,
               TOGGLE_PLAIN)
{
  g_assert (w_current != NULL);

  o_redraw_cleanstates (w_current);
  o_invalidate_rubber (w_current);

  i_set_state (w_current, PATHMODE);
  i_update_middle_button (w_current, action, _("Path"));

  /* Don't start path here since setting of its first point and
   * control point requires the left button click and release */
}

action_add_path = gschem_action_register(, , , , , , , action_callback_add_path);


GschemAction *action_add_box;

void action_callback_add_box(GschemAction *action, GschemToplevel *w_current)
{
               "add-box",
               "insert-box",
               _("Add Box"),
               _("Add Box"),
               _("_Box"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;

  o_redraw_cleanstates(w_current);
  o_invalidate_rubber (w_current);

  i_set_state(w_current, BOXMODE);
  i_update_middle_button (w_current, action, _("Box"));

  if (g_action_get_position (TRUE, &wx, &wy)) {
    o_box_start(w_current, wx, wy);
  }
}

action_add_box = gschem_action_register(, , , , , , , action_callback_add_box);


GschemAction *action_add_circle;

void action_callback_add_circle(GschemAction *action, GschemToplevel *w_current)
{
               "add-circle",
               "insert-circle",
               _("Add Circle"),
               _("Add Circle"),
               _("C_ircle"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;

  o_redraw_cleanstates(w_current);
  o_invalidate_rubber (w_current);

  i_set_state(w_current, CIRCLEMODE);
  i_update_middle_button (w_current, action, _("Circle"));

  if (g_action_get_position (TRUE, &wx, &wy)) {
    o_circle_start(w_current, wx, wy);
  }
}

action_add_circle = gschem_action_register(, , , , , , , action_callback_add_circle);


GschemAction *action_add_arc;

void action_callback_add_arc(GschemAction *action, GschemToplevel *w_current)
{
               "add-arc",
               "insert-arc",
               _("Add Arc"),
               _("Add Arc"),
               _("A_rc"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;

  o_redraw_cleanstates(w_current);
  o_invalidate_rubber (w_current);

  i_set_state(w_current, ARCMODE);
  i_update_middle_button (w_current, action, _("Arc"));

  if (g_action_get_position (TRUE, &wx, &wy)) {
    o_arc_start(w_current, wx, wy);
  }
}

action_add_arc = gschem_action_register(, , , , , , , action_callback_add_arc);


GschemAction *action_add_pin;

void action_callback_add_pin(GschemAction *action, GschemToplevel *w_current)
{
               "add-pin",
               "insert-pin",
               _("Add Pin"),
               _("Add Pin"),
               _("_Pin"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;

  o_redraw_cleanstates(w_current);
  o_invalidate_rubber (w_current);

  i_set_state (w_current, PINMODE);
  i_update_middle_button (w_current, action, _("Pin"));

  if (g_action_get_position (TRUE, &wx, &wy)) {
    o_pin_start(w_current, wx, wy);
  }
}

action_add_pin = gschem_action_register(, , , , , , , action_callback_add_pin);


GschemAction *action_add_picture;

void action_callback_add_picture(GschemAction *action, GschemToplevel *w_current)
{
               "add-picture",
               "insert-image",
               _("Add Picture"),
               _("Add Picture..."),
               _("Pictu_re..."),
               NULL,
               TOGGLE_PLAIN)
{
  o_redraw_cleanstates(w_current);
  o_invalidate_rubber (w_current);

  i_set_state(w_current, SELECT);

  picture_selection_dialog(w_current);
}

action_add_picture = gschem_action_register(, , , , , , , action_callback_add_picture);


/*! \section object-menu Object Menu Callback Functions */

/*! \brief Rotate all objects in the selection list by 90 degrees. */

GschemAction *action_edit_edit;

void action_callback_edit_edit(GschemAction *action, GschemToplevel *w_current)
{
               "edit-edit",
               "gtk-edit",
               _("Edit"),
               _("Edit..."),
               _("_Edit..."),
               NULL,
               ACTUATE)
{
  i_update_middle_button (w_current, action, _("Edit"));
  o_edit (w_current, geda_list_get_glist (gschem_toplevel_get_toplevel (w_current)->page_current->selection_list), FALSE);
}

action_edit_edit = gschem_action_register(, , , , , , , action_callback_edit_edit);


GschemAction *action_edit_properties;

void action_callback_edit_properties(GschemAction *action, GschemToplevel *w_current)
{
               "edit-properties",
               "gtk-properties",
               _("Object Properties"),
               _("Object Properties..."),
               _("Object _Properties..."),
               NULL,
               ACTUATE)
{
  i_update_middle_button (w_current, action, _("Properties"));

  /* dialogs have been merged */
  gschem_dockable_present (w_current->object_properties_dockable);
}

action_edit_properties = gschem_action_register(, , , , , , , action_callback_edit_properties);


GschemAction *action_edit_copy;

void action_callback_edit_copy(GschemAction *action, GschemToplevel *w_current)
{
               "edit-copy",
               "clone",
               _("Copy"),
               _("Copy"),
               _("_Copy Mode"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;

  i_update_middle_button (w_current, action, _("Copy"));

  if (o_select_return_first_object(w_current)) {
    o_redraw_cleanstates(w_current);
    if (g_action_get_position (TRUE, &wx, &wy)) {
      o_copy_start(w_current, wx, wy);
    }
    i_set_state (w_current, COPYMODE);
  } else {
    i_set_state_msg(w_current, SELECT, _("Select objs first"));
  }
}

action_edit_copy = gschem_action_register(, , , , , , , action_callback_edit_copy);


GschemAction *action_edit_mcopy;

void action_callback_edit_mcopy(GschemAction *action, GschemToplevel *w_current)
{
               "edit-mcopy",
               "multi-clone",
               _("Multiple Copy"),
               _("Multiple Copy"),
               _("Multiple Cop_y Mode"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;

  i_update_middle_button (w_current, action, _("Multiple Copy"));

  if (o_select_return_first_object(w_current)) {
    o_redraw_cleanstates(w_current);
    if (g_action_get_position (TRUE, &wx, &wy)) {
      o_copy_start(w_current, wx, wy);
    }
    i_set_state (w_current, MCOPYMODE);
  } else {
    i_set_state_msg(w_current, SELECT, _("Select objs first"));
  }
}

action_edit_mcopy = gschem_action_register(, , , , , , , action_callback_edit_mcopy);


GschemAction *action_edit_move;

void action_callback_edit_move(GschemAction *action, GschemToplevel *w_current)
{
               "edit-move",
               NULL,
               _("Move"),
               _("Move"),
               _("_Move Mode"),
               NULL,
               ACTUATE)
{
  gint wx, wy;

  i_update_middle_button (w_current, action, _("Move"));

  if (o_select_return_first_object(w_current)) {
    o_redraw_cleanstates(w_current);
    if (g_action_get_position (TRUE, &wx, &wy)) {
      o_move_start(w_current, wx, wy);
    }
    i_set_state (w_current, MOVEMODE);
  } else {
    i_set_state_msg(w_current, SELECT, _("Select objs first"));
  }
}

action_edit_move = gschem_action_register(, , , , , , , action_callback_edit_move);


GschemAction *action_edit_rotate_90;

void action_callback_edit_rotate_90(GschemAction *action, GschemToplevel *w_current)
{
               "edit-rotate-90",
               "object-rotate-left",
               _("Rotate"),
               _("Rotate"),
               _("_Rotate 90 Mode"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;
  GList *object_list;
  GschemPageView *view = NULL;
  PAGE* page = NULL;

  view = (gschem_toplevel_get_current_page_view (w_current));
  g_return_if_fail (view != NULL);

  page = (gschem_page_view_get_page (view));

  if (page == NULL) {
    return;
  }

  if (w_current->inside_action && (page->place_list != NULL)) {
    o_place_rotate (w_current);
    return;
  }

  if (!g_action_get_position (TRUE, &wx, &wy)) {
    i_set_state(w_current, ROTATEMODE);
    i_update_middle_button (w_current, action, _("Rotate"));
    return;
  }

  o_redraw_cleanstates(w_current);

  object_list = geda_list_get_glist( gschem_toplevel_get_toplevel (w_current)->page_current->selection_list );

  if (object_list) {
    i_update_middle_button (w_current, action, _("Rotate"));
    /* Allow o_rotate_world_update to redraw the objects */
    o_rotate_world_update(w_current, wx, wy, 90, object_list);
  }
}

action_edit_rotate_90 = gschem_action_register(, , , , , , , action_callback_edit_rotate_90);


GschemAction *action_edit_mirror;

void action_callback_edit_mirror(GschemAction *action, GschemToplevel *w_current)
{
               "edit-mirror",
               "object-flip-horizontal",
               _("Mirror"),
               _("Mirror"),
               _("M_irror Mode"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;
  GList *object_list;
  GschemPageView *view = NULL;
  PAGE* page = NULL;

  view = (gschem_toplevel_get_current_page_view (w_current));
  g_return_if_fail (view != NULL);

  page = (gschem_page_view_get_page (view));

  if (page == NULL) {
    return;
  }

  if (w_current->inside_action && (page->place_list != NULL)) {
    o_place_mirror (w_current);
    return;
  }

  if (!g_action_get_position (TRUE, &wx, &wy)) {
    i_set_state(w_current, MIRRORMODE);
    i_update_middle_button (w_current, action, _("Mirror"));
    return;
  }

  o_redraw_cleanstates(w_current);

  object_list = geda_list_get_glist( gschem_toplevel_get_toplevel (w_current)->page_current->selection_list );

  if (object_list) {
    i_update_middle_button (w_current, action, _("Mirror"));
    o_mirror_world_update(w_current, wx, wy, object_list);
  }
}

action_edit_mirror = gschem_action_register(, , , , , , , action_callback_edit_mirror);


GschemAction *action_edit_text;

void action_callback_edit_text(GschemAction *action, GschemToplevel *w_current)
{
               "edit-text",
               "gtk-edit",
               _("Edit Text"),
               _("Edit Text..."),
               _("Edit Te_xt..."),
               NULL,
               ACTUATE)
{
  i_update_middle_button (w_current, action, _("Edit Text"));

  gschem_dockable_present (w_current->text_properties_dockable);
}

action_edit_text = gschem_action_register(, , , , , , , action_callback_edit_text);


GschemAction *action_attributes_attach;

void action_callback_attributes_attach(GschemAction *action, GschemToplevel *w_current)
{
               "attributes-attach",
               "attribute-attach",
               _("Attach Attributes"),
               _("Attach Attributes"),
               _("_Attach Attributes"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  OBJECT *base_object = NULL;
  GList *attached_objects = NULL;

  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action) {
    return;
  }

  /* do we want to update the shortcut outside of the ifs? */
  /* probably, if this fails the user may want to try again */
  i_update_middle_button (w_current, action, _("Attach"));

  /* find object to attach to */
  for (GList *l = geda_list_get_glist (toplevel->page_current->selection_list);
       l != NULL; l = l->next) {
    OBJECT *obj = l->data;
    if (obj->type != OBJ_NET && obj->type != OBJ_BUS && obj->type != OBJ_PIN &&
        obj->type != OBJ_COMPLEX && obj->type != OBJ_PLACEHOLDER)
      continue;

    if (base_object != NULL) {
      g_warning (_("Multiple suitable base objects selected!\n"));
      return;
    }
    base_object = obj;
  }
  if (base_object == NULL) {
    g_warning (_("No suitable base object selected!\n"));
    return;
  }

  for (GList *l = geda_list_get_glist (toplevel->page_current->selection_list);
       l != NULL; l = l->next) {
    OBJECT *obj = l->data;

    /* ignore the base object and already attached attributes */
    if (obj == base_object || obj->attached_to == base_object)
      continue;

    o_attrib_attach (toplevel, obj, base_object, TRUE);
    if (obj->attached_to != base_object)
      /* not a text object or already attached to something else */
      continue;

    attached_objects = g_list_prepend (attached_objects, obj);
  }

  if (attached_objects != NULL) {
    attached_objects = g_list_reverse (attached_objects);
    g_run_hook_object_list (w_current, "%attach-attribs-hook",
                            attached_objects);
    g_list_free (attached_objects);

    gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
    o_undo_savestate_old (w_current, UNDO_ALL, _("Attach Attributes"));
  }

  i_update_menus (w_current);
}

action_attributes_attach = gschem_action_register(, , , , , , , action_callback_attributes_attach);


GschemAction *action_attributes_detach;

void action_callback_attributes_detach(GschemAction *action, GschemToplevel *w_current)
{
               "attributes-detach",
               "attribute-detach",
               _("Detach Attributes"),
               _("Detach Attributes"),
               _("_Detach Attributes"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  GList *detached_attribs = NULL;

  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action) {
    return;
  }

  /* same note as above on i_update_middle_button */
  i_update_middle_button (w_current, action, _("Detach"));

  for (GList *l = geda_list_get_glist (toplevel->page_current->selection_list);
       l != NULL; l = l->next) {
    OBJECT *obj = (OBJECT *) l->data;

    if (obj->type == OBJ_TEXT && obj->attached_to != NULL) {
      obj->attached_to->attribs =
        g_list_remove (obj->attached_to->attribs, obj);
      obj->attached_to = NULL;
      o_set_color (toplevel, obj, DETACHED_ATTRIBUTE_COLOR);
      detached_attribs = g_list_prepend (detached_attribs, obj);
    }
  }

  if (detached_attribs != NULL) {
    detached_attribs = g_list_reverse (detached_attribs);
    g_run_hook_object_list (w_current, "%detach-attribs-hook",
                            detached_attribs);
    g_list_free (detached_attribs);

    gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
    o_undo_savestate_old (w_current, UNDO_ALL, _("Detach Attributes"));
  }

  i_update_menus (w_current);
}

action_attributes_detach = gschem_action_register(, , , , , , , action_callback_attributes_detach);


GschemAction *action_attributes_show_value;

void action_callback_attributes_show_value(GschemAction *action, GschemToplevel *w_current)
{
               "attributes-show-value",
               "attribute-show-value",
               _("Show Attribute Value"),
               _("Show Attribute Value"),
               _("Show Attribute _Value"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);

  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action) {
    return;
  }

  i_update_middle_button (w_current, action, _("ShowV"));

  if (o_select_selected (w_current)) {
    SELECTION *selection = toplevel->page_current->selection_list;
    GList *s_current;
    gboolean changed = FALSE;

    for (s_current = geda_list_get_glist (selection);
         s_current != NULL;
         s_current = g_list_next (s_current)) {
      OBJECT *object = (OBJECT*)s_current->data;
      if (object->type == OBJ_TEXT) {
        o_attrib_toggle_show_name_value (w_current, object, SHOW_VALUE);
        changed = TRUE;
      }
    }

    if (changed) {
      gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
      o_undo_savestate_old (w_current, UNDO_ALL, _("Show Value"));
    }
  }
}

action_attributes_show_value = gschem_action_register(, , , , , , , action_callback_attributes_show_value);


GschemAction *action_attributes_show_name;

void action_callback_attributes_show_name(GschemAction *action, GschemToplevel *w_current)
{
               "attributes-show-name",
               "attribute-show-name",
               _("Show Attribute Name"),
               _("Show Attribute Name"),
               _("Show Attribute _Name"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);

  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action) {
    return;
  }

  i_update_middle_button (w_current, action, _("ShowN"));

  if (o_select_selected (w_current)) {
    SELECTION *selection = toplevel->page_current->selection_list;
    GList *s_current;
    gboolean changed = FALSE;

    for (s_current = geda_list_get_glist (selection);
         s_current != NULL;
         s_current = g_list_next (s_current)) {
      OBJECT *object = (OBJECT*)s_current->data;
      if (object->type == OBJ_TEXT) {
        o_attrib_toggle_show_name_value (w_current, object, SHOW_NAME);
        changed = TRUE;
      }
    }

    if (changed) {
      gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
      o_undo_savestate_old (w_current, UNDO_ALL, _("Show Name"));
    }
  }
}

action_attributes_show_name = gschem_action_register(, , , , , , , action_callback_attributes_show_name);


GschemAction *action_attributes_show_both;

void action_callback_attributes_show_both(GschemAction *action, GschemToplevel *w_current)
{
               "attributes-show-both",
               "attribute-show-both",
               _("Show Name & Value"),
               _("Show Attribute Name & Value"),
               _("Show Attribute Name _& Value"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);

  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action) {
    return;
  }

  i_update_middle_button (w_current, action, _("ShowB"));

  if (o_select_selected (w_current)) {
    SELECTION *selection = toplevel->page_current->selection_list;
    GList *s_current;
    gboolean changed = FALSE;

    for (s_current = geda_list_get_glist (selection);
         s_current != NULL;
         s_current = g_list_next (s_current)) {
      OBJECT *object = (OBJECT*)s_current->data;
      if (object->type == OBJ_TEXT) {
        o_attrib_toggle_show_name_value (w_current, object, SHOW_NAME_VALUE);
        changed = TRUE;
      }
    }

    if (changed) {
      gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
      o_undo_savestate_old (w_current, UNDO_ALL, _("Show Name & Value"));
    }
  }
}

action_attributes_show_both = gschem_action_register(, , , , , , , action_callback_attributes_show_both);


GschemAction *action_attributes_visibility_toggle;

void action_callback_attributes_visibility_toggle(GschemAction *action, GschemToplevel *w_current)
{
               "attributes-visibility-toggle",
               NULL,
               _("Toggle Text Visibility"),
               _("Toggle Text Visibility"),
               _("_Toggle Text Visibility"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);

  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action) {
    return;
  }

  i_update_middle_button (w_current, action, _("VisToggle"));

  if (o_select_selected (w_current)) {
    SELECTION *selection = toplevel->page_current->selection_list;
    GList *s_current;
    gboolean changed = FALSE;

    for (s_current = geda_list_get_glist (selection);
         s_current != NULL;
         s_current = g_list_next (s_current)) {
      OBJECT *object = (OBJECT*)s_current->data;
      if (object->type == OBJ_TEXT) {
        o_attrib_toggle_visibility (w_current, object);
        changed = TRUE;
      }
    }

    if (changed) {
      gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
      o_undo_savestate_old (w_current, UNDO_ALL, _("Toggle Visibility"));
    }
  }
}

action_attributes_visibility_toggle = gschem_action_register(, , , , , , , action_callback_attributes_visibility_toggle);


GschemAction *action_attributes_overbar_toggle;

void action_callback_attributes_overbar_toggle(GschemAction *action, GschemToplevel *w_current)
{
               "attributes-overbar-toggle",
               NULL,
               _("Toggle Text Overbar"),
               _("Toggle Text Overbar"),
               _("To_ggle Text Overbar"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  SELECTION *selection = toplevel->page_current->selection_list;
  GschemSelectionAdapter *adapter =
    gschem_toplevel_get_selection_adapter (w_current);
  gboolean changed = FALSE;

  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action)
    return;

  i_update_middle_button (w_current, action, _("OverbarToggle"));

  for (GList *l = geda_list_get_glist (selection); l != NULL; l = l->next) {
    OBJECT *object = (OBJECT *) l->data;
    if (o_text_toggle_overbar (w_current, object))
      changed = TRUE;
  }
  if (!changed)
    return;

  gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
  o_undo_savestate_old (w_current, UNDO_ALL, _("Toggle Overbar"));

  if (adapter != NULL)
    g_object_notify (G_OBJECT (adapter), "text-string");
  x_multiattrib_update (w_current);
}

action_attributes_overbar_toggle = gschem_action_register(, , , , , , , action_callback_attributes_overbar_toggle);


/*! \brief Embed all objects in selection list. */

GschemAction *action_edit_embed;

void action_callback_edit_embed(GschemAction *action, GschemToplevel *w_current)
{
               "edit-embed",
               NULL,
               _("Embed Symbol/Picture"),
               _("Embed Symbol/Picture"),
               _("Em_bed Symbol/Picture"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  OBJECT *o_current;

  i_update_middle_button (w_current, action, _("Embed"));
  /* anything selected ? */
  if (o_select_selected(w_current)) {
    /* yes, embed each selected component */
    GList *s_current =
      geda_list_get_glist (toplevel->page_current->selection_list);
    gboolean changed = FALSE;

    while (s_current != NULL) {
      o_current = (OBJECT *) s_current->data;
      g_assert (o_current != NULL);
      if ( (o_current->type == OBJ_COMPLEX) ||
	   (o_current->type == OBJ_PICTURE) ) {
        if (o_embed (toplevel, o_current))
          changed = TRUE;
      }
      s_current = g_list_next(s_current);
    }
    if (changed) {
      gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
      o_undo_savestate_old (w_current, UNDO_ALL, _("Embed"));
    }
  } else {
    /* nothing selected, go back to select state */
    o_redraw_cleanstates(w_current);
    i_action_stop (w_current);
    i_set_state(w_current, SELECT);
  }
}

action_edit_embed = gschem_action_register(, , , , , , , action_callback_edit_embed);


/*! \brief Unembed all objects in selection list. */

GschemAction *action_edit_unembed;

void action_callback_edit_unembed(GschemAction *action, GschemToplevel *w_current)
{
               "edit-unembed",
               NULL,
               _("Unembed Symbol/Picture"),
               _("Unembed Symbol/Picture"),
               _("Unembed Symbol/Picture"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  OBJECT *o_current;

  i_update_middle_button (w_current, action, _("Unembed"));
  /* anything selected ? */
  if (o_select_selected(w_current)) {
    /* yes, unembed each selected component */
    GList *s_current =
      geda_list_get_glist (toplevel->page_current->selection_list);
    gboolean changed = FALSE;

    while (s_current != NULL) {
      o_current = (OBJECT *) s_current->data;
      g_assert (o_current != NULL);
      if ( (o_current->type == OBJ_COMPLEX) ||
           (o_current->type == OBJ_PICTURE) ) {
        if (o_unembed (toplevel, o_current))
          changed = TRUE;
      }
      s_current = g_list_next(s_current);
    }
    if (changed) {
      gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
      o_undo_savestate_old (w_current, UNDO_ALL, _("Unembed"));
    }
  } else {
    /* nothing selected, go back to select state */
    o_redraw_cleanstates(w_current);
    i_action_stop (w_current);
    i_set_state(w_current, SELECT);
  }
}

action_edit_unembed = gschem_action_register(, , , , , , , action_callback_edit_unembed);


/*! \brief Update components. */

GschemAction *action_edit_update;

void action_callback_edit_update(GschemAction *action, GschemToplevel *w_current)
{
               "edit-update",
               "gtk-refresh",
               _("Update Symbol"),
               _("Update Symbol"),
               _("_Update Symbol"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  GList *selection;
  GList *selected_components = NULL;
  GList *iter;

  i_update_middle_button (w_current, action, _("Update"));
  if (o_select_selected(w_current)) {

    /* Updating components modifies the selection. Therefore, create a
     * new list of only the OBJECTs we want to update from the current
     * selection, then iterate over that new list to perform the
     * update. */
    selection = geda_list_get_glist (toplevel->page_current->selection_list);
    for (iter = selection; iter != NULL; iter = g_list_next (iter)) {
      OBJECT *o_current = (OBJECT *) iter->data;
      if (o_current != NULL && o_current->type == OBJ_COMPLEX) {
        selected_components = g_list_prepend (selected_components, o_current);
      }
    }
    for (iter = selected_components; iter != NULL; iter = g_list_next (iter)) {
      OBJECT *o_current = (OBJECT *) iter->data;
      iter->data = o_update_component (w_current, o_current);
    }
    g_list_free (selected_components);

  } else {
    /* nothing selected, go back to select state */
    o_redraw_cleanstates(w_current);
    i_action_stop (w_current);
    i_set_state(w_current, SELECT);
  }
}

action_edit_update = gschem_action_register(, , , , , , , action_callback_edit_update);


/*! \brief Lock all objects in selection list. */

GschemAction *action_edit_lock;

void action_callback_edit_lock(GschemAction *action, GschemToplevel *w_current)
{
               "edit-lock",
               NULL,
               _("Lock"),
               _("Lock Component"),
               _("_Lock Component"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  gboolean changed = FALSE;

  i_update_middle_button (w_current, action, _("Lock"));
  if (!o_select_return_first_object (w_current))
    return;

  /* Lock the entire selected list.  It does lock components but does
     NOT change the color (of primatives of the components). */

  for (GList *l = geda_list_get_glist (toplevel->page_current->selection_list);
       l != NULL; l = l->next) {
    OBJECT *object = (OBJECT *) l->data;
    g_assert (object != NULL);

    /* non-component objects can't be locked */
    if (object->type != OBJ_COMPLEX)
      continue;

    /* skip already locked components */
    if (object->selectable == FALSE) {
      s_log_message (_("Object already locked\n"));
      continue;
    }

    object->selectable = FALSE;

    /* apply "locked" color to attached attributes */
    for (GList *la = object->attribs; la != NULL; la = la->next) {
      OBJECT *attrib = (OBJECT *) la->data;
      if (attrib->color == LOCK_COLOR)
        continue;

      attrib->locked_color = attrib->color;
      attrib->color = LOCK_COLOR;
    }

    changed = TRUE;
  }

  if (!w_current->SHIFTKEY)
    o_select_unselect_all (w_current);

  if (changed) {
    gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
    o_undo_savestate_old (w_current, UNDO_ALL, _("Lock"));

    /* invalidate all since unselected attributes may have been changed */
    gschem_page_view_invalidate_all (
      gschem_toplevel_get_current_page_view (w_current));
  }

  i_update_menus (w_current);
}

action_edit_lock = gschem_action_register(, , , , , , , action_callback_edit_lock);


/*! \brief Unlock all objects in selection list. */

GschemAction *action_edit_unlock;

void action_callback_edit_unlock(GschemAction *action, GschemToplevel *w_current)
{
               "edit-unlock",
               NULL,
               _("Unlock"),
               _("Unlock Component"),
               _("Unloc_k Component"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  gboolean changed = FALSE;

  i_update_middle_button (w_current, action, _("Unlock"));
  if (!o_select_return_first_object (w_current))
    return;

  /* You can unlock something by selecting it with a bounding box... */
  /* This will probably change in the future, but for now it's a
     something.. :-) */

  for (GList *l = geda_list_get_glist (toplevel->page_current->selection_list);
       l != NULL; l = l->next) {
    OBJECT *object = (OBJECT *) l->data;
    g_assert (object != NULL);

    /* non-component objects can't be (un-)locked */
    if (object->type != OBJ_COMPLEX)
      continue;

    /* only unlock if the object is locked */
    if (object->selectable == TRUE) {
      s_log_message (_("Object already unlocked\n"));
      continue;
    }

    object->selectable = TRUE;

    /* restore color of attached attributes */
    for (GList *la = object->attribs; la != NULL; la = la->next) {
      OBJECT *attrib = (OBJECT *) la->data;
      if (attrib->color != LOCK_COLOR)
        continue;

      if (attrib->locked_color == -1)
        attrib->color = ATTRIBUTE_COLOR;
      else {
        attrib->color = attrib->locked_color;
        attrib->locked_color = -1;
      }
    }

    changed = TRUE;
  }

  if (changed) {
    gschem_toplevel_page_content_changed (w_current, toplevel->page_current);
    o_undo_savestate_old (w_current, UNDO_ALL, _("Unlock"));

    /* invalidate all since unselected attributes may have been changed */
    gschem_page_view_invalidate_all (
      gschem_toplevel_get_current_page_view (w_current));
  }

  i_update_menus (w_current);
}

action_edit_unlock = gschem_action_register(, , , , , , , action_callback_edit_unlock);


GschemAction *action_edit_slot;

void action_callback_edit_slot(GschemAction *action, GschemToplevel *w_current)
{
               "edit-slot",
               NULL,
               _("Choose Slot"),
               _("Slot..."),
               _("_Slot..."),
               NULL,
               ACTUATE)
{
  OBJECT *object;

  object = o_select_return_first_object(w_current);

  i_update_middle_button (w_current, action, _("Slot"));
  if (object) {
    o_slot_start(w_current, object);
  }
}

action_edit_slot = gschem_action_register(, , , , , , , action_callback_edit_slot);


/*! \brief Search for and display documentation for selected component
 *         in a browser or PDF viewer. */

GschemAction *action_hierarchy_documentation;

void action_callback_hierarchy_documentation(GschemAction *action, GschemToplevel *w_current)
{
               "hierarchy-documentation",
               "symbol-datasheet",
               _("Part Documentation"),
               _("Part Documentation..."),
               _("Part D_ocumentation..."),
               _("View documentation for selected component"),
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  SCM s_show_documentation_proc =
    scm_variable_ref (
      scm_c_public_variable ("gschem gschemdoc",
                             "show-component-documentation-or-error-msg"));

  for (GList *l = geda_list_get_glist (toplevel->page_current->selection_list);
       l != NULL; l = l->next) {
    OBJECT *obj = (OBJECT *) l->data;
    if (obj->type == OBJ_COMPLEX)
      g_scm_eval_protected (scm_list_2 (s_show_documentation_proc,
                                        edascm_from_object (obj)),
                            SCM_UNDEFINED);
  }
}

action_hierarchy_documentation = gschem_action_register(, , , , , , , action_callback_hierarchy_documentation);


/*! \section view-menu View Menu Callback Functions */

GschemAction *action_edit_show_hidden;

void action_callback_edit_show_hidden(GschemAction *action, GschemToplevel *w_current)
{
               "edit-show-hidden",
               NULL,
               _("Show/Hide Invisible Text"),
               _("Show Invisible Text"),
               _("_Show Invisible Text"),
               NULL,
               TOGGLE_CHECK)
{
  i_update_middle_button (w_current, action, _("ShowHidden"));

  o_edit_show_hidden (w_current,
                      s_page_objects (gschem_toplevel_get_toplevel (w_current)->page_current));
}

action_edit_show_hidden = gschem_action_register(, , , , , , , action_callback_edit_show_hidden);


GschemAction *action_view_show_origin;

void action_callback_view_show_origin(GschemAction *action, GschemToplevel *w_current)
{
               "view-show-origin",
               NULL,
               _("Toggle Origin Visibility"),
               _("Show Origin"),
               _("Show Ori_gin"),
               NULL,
               TOGGLE_CHECK)
{
  gschem_options_cycle_show_origin (w_current->options);
  i_show_state (w_current, NULL);
}

action_view_show_origin = gschem_action_register(, , , , , , , action_callback_view_show_origin);


GschemAction *action_view_zoom_in;

void action_callback_view_zoom_in(GschemAction *action, GschemToplevel *w_current)
{
               "view-zoom-in",
               "gtk-zoom-in",
               _("Zoom In"),
               _("Zoom In"),
               _("Zoom _In"),
               NULL,
               ACTUATE)
{
  /* repeat middle shortcut would get into the way of what user is try to do */
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  a_zoom (w_current,
          page_view,
          ZOOM_IN,
          g_action_get_position (FALSE, NULL, NULL) ? HOTKEY : MENU);

  if (w_current->undo_panzoom) {
    o_undo_savestate_old (w_current, UNDO_VIEWPORT_ONLY, _("Zoom In"));
  }
}

action_view_zoom_in = gschem_action_register(, , , , , , , action_callback_view_zoom_in);


GschemAction *action_view_zoom_out;

void action_callback_view_zoom_out(GschemAction *action, GschemToplevel *w_current)
{
               "view-zoom-out",
               "gtk-zoom-out",
               _("Zoom Out"),
               _("Zoom Out"),
               _("Zoom _Out"),
               NULL,
               ACTUATE)
{
  /* repeat middle shortcut would get into the way of what user is try to do */
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  a_zoom(w_current,
         page_view,
         ZOOM_OUT,
         g_action_get_position (FALSE, NULL, NULL) ? HOTKEY : MENU);

  if (w_current->undo_panzoom) {
    o_undo_savestate_old (w_current, UNDO_VIEWPORT_ONLY, _("Zoom Out"));
  }
}

action_view_zoom_out = gschem_action_register(, , , , , , , action_callback_view_zoom_out);


GschemAction *action_view_zoom_extents;

void action_callback_view_zoom_extents(GschemAction *action, GschemToplevel *w_current)
{
               "view-zoom-extents",
               "gtk-zoom-fit",
               _("Zoom Extents"),
               _("Zoom Extents"),
               _("Zoom _Extents"),
               NULL,
               ACTUATE)
{
  /* repeat middle shortcut would get into the way of what user is try to do */
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  gschem_page_view_zoom_extents (page_view, NULL);

  if (w_current->undo_panzoom) {
    o_undo_savestate_old (w_current, UNDO_VIEWPORT_ONLY, _("Zoom Extents"));
  }
}

action_view_zoom_extents = gschem_action_register(, , , , , , , action_callback_view_zoom_extents);


GschemAction *action_view_zoom_full;

void action_callback_view_zoom_full(GschemAction *action, GschemToplevel *w_current)
{
               "view-zoom-full",
               NULL,
               _("Zoom Full"),
               _("Zoom Full"),
               _("Zoom _Full"),
               NULL,
               ACTUATE)
{
  /* repeat middle shortcut would get into the way of what user is try to do */
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  /* scroll bar stuff */
  a_zoom(w_current, page_view, ZOOM_FULL, DONTCARE);

  if (w_current->undo_panzoom) {
    o_undo_savestate_old (w_current, UNDO_VIEWPORT_ONLY, _("Zoom Full"));
  }
}

action_view_zoom_full = gschem_action_register(, , , , , , , action_callback_view_zoom_full);


GschemAction *action_view_zoom_box;

void action_callback_view_zoom_box(GschemAction *action, GschemToplevel *w_current)
{
               "view-zoom-box",
               NULL,
               _("Zoom Box"),
               _("Zoom Box"),
               _("Zoom Bo_x"),
               NULL,
               TOGGLE_PLAIN)
{
  /* repeat middle shortcut would get into the way of what user is try to do */
  gint wx, wy;

  o_redraw_cleanstates(w_current);

  i_set_state(w_current, ZOOMBOX);

  if (g_action_get_position (FALSE, &wx, &wy)) {
    a_zoom_box_start(w_current, wx, wy);
  }
}

action_view_zoom_box = gschem_action_register(, , , , , , , action_callback_view_zoom_box);


GschemAction *action_view_pan;

void action_callback_view_pan(GschemAction *action, GschemToplevel *w_current)
{
               "view-pan",
               NULL,
               _("Pan"),
               _("Pan"),
               _("_Pan"),
               NULL,
               TOGGLE_PLAIN)
{
  gint wx, wy;

  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  i_update_middle_button (w_current, action, _("Pan"));

  if (!g_action_get_position (FALSE, &wx, &wy)) {
    o_redraw_cleanstates (w_current);
    i_action_stop (w_current);
    i_set_state (w_current, PAN);
  } else {
    gschem_page_view_pan (page_view, wx, wy);
    if (w_current->undo_panzoom) {
      o_undo_savestate_old (w_current, UNDO_VIEWPORT_ONLY, _("Pan"));
    }
  }
}

action_view_pan = gschem_action_register(, , , , , , , action_callback_view_pan);


GschemAction *action_view_redraw;

void action_callback_view_redraw(GschemAction *action, GschemToplevel *w_current)
{
               "view-redraw",
               "gtk-refresh",
               _("Redraw"),
               _("Redraw"),
               _("Re_draw"),
               NULL,
               ACTUATE)
{
  /* repeat middle shortcut doesn't make sense on redraw,
     just hit right button */
  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}

action_view_redraw = gschem_action_register(, , , , , , , action_callback_view_redraw);


/*! \brief Toggle visibility of the menu bar. */

GschemAction *action_view_menubar;

void action_callback_view_menubar(GschemAction *action, GschemToplevel *w_current)
{
               "view-menubar",
               NULL,
               _("Show/Hide Menubar"),
               _("Show Menubar"),
               _("Show _Menubar"),
               NULL,
               TOGGLE_CHECK)
{
  GtkWidget *w = w_current->menubar;
  if (w == NULL)
    return;
  if (w_current->handleboxes)
    w = gtk_widget_get_parent (w);

  gboolean show = !gtk_widget_get_visible (w);
  gtk_widget_set_visible (w, show);
  gschem_action_set_active (action, show, w_current);


  /* If the user accidentally hid the menu bar, they may not know how
     to re-show it.  They may try the context menu, so add a temporary
     "Show Menubar" item there. */

  /* See if there's a "Show Menubar" action in the context menu and/or
     update the visibility of any previously created temporary item. */
  GList *popup_items =
    gtk_container_get_children (GTK_CONTAINER (w_current->popup_menu));
  gboolean found = FALSE;
  for (GList *l = popup_items; l != NULL; l = l->next) {
    if (g_object_get_data (l->data, "action") == action)
      found = TRUE;
    if (g_object_get_data (l->data, "temporary-item") == action)
      gtk_widget_set_visible (GTK_WIDGET (l->data), !show);
  }
  g_list_free (popup_items);

  /* Only need to create the temporary item if we actually hid the
     menu bar and if there's not already a matching item in the menu. */
  if (show || found)
    return;

  GtkWidget *menu_item;

  menu_item = gtk_menu_item_new();
  g_object_set_data (G_OBJECT (menu_item), "temporary-item", action);
  gtk_menu_shell_append (GTK_MENU_SHELL (w_current->popup_menu), menu_item);
  gtk_widget_show (menu_item);

  menu_item = gschem_action_create_menu_item (action, FALSE, w_current);
  g_object_set_data (G_OBJECT (menu_item), "temporary-item", action);
  gtk_menu_shell_append (GTK_MENU_SHELL (w_current->popup_menu), menu_item);
  gtk_widget_show (menu_item);
}

action_view_menubar = gschem_action_register(, , , , , , , action_callback_view_menubar);


/*! \brief Toggle visibility of the toolbar. */

GschemAction *action_view_toolbar;

void action_callback_view_toolbar(GschemAction *action, GschemToplevel *w_current)
{
               "view-toolbar",
               NULL,
               _("Show/Hide Toolbar"),
               _("Show Toolbar"),
               _("Show _Toolbar"),
               NULL,
               TOGGLE_CHECK)
{
  GtkWidget *w = w_current->toolbar;
  if (w == NULL)
    return;
  if (w_current->handleboxes)
    w = gtk_widget_get_parent (w);

  gboolean show = !gtk_widget_get_visible (w);
  gtk_widget_set_visible (w, show);
  gschem_action_set_active (action, show, w_current);
}

action_view_toolbar = gschem_action_register(, , , , , , , action_callback_view_toolbar);


GschemAction *action_view_scrollbars;

void action_callback_view_scrollbars(GschemAction *action, GschemToplevel *w_current)
{
               "view-scrollbars",
               NULL,
               _("Show/Hide Scrollbars"),
               _("Show Scrollbars"),
               _("Show S_crollbars"),
               NULL,
               TOGGLE_CHECK)
{
  GtkWidget *scrolled_window;
  GtkPolicyType policy;
  gboolean show_scrollbars;

  scrolled_window = gtk_widget_get_parent (w_current->drawing_area);
  g_return_if_fail (GTK_IS_SCROLLED_WINDOW (scrolled_window));
  gtk_scrolled_window_get_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  &policy, NULL);

  show_scrollbars = policy == GTK_POLICY_NEVER;
  policy = show_scrollbars ? GTK_POLICY_ALWAYS : GTK_POLICY_NEVER;
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  policy, policy);
  gschem_action_set_active (action, show_scrollbars, w_current);
}

action_view_scrollbars = gschem_action_register(, , , , , , , action_callback_view_scrollbars);


GschemAction *action_docking_area_left;

void action_callback_docking_area_left(GschemAction *action, GschemToplevel *w_current)
{
               "docking-area-left",
               NULL,
               _("Left Dock Windows"),
               _("Left Dock"),
               _("_Left Dock"),
               NULL,
               ACTUATE)
{
  /* submenu */
}

action_docking_area_left = gschem_action_register(, , , , , , , action_callback_docking_area_left);


GschemAction *action_docking_area_bottom;

void action_callback_docking_area_bottom(GschemAction *action, GschemToplevel *w_current)
{
               "docking-area-bottom",
               NULL,
               _("Bottom Dock Windows"),
               _("Bottom Dock"),
               _("_Bottom Dock"),
               NULL,
               ACTUATE)
{
  /* submenu */
}

action_docking_area_bottom = gschem_action_register(, , , , , , , action_callback_docking_area_bottom);


GschemAction *action_docking_area_right;

void action_callback_docking_area_right(GschemAction *action, GschemToplevel *w_current)
{
               "docking-area-right",
               NULL,
               _("Right Dock Windows"),
               _("Right Dock"),
               _("_Right Dock"),
               NULL,
               ACTUATE)
{
  /* submenu */
}

action_docking_area_right = gschem_action_register(, , , , , , , action_callback_docking_area_right);


/*! \section pages-menu Pages Menu Callback Functions */

GschemAction *action_page_update_messages;

void action_callback_page_update_messages(GschemAction *action, GschemToplevel *w_current)
{
               "page-update-messages",
               "gtk-refresh",
               _("Update Messages"),
               _("Update Messages"),
               _("_Update Messages"),
               NULL,
               ACTUATE)
{
  x_messages_update (w_current);
}

action_page_update_messages = gschem_action_register(, , , , , , , action_callback_page_update_messages);


GschemAction *action_hierarchy_down_schematic;

void action_callback_hierarchy_down_schematic(GschemAction *action, GschemToplevel *w_current)
{
               "hierarchy-down-schematic",
               "gtk-go-down",
               _("Down Schematic"),
               _("Edit Subschematic"),
               _("Edit _Subschematic"),
               NULL,
               ACTUATE)
{
  OBJECT *object = o_select_return_first_object (w_current);

  if (object != NULL)
    x_hierarchy_down_schematic (w_current, object);
}

action_hierarchy_down_schematic = gschem_action_register(, , , , , , , action_callback_hierarchy_down_schematic);


GschemAction *action_hierarchy_down_symbol;

void action_callback_hierarchy_down_symbol(GschemAction *action, GschemToplevel *w_current)
{
               "hierarchy-down-symbol",
               "gtk-goto-bottom",
               _("Down Symbol"),
               _("Edit Symbol"),
               _("Edit S_ymbol"),
               NULL,
               ACTUATE)
{
  OBJECT *object = o_select_return_first_object (w_current);

  if (object != NULL)
    x_hierarchy_down_symbol (w_current, object);
}

action_hierarchy_down_symbol = gschem_action_register(, , , , , , , action_callback_hierarchy_down_symbol);


/*! \brief Return to the parent page in the hierarchy of schematics. */

GschemAction *action_hierarchy_up;

void action_callback_hierarchy_up(GschemAction *action, GschemToplevel *w_current)
{
               "hierarchy-up",
               "gtk-go-up",
               _("Up"),
               _("Leave"),
               _("_Leave Subschematic/Symbol"),
               NULL,
               ACTUATE)
{
  x_hierarchy_up (w_current);
}

action_hierarchy_up = gschem_action_register(, , , , , , , action_callback_hierarchy_up);


GschemAction *action_page_prev;

void action_callback_page_prev(GschemAction *action, GschemToplevel *w_current)
{
               "page-prev",
               "gtk-go-back",
               _("Previous Page"),
               _("Previous Page"),
               _("_Previous Page"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  PAGE *p_current = toplevel->page_current;
  PAGE *p_new;
  GList *iter;

  iter = g_list_find( geda_list_get_glist( toplevel->pages ), p_current );
  iter = g_list_previous( iter );

  if ( iter == NULL  )
    return;

  if (w_current->enforce_hierarchy) {
    p_new = s_hierarchy_find_prev_page(toplevel->pages, p_current);
  } else {
    p_new = (PAGE *)iter->data;
  }

  if (p_new == NULL || p_new == p_current) {
    return;
  }

  x_window_set_current_page (w_current, p_new);
}

action_page_prev = gschem_action_register(, , , , , , , action_callback_page_prev);


GschemAction *action_page_next;

void action_callback_page_next(GschemAction *action, GschemToplevel *w_current)
{
               "page-next",
               "gtk-go-forward",
               _("Next Page"),
               _("Next Page"),
               _("_Next Page"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  PAGE *p_current = toplevel->page_current;
  PAGE *p_new;
  GList *iter;

  iter = g_list_find( geda_list_get_glist( toplevel->pages ), p_current );
  iter = g_list_next( iter );

  if (iter == NULL) {
    return;
  }

  if (w_current->enforce_hierarchy) {
    p_new = s_hierarchy_find_next_page(toplevel->pages, p_current);
  } else {
    p_new = (PAGE *)iter->data;
  }

  if (p_new == NULL || p_new == p_current) {
    return;
  }

  x_window_set_current_page (w_current, p_new);
}

action_page_next = gschem_action_register(, , , , , , , action_callback_page_next);


GschemAction *action_page_manager;

void action_callback_page_manager(GschemAction *action, GschemToplevel *w_current)
{
               "page-manager",
               NULL,
               _("Page Manager"),
               _("Page Manager..."),
               _("Page _Manager..."),
               NULL,
               ACTUATE)
{
  gschem_dockable_present (w_current->pagesel_dockable);
}

action_page_manager = gschem_action_register(, , , , , , , action_callback_page_manager);


/*! \section tools-menu Tools Menu Callback Functions */

GschemAction *action_edit_autonumber_text;

void action_callback_edit_autonumber_text(GschemAction *action, GschemToplevel *w_current)
{
               "edit-autonumber",
               NULL,
               _("Autonumber Text"),
               _("Autonumber..."),
               _("A_utonumber..."),
               NULL,
               ACTUATE)
{
  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action)
    return;

  autonumber_text_dialog(w_current);
}

action_edit_autonumber_text = gschem_action_register(, , , , , , , action_callback_edit_autonumber_text);


GschemAction *action_edit_translate;

void action_callback_edit_translate(GschemAction *action, GschemToplevel *w_current)
{
               "edit-translate",
               NULL,
               _("Reset Origin"),
               _("Place Origin"),
               _("_Place Origin"),
               NULL,
               TOGGLE_PLAIN)
{
  SNAP_STATE snap_mode;
  gint wx, wy;

  snap_mode = gschem_options_get_snap_mode (w_current->options);

  if (snap_mode == SNAP_OFF) {
    s_log_message(_("WARNING: Do not translate with snap off!\n"));
    s_log_message(_("WARNING: Turning snap on and continuing "
                  "with translate.\n"));
    gschem_options_set_snap_mode (w_current->options, SNAP_GRID);
    i_show_state(w_current, NULL); /* update status on screen */
  }

  if (gschem_options_get_snap_size (w_current->options) != 100) {
    s_log_message(_("WARNING: Snap grid size is "
                  "not equal to 100!\n"));
    s_log_message(_("WARNING: If you are translating a symbol "
                  "to the origin, the snap grid size should be "
                  "set to 100\n"));
  }

  o_redraw_cleanstates (w_current);
  i_set_state (w_current, OGNRSTMODE);
  i_update_middle_button (w_current, action, _("Reset Origin"));

  i_action_start (w_current);
  if (g_action_get_position (TRUE, &wx, &wy))
    o_ognrst_motion (w_current, wx, wy);
  else
    w_current->rubber_visible = 0;
}

action_edit_translate = gschem_action_register(, , , , , , , action_callback_edit_translate);


GschemAction *action_edit_find_patch;

void action_callback_edit_find_patch(GschemAction *action, GschemToplevel *w_current)
{
               "edit-find-patch",
               NULL,
               _("Import Patch And Find Mismatches"),
               _("Import Patch..."),
               _("_Import Patch..."),
               NULL,
               ACTUATE)
{
  x_patch_import (w_current);
}

action_edit_find_patch = gschem_action_register(, , , , , , , action_callback_edit_find_patch);


GschemAction *action_edit_hide_text;

void action_callback_edit_hide_text(GschemAction *action, GschemToplevel *w_current)
{
               "edit-hide-text",
               NULL,
               _("Hide Specific Text"),
               _("Hide Specific Text..."),
               _("_Hide Specific Text..."),
               NULL,
               ACTUATE)
{
  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action)
    return;

  hide_text_dialog(w_current);
}

action_edit_hide_text = gschem_action_register(, , , , , , , action_callback_edit_hide_text);


GschemAction *action_edit_show_text;

void action_callback_edit_show_text(GschemAction *action, GschemToplevel *w_current)
{
               "edit-show-text",
               NULL,
               _("Show Specific Text"),
               _("Show Specific Text..."),
               _("_Show Specific Text..."),
               NULL,
               ACTUATE)
{
  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action)
    return;

  show_text_dialog(w_current);
}

action_edit_show_text = gschem_action_register(, , , , , , , action_callback_edit_show_text);


GschemAction *action_edit_select_locked;

void action_callback_edit_select_locked(GschemAction *action, GschemToplevel *w_current)
{
               "edit-select-locked",
               NULL,
               _("Select Locked Objects"),
               _("Select Locked Objects"),
               _("Select Loc_ked Objects"),
               NULL,
               ACTUATE)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  SELECTION *selection = toplevel->page_current->selection_list;
  GList *added;

  o_redraw_cleanstates (w_current);
  o_select_unselect_all (w_current);

  for (const GList *l = s_page_objects (toplevel->page_current);
       l != NULL; l = l->next) {
    OBJECT *obj = (OBJECT *) l->data;
    if (obj->selectable)
      continue;

    o_selection_add (toplevel, selection, obj);
    o_attrib_add_selected (w_current, selection, obj);
  }

  added = geda_list_get_glist (selection);
  if (added != NULL)
    g_run_hook_object_list (w_current, "%select-objects-hook", added);

  i_set_state (w_current, SELECT);
  i_action_stop (w_current);
  i_update_menus (w_current);
}

action_edit_select_locked = gschem_action_register(, , , , , , , action_callback_edit_select_locked);


GschemAction *action_options_show_log_window;

void action_callback_options_show_log_window(GschemAction *action, GschemToplevel *w_current)
{
               "options-show-log-window",
               NULL,
               _("Show Log Window"),
               _("Log Window..."),
               _("_Log Window..."),
               NULL,
               ACTUATE)
{
  gschem_dockable_present (w_current->log_dockable);
}

action_options_show_log_window = gschem_action_register(, , , , , , , action_callback_options_show_log_window);


GschemAction *action_options_show_coordinates;

void action_callback_options_show_coordinates(GschemAction *action, GschemToplevel *w_current)
{
               "options-show-coordinates",
               NULL,
               _("Show/Hide Coordinates"),
               _("Show Coordinates"),
               _("Show _Coordinates"),
               NULL,
               TOGGLE_CHECK)
{
  GschemBottomWidget *bottom_widget =
    GSCHEM_BOTTOM_WIDGET (w_current->bottom_widget);
  gboolean do_show_coords =
    gtk_widget_get_visible (bottom_widget->coord_label) == FALSE;

  gtk_widget_set_visible (bottom_widget->coord_separator, do_show_coords);
  gtk_widget_set_visible (bottom_widget->coord_label, do_show_coords);

  gschem_action_set_active (action, do_show_coords, w_current);
}

action_options_show_coordinates = gschem_action_register(, , , , , , , action_callback_options_show_coordinates);


GschemAction *action_edit_invoke_macro;

void action_callback_edit_invoke_macro(GschemAction *action, GschemToplevel *w_current)
{
               "edit-invoke-macro",
               "gtk-execute",
               _("Invoke Macro"),
               _("Evaluate Guile Expression..."),
               _("_Evaluate Guile Expression..."),
               NULL,
               ACTUATE)
{
  gtk_widget_show (w_current->macro_widget);
  gtk_widget_grab_focus (gschem_macro_widget_get_entry (GSCHEM_MACRO_WIDGET (w_current->macro_widget)));
}

action_edit_invoke_macro = gschem_action_register(, , , , , , , action_callback_edit_invoke_macro);


GschemAction *action_file_script;

void action_callback_file_script(GschemAction *action, GschemToplevel *w_current)
{
               "file-script",
               "gtk-execute",
               _("Run Script"),
               _("Execute Guile Script..."),
               _("E_xecute Guile Script..."),
               NULL,
               ACTUATE)
{
  setup_script_selector(w_current);
}

action_file_script = gschem_action_register(, , , , , , , action_callback_file_script);


GschemAction *action_file_repl;

void action_callback_file_repl(GschemAction *action, GschemToplevel *w_current)
{
               "file-repl",
               "gtk-execute",
               _("Terminal REPL"),
               _("Start Guile REPL in Terminal..."),
               _("Start Guile _REPL in Terminal..."),
               NULL,
               ACTUATE)
{
  g_scm_eval_protected (scm_list_1 (
                          scm_variable_ref (
                            scm_c_public_variable (
                              "gschem repl",
                              "start-repl-in-background-terminal"))),
                        SCM_UNDEFINED);
}

action_file_repl = gschem_action_register(, , , , , , , action_callback_file_repl);


/*! \section options-menu Options Menu Callback Functions */

GschemAction *action_options_options;

void action_callback_options_options(GschemAction *action, GschemToplevel *w_current)
{
               "options-options",
               "gtk-preferences",
               _("Options"),
               _("Options..."),
               _("_Options..."),
               NULL,
               ACTUATE)
{
  gschem_dockable_present (w_current->options_dockable);
}

action_options_options = gschem_action_register(, , , , , , , action_callback_options_options);


GschemAction *action_options_rubberband;

void action_callback_options_rubberband(GschemAction *action, GschemToplevel *w_current)
{
               "options-rubberband",
               NULL,
               _("Toggle Net Rubber Band"),
               _("Net Rubberband Mode"),
               _("Net _Rubberband Mode"),
               _("When moving components or nets, drag the nets which are "
                 "attached to them"),
               TOGGLE_CHECK)
{
  /* Rubber band is cool !
   * Added on/off option from the pull down menu
   * Chris Ellec - January 2001 */

  gschem_options_cycle_net_rubber_band_mode (w_current->options);

  if (gschem_options_get_net_rubber_band_mode (w_current->options)) {
    s_log_message(_("Rubber band ON\n"));
  } else {
    s_log_message(_("Rubber band OFF \n"));
  }
}

action_options_rubberband = gschem_action_register(, , , , , , , action_callback_options_rubberband);


/*! \brief Toggle magnetic net mode option ON and OFF. */

GschemAction *action_options_magneticnet;

void action_callback_options_magneticnet(GschemAction *action, GschemToplevel *w_current)
{
               "options-magneticnet",
               NULL,
               _("Toggle Magnetic Nets"),
               _("Magnetic Net Mode"),
               _("_Magnetic Net Mode"),
               _("When adding nets, the endpoint snaps to the nearest net or "
                 "pin (indicated by a circle)"),
               TOGGLE_CHECK)
{
  gschem_options_cycle_magnetic_net_mode (w_current->options);

  if (gschem_options_get_magnetic_net_mode (w_current->options)) {
    s_log_message(_("magnetic net mode: ON\n"));
  }
  else {
    s_log_message(_("magnetic net mode: OFF\n"));
  }

  i_show_state(w_current, NULL);
}

action_options_magneticnet = gschem_action_register(, , , , , , , action_callback_options_magneticnet);


GschemAction *action_options_afeedback;

void action_callback_options_afeedback(GschemAction *action, GschemToplevel *w_current)
{
               "options-action-feedback",
               NULL,
               _("Toggle Bounding Box Drawing"),
               _("Draw Bounding Box"),
               _("Draw _Bounding Box"),
               _("While moving objects, show their bounding box only "
                 "(speeds up drawing, may be useful for older machines)"),
               TOGGLE_CHECK)
{
  /* repeat last command doesn't make sense on options either??? (does it?) */

  if (w_current->actionfeedback_mode == BOUNDINGBOX) {
    w_current->actionfeedback_mode = OUTLINE;
    s_log_message(_("Action feedback mode set to OUTLINE\n"));
  } else {
    w_current->actionfeedback_mode = BOUNDINGBOX;
    s_log_message(_("Action feedback mode set to BOUNDINGBOX\n"));
  }
  if (w_current->inside_action &&
      gschem_toplevel_get_toplevel (w_current)->page_current->place_list != NULL)
    o_place_invalidate_rubber (w_current, FALSE);

  gschem_action_set_active (action,
                            w_current->actionfeedback_mode == BOUNDINGBOX,
                            w_current);
}

action_options_afeedback = gschem_action_register(, , , , , , , action_callback_options_afeedback);


GschemAction *action_options_snap_off;

void action_callback_options_snap_off(GschemAction *action, GschemToplevel *w_current)
{
               "options-snap-off",
               NULL,
               _("Snap Off"),
               _("Snap Off"),
               _("Snap Off"),
               NULL,
               TOGGLE_RADIO)
{
  gschem_options_set_snap_mode (w_current->options, SNAP_OFF);

  i_show_state (w_current, NULL); /* update status on screen */
  i_update_grid_info (w_current);
}

action_options_snap_off = gschem_action_register(, , , , , , , action_callback_options_snap_off);


GschemAction *action_options_snap_grid;

void action_callback_options_snap_grid(GschemAction *action, GschemToplevel *w_current)
{
               "options-snap-grid",
               NULL,
               _("Snap On"),
               _("Snap On"),
               _("Snap On"),
               _("When moving off-grid points, translate them by multiples of "
                 "the grid but preserve their offset relative to the grid"),
               TOGGLE_RADIO)
{
  gschem_options_set_snap_mode (w_current->options, SNAP_GRID);

  i_show_state (w_current, NULL); /* update status on screen */
  i_update_grid_info (w_current);
}

action_options_snap_grid = gschem_action_register(, , , , , , , action_callback_options_snap_grid);


GschemAction *action_options_snap_resnap;

void action_callback_options_snap_resnap(GschemAction *action, GschemToplevel *w_current)
{
               "options-snap-resnap",
               NULL,
               _("Snap Back to Nearest Grid Point"),
               _("Snap Back to Nearest Grid Point"),
               _("Snap Back to Nearest Grid Point"),
               _("When moving off-grid objects, snap them to the nearest "
                 "location on the grid"),
               TOGGLE_RADIO)
{
  gschem_options_set_snap_mode (w_current->options, SNAP_RESNAP);

  i_show_state (w_current, NULL); /* update status on screen */
  i_update_grid_info (w_current);
}

action_options_snap_resnap = gschem_action_register(, , , , , , , action_callback_options_snap_resnap);


/*! \brief Multiply snap grid size by two. */

GschemAction *action_options_scale_up_snap_size;

void action_callback_options_scale_up_snap_size(GschemAction *action, GschemToplevel *w_current)
{
               "options-scale-up-snap-size",
               NULL,
               _("Increase Grid Spacing"),
               _("Scale up Grid Spacing"),
               _("Scale _up Grid Spacing"),
               NULL,
               ACTUATE)
{
  gschem_options_scale_snap_up (w_current->options);
}

action_options_scale_up_snap_size = gschem_action_register(, , , , , , , action_callback_options_scale_up_snap_size);


/*! \brief Divide snap grid size by two (if it's and even number). */

GschemAction *action_options_scale_down_snap_size;

void action_callback_options_scale_down_snap_size(GschemAction *action, GschemToplevel *w_current)
{
               "options-scale-down-snap-size",
               NULL,
               _("Decrease Grid Spacing"),
               _("Scale down Grid Spacing"),
               _("Scale _down Grid Spacing"),
               NULL,
               ACTUATE)
{
  gschem_options_scale_snap_down (w_current->options);
}

action_options_scale_down_snap_size = gschem_action_register(, , , , , , , action_callback_options_scale_down_snap_size);


GschemAction *action_view_dark_colors;

void action_callback_view_dark_colors(GschemAction *action, GschemToplevel *w_current)
{
               "view-dark-colors",
               NULL,
               _("Dark Color Scheme"),
               _("Dark Color Scheme"),
               _("D_ark Color Scheme"),
               NULL,
               ACTUATE)
{
  x_color_free();
  /* Change the scheme here */
  g_scm_c_eval_string_protected ("(load (build-path geda-rc-path \"gschem-colormap-darkbg\"))");
  x_color_allocate();

  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}

action_view_dark_colors = gschem_action_register(, , , , , , , action_callback_view_dark_colors);


GschemAction *action_view_light_colors;

void action_callback_view_light_colors(GschemAction *action, GschemToplevel *w_current)
{
               "view-light-colors",
               NULL,
               _("Light Color Scheme"),
               _("Light Color Scheme"),
               _("_Light Color Scheme"),
               NULL,
               ACTUATE)
{
  x_color_free();
  /* Change the scheme here */
  g_scm_c_eval_string_protected ("(load (build-path geda-rc-path \"gschem-colormap-whitebg\"))");
  x_color_allocate();

  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}

action_view_light_colors = gschem_action_register(, , , , , , , action_callback_view_light_colors);


GschemAction *action_view_light_bw_colors;

void action_callback_view_light_bw_colors(GschemAction *action, GschemToplevel *w_current)
{
               "view-light-bw-colors",
               NULL,
               _("Light Monochrome Color Scheme"),
               _("Light Color Scheme (B&W)"),
               _("Light Color Scheme (B&_W)"),
               NULL,
               ACTUATE)
{
  x_color_free();
  /* Change the scheme here */
  g_scm_c_eval_string_protected ("(load (build-path geda-rc-path \"gschem-colormap-whitebg-bw\"))");
  x_color_allocate();

  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}

action_view_light_bw_colors = gschem_action_register(, , , , , , , action_callback_view_light_bw_colors);


GschemAction *action_view_shaded_colors;

void action_callback_view_shaded_colors(GschemAction *action, GschemToplevel *w_current)
{
               "view-shaded-colors",
               NULL,
               _("Shaded Color Scheme"),
               _("Shaded Color Scheme"),
               _("S_haded Color Scheme"),
               NULL,
               ACTUATE)
{
  x_color_free();
  /* Change the scheme here */
  g_scm_c_eval_string_protected ("(load (build-path geda-rc-path \"gschem-colormap-lightbg\"))");
  x_color_allocate();

  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}

action_view_shaded_colors = gschem_action_register(, , , , , , , action_callback_view_shaded_colors);


GschemAction *action_view_shaded_bw_colors;

void action_callback_view_shaded_bw_colors(GschemAction *action, GschemToplevel *w_current)
{
               "view-shaded-bw-colors",
               NULL,
               _("Shaded Monochrome Color Scheme"),
               _("Shaded Color Scheme (B&W)"),
               _("Shaded Color Scheme (B&W)"),
               NULL,
               ACTUATE)
{
  x_color_free();
  /* Change the scheme here */
  g_scm_c_eval_string_protected ("(load (build-path geda-rc-path \"gschem-colormap-bw\"))");
  x_color_allocate();

  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}

action_view_shaded_bw_colors = gschem_action_register(, , , , , , , action_callback_view_shaded_bw_colors);


/*! \section help-menu Help Menu Callback Functions */

GschemAction *action_help_hotkeys;

void action_callback_help_hotkeys(GschemAction *action, GschemToplevel *w_current)
{
               "help-hotkeys",
               "preferences-desktop-keyboard-shortcuts",
               _("Show Hotkeys"),
               _("Hotkeys..."),
               _("_Hotkeys..."),
               NULL,
               ACTUATE)
{
  x_dialog_hotkeys(w_current);
}

action_help_hotkeys = gschem_action_register(, , , , , , , action_callback_help_hotkeys);


GschemAction *action_help_about;

void action_callback_help_about(GschemAction *action, GschemToplevel *w_current)
{
               "help-about",
               "gtk-about",
               _("About gschem"),
               _("About..."),
               _("_About..."),
               NULL,
               ACTUATE)
{
  about_dialog(w_current);
}

action_help_about = gschem_action_register(, , , , , , , action_callback_help_about);


/*! \section buffer-menu Buffer Menu Callback Functions */

GschemAction *action_buffer_copy1;

void action_callback_buffer_copy1(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-copy1",
               "gtk-copy",
               _("Copy into 1"),
               _("Copy into 1"),
               _("Copy into 1"),
               NULL,
               ACTUATE)
{
  i_buffer_copy (w_current, 1, action);
}

action_buffer_copy1 = gschem_action_register(, , , , , , , action_callback_buffer_copy1);


GschemAction *action_buffer_copy2;

void action_callback_buffer_copy2(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-copy2",
               "gtk-copy",
               _("Copy into 2"),
               _("Copy into 2"),
               _("Copy into 2"),
               NULL,
               ACTUATE)
{
  i_buffer_copy (w_current, 2, action);
}

action_buffer_copy2 = gschem_action_register(, , , , , , , action_callback_buffer_copy2);


GschemAction *action_buffer_copy3;

void action_callback_buffer_copy3(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-copy3",
               "gtk-copy",
               _("Copy into 3"),
               _("Copy into 3"),
               _("Copy into 3"),
               NULL,
               ACTUATE)
{
  i_buffer_copy (w_current, 3, action);
}

action_buffer_copy3 = gschem_action_register(, , , , , , , action_callback_buffer_copy3);


GschemAction *action_buffer_copy4;

void action_callback_buffer_copy4(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-copy4",
               "gtk-copy",
               _("Copy into 4"),
               _("Copy into 4"),
               _("Copy into 4"),
               NULL,
               ACTUATE)
{
  i_buffer_copy (w_current, 4, action);
}

action_buffer_copy4 = gschem_action_register(, , , , , , , action_callback_buffer_copy4);


GschemAction *action_buffer_copy5;

void action_callback_buffer_copy5(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-copy5",
               "gtk-copy",
               _("Copy into 5"),
               _("Copy into 5"),
               _("Copy into 5"),
               NULL,
               ACTUATE)
{
  i_buffer_copy (w_current, 5, action);
}

action_buffer_copy5 = gschem_action_register(, , , , , , , action_callback_buffer_copy5);


GschemAction *action_buffer_cut1;

void action_callback_buffer_cut1(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-cut1",
               "gtk-cut",
               _("Cut into 1"),
               _("Cut into 1"),
               _("Cut into 1"),
               NULL,
               ACTUATE)
{
  i_buffer_cut (w_current, 1, action);
}

action_buffer_cut1 = gschem_action_register(, , , , , , , action_callback_buffer_cut1);


GschemAction *action_buffer_cut2;

void action_callback_buffer_cut2(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-cut2",
               "gtk-cut",
               _("Cut into 2"),
               _("Cut into 2"),
               _("Cut into 2"),
               NULL,
               ACTUATE)
{
  i_buffer_cut (w_current, 2, action);
}

action_buffer_cut2 = gschem_action_register(, , , , , , , action_callback_buffer_cut2);


GschemAction *action_buffer_cut3;

void action_callback_buffer_cut3(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-cut3",
               "gtk-cut",
               _("Cut into 3"),
               _("Cut into 3"),
               _("Cut into 3"),
               NULL,
               ACTUATE)
{
  i_buffer_cut (w_current, 3, action);
}

action_buffer_cut3 = gschem_action_register(, , , , , , , action_callback_buffer_cut3);


GschemAction *action_buffer_cut4;

void action_callback_buffer_cut4(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-cut4",
               "gtk-cut",
               _("Cut into 4"),
               _("Cut into 4"),
               _("Cut into 4"),
               NULL,
               ACTUATE)
{
  i_buffer_cut (w_current, 4, action);
}

action_buffer_cut4 = gschem_action_register(, , , , , , , action_callback_buffer_cut4);


GschemAction *action_buffer_cut5;

void action_callback_buffer_cut5(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-cut5",
               "gtk-cut",
               _("Cut into 5"),
               _("Cut into 5"),
               _("Cut into 5"),
               NULL,
               ACTUATE)
{
  i_buffer_cut (w_current, 5, action);
}

action_buffer_cut5 = gschem_action_register(, , , , , , , action_callback_buffer_cut5);


GschemAction *action_buffer_paste1;

void action_callback_buffer_paste1(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-paste1",
               "gtk-paste",
               _("Paste from 1"),
               _("Paste from 1"),
               _("Paste from 1"),
               NULL,
               ACTUATE)
{
  i_buffer_paste (w_current, 1, action);
}

action_buffer_paste1 = gschem_action_register(, , , , , , , action_callback_buffer_paste1);


GschemAction *action_buffer_paste2;

void action_callback_buffer_paste2(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-paste2",
               "gtk-paste",
               _("Paste from 2"),
               _("Paste from 2"),
               _("Paste from 2"),
               NULL,
               ACTUATE)
{
  i_buffer_paste (w_current, 2, action);
}

action_buffer_paste2 = gschem_action_register(, , , , , , , action_callback_buffer_paste2);


GschemAction *action_buffer_paste3;

void action_callback_buffer_paste3(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-paste3",
               "gtk-paste",
               _("Paste from 3"),
               _("Paste from 3"),
               _("Paste from 3"),
               NULL,
               ACTUATE)
{
  i_buffer_paste (w_current, 3, action);
}

action_buffer_paste3 = gschem_action_register(, , , , , , , action_callback_buffer_paste3);


GschemAction *action_buffer_paste4;

void action_callback_buffer_paste4(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-paste4",
               "gtk-paste",
               _("Paste from 4"),
               _("Paste from 4"),
               _("Paste from 4"),
               NULL,
               ACTUATE)
{
  i_buffer_paste (w_current, 4, action);
}

action_buffer_paste4 = gschem_action_register(, , , , , , , action_callback_buffer_paste4);


GschemAction *action_buffer_paste5;

void action_callback_buffer_paste5(GschemAction *action, GschemToplevel *w_current)
{
               "buffer-paste5",
               "gtk-paste",
               _("Paste from 5"),
               _("Paste from 5"),
               _("Paste from 5"),
               NULL,
               ACTUATE)
{
  i_buffer_paste (w_current, 5, action);
}

action_buffer_paste5 = gschem_action_register(, , , , , , , action_callback_buffer_paste5);


/*! \section special-actions Special Actions */

GschemAction *action_options_grid_size;

void action_callback_options_grid_size(GschemAction *action, GschemToplevel *w_current)
{
               "options-grid-size",
               NULL,
               _("Grid Size"),
               _("Grid Size"),
               _("Grid Size"),
               NULL,
               ACTUATE)
{
  /* grid size spinbox */
}

action_options_grid_size = gschem_action_register(, , , , , , , action_callback_options_grid_size);


GschemAction *action_options_grid;

void action_callback_options_grid(GschemAction *action, GschemToplevel *w_current)
{
               "options-grid",
               NULL,
               _("Switch Grid Style"),
               _("Cycle Grid Styles"),
               _("Cycle _Grid Styles"),
               NULL,
               ACTUATE)
{
  GRID_MODE grid_mode;

  gschem_options_cycle_grid_mode (w_current->options);

  grid_mode = gschem_options_get_grid_mode (w_current->options);

  switch (grid_mode) {
    case GRID_MODE_NONE: s_log_message (_("Grid OFF\n"));           break;
    case GRID_MODE_DOTS: s_log_message (_("Dot grid selected\n"));  break;
    case GRID_MODE_MESH: s_log_message (_("Mesh grid selected\n")); break;
    default:             s_log_message (_("Invalid grid mode\n"));
  }
}

action_options_grid = gschem_action_register(, , , , , , , action_callback_options_grid);


GschemAction *action_options_grid_none;

void action_callback_options_grid_none(GschemAction *action, GschemToplevel *w_current)
{
               "options-grid-none",
               NULL,
               _("No Grid"),
               _("No Grid"),
               _("No Grid"),
               NULL,
               TOGGLE_RADIO)
{
  gschem_options_set_grid_mode (w_current->options, GRID_MODE_NONE);
}

action_options_grid_none = gschem_action_register(, , , , , , , action_callback_options_grid_none);


GschemAction *action_options_grid_dots;

void action_callback_options_grid_dots(GschemAction *action, GschemToplevel *w_current)
{
               "options-grid-dots",
               NULL,
               _("Dot Grid"),
               _("Dot Grid"),
               _("Dot Grid"),
               NULL,
               TOGGLE_RADIO)
{
  gschem_options_set_grid_mode (w_current->options, GRID_MODE_DOTS);
}

action_options_grid_dots = gschem_action_register(, , , , , , , action_callback_options_grid_dots);


GschemAction *action_options_grid_mesh;

void action_callback_options_grid_mesh(GschemAction *action, GschemToplevel *w_current)
{
               "options-grid-mesh",
               NULL,
               _("Mesh Grid"),
               _("Mesh Grid"),
               _("Mesh Grid"),
               NULL,
               TOGGLE_RADIO)
{
  gschem_options_set_grid_mode (w_current->options, GRID_MODE_MESH);
}

action_options_grid_mesh = gschem_action_register(, , , , , , , action_callback_options_grid_mesh);


GschemAction *action_options_snap;

void action_callback_options_snap(GschemAction *action, GschemToplevel *w_current)
{
               "options-snap",
               NULL,
               _("Switch Snap Mode"),
               _("Cycle Snap Modes"),
               _("Cycle _Snap Modes"),
               NULL,
               ACTUATE)
{
  SNAP_STATE snap_mode;

  gschem_options_cycle_snap_mode (w_current->options);

  snap_mode = gschem_options_get_snap_mode (w_current->options);

  switch (snap_mode) {
  case SNAP_OFF:
    s_log_message(_("Snap OFF (CAUTION!)\n"));
    break;
  case SNAP_GRID:
    s_log_message(_("Snap ON\n"));
    break;
  case SNAP_RESNAP:
    s_log_message(_("Snap back to the grid (CAUTION!)\n"));
    break;
  default:
    g_critical("options_snap: toplevel->snap out of range: %d\n",
               snap_mode);
  }

  i_show_state(w_current, NULL); /* update status on screen */
  i_update_grid_info (w_current);
}

action_options_snap = gschem_action_register(, , , , , , , action_callback_options_snap);


/*! \brief Move the viewport to the left.
 *
 * The distance can be set with "keyboardpan-gain" scheme callback. */

GschemAction *action_view_pan_left;

void action_callback_view_pan_left(GschemAction *action, GschemToplevel *w_current)
{
               "view-pan-left",
               NULL,
               _("Pan Left"),
               _("Pan Left"),
               _("Pan Left"),
               NULL,
               ACTUATE)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  gschem_page_view_pan_mouse (page_view, w_current->keyboardpan_gain, 0);
}

action_view_pan_left = gschem_action_register(, , , , , , , action_callback_view_pan_left);


/*! \brief Move the viewport to the right.
 *
 * The distance can be set with "keyboardpan-gain" scheme callback. */

GschemAction *action_view_pan_right;

void action_callback_view_pan_right(GschemAction *action, GschemToplevel *w_current)
{
               "view-pan-right",
               NULL,
               _("Pan Right"),
               _("Pan Right"),
               _("Pan Right"),
               NULL,
               ACTUATE)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  /* yes, that's a negative sign there */
  gschem_page_view_pan_mouse (page_view, -w_current->keyboardpan_gain, 0);
}

action_view_pan_right = gschem_action_register(, , , , , , , action_callback_view_pan_right);


/*! \brief Move the viewport up.
 *
 * The distance can be set with "keyboardpan-gain" scheme callback. */

GschemAction *action_view_pan_up;

void action_callback_view_pan_up(GschemAction *action, GschemToplevel *w_current)
{
               "view-pan-up",
               NULL,
               _("Pan Up"),
               _("Pan Up"),
               _("Pan Up"),
               NULL,
               ACTUATE)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  gschem_page_view_pan_mouse (page_view, 0, w_current->keyboardpan_gain);
}

action_view_pan_up = gschem_action_register(, , , , , , , action_callback_view_pan_up);


/*! \brief Move the viewport down.
 *
 * The distance can be set with "keyboardpan-gain" scheme callback. */

GschemAction *action_view_pan_down;

void action_callback_view_pan_down(GschemAction *action, GschemToplevel *w_current)
{
               "view-pan-down",
               NULL,
               _("Pan Down"),
               _("Pan Down"),
               _("Pan Down"),
               NULL,
               ACTUATE)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  /* yes, that's a negative sign there */
  gschem_page_view_pan_mouse (page_view, 0, -w_current->keyboardpan_gain);
}

action_view_pan_down = gschem_action_register(, , , , , , , action_callback_view_pan_down);


GschemAction *action_edit_select;

void action_callback_edit_select(GschemAction *action, GschemToplevel *w_current)
{
               "edit-select",
               "select",
               _("Select"),
               _("Select"),
               _("Select Mode"),
               NULL,
               TOGGLE_PLAIN)
{
  /* Select also does not update the middle button shortcut. */
  o_redraw_cleanstates(w_current);

  /* this is probably the only place this should be */
  i_set_state(w_current, SELECT);
  i_action_stop (w_current);
}

action_edit_select = gschem_action_register(, , , , , , , action_callback_edit_select);


GschemAction *action_cancel;

void action_callback_cancel(GschemAction *action, GschemToplevel *w_current)
{
               "cancel",
               NULL,
               _("Cancel"),
               _("Cancel"),
               _("Cancel"),
               NULL,
               ACTUATE)
{
  i_cancel (w_current);
}

action_cancel = gschem_action_register(, , , , , , , action_callback_cancel);


GschemAction *action_page_print;

void action_callback_page_print(GschemAction *action, GschemToplevel *w_current)
{
               "page-print",
               "gtk-print",
               _("Print Page"),
               _("Print Page"),
               _("Print Page"),
               NULL,
               ACTUATE)
{
  s_page_print_all(gschem_toplevel_get_toplevel (w_current));
}

action_page_print = gschem_action_register(, , , , , , , action_callback_page_print);


/*! \brief Cause the last action executed to be repeated. */

GschemAction *action_repeat_last_action;

void action_callback_repeat_last_action(GschemAction *action, GschemToplevel *w_current)
{
               "repeat-last-action",
               "gtk-redo",
               _("Repeat Last Action"),
               _("Repeat Last Action"),
               _("Repeat Last Action"),
               NULL,
               ACTUATE)
{
  if (w_current->last_action != NULL)
    gschem_action_activate (w_current->last_action, w_current);
}

action_repeat_last_action = gschem_action_register(, , , , , , , action_callback_repeat_last_action);

