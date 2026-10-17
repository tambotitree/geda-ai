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
 * \brief Action registry definitions.
 *
 * Every action is described by one row of the ::action_specs table and one
 * callback function.  There is no preprocessor fan-out and no generated
 * intermediate file: this file is compiled directly.
 *
 * The table is generated from the historical DEFINE_ACTION definitions by
 * ``python3 -m gedai.refactor gen``; see gschem/src/README.md.  Edit the
 * table rather than re-running the generator once the refactor is closed.
 */

#include "config.h"

#include <glib/gi18n.h>

#include "gschem.h"
#include "gschem_action.h"
#include "actions.h"


/* Storage for the global action pointers.  These are the handles
 * other C files use to change an action's sensitivity or checked
 * state; they are filled in by gschem_register_all_actions(). */

GschemAction *action_file_new = NULL;
GschemAction *action_file_open = NULL;
GschemAction *action_file_open_recent = NULL;
GschemAction *action_file_save = NULL;
GschemAction *action_file_save_as = NULL;
GschemAction *action_file_save_all = NULL;
GschemAction *action_page_revert = NULL;
GschemAction *action_page_close = NULL;
GschemAction *action_file_print = NULL;
GschemAction *action_file_write_png = NULL;
GschemAction *action_file_new_window = NULL;
GschemAction *action_file_close = NULL;
GschemAction *action_file_quit = NULL;
GschemAction *action_edit_undo = NULL;
GschemAction *action_edit_redo = NULL;
GschemAction *action_clipboard_cut = NULL;
GschemAction *action_clipboard_copy = NULL;
GschemAction *action_clipboard_paste = NULL;
GschemAction *action_edit_delete = NULL;
GschemAction *action_edit_select_all = NULL;
GschemAction *action_edit_deselect = NULL;
GschemAction *action_edit_find = NULL;
GschemAction *action_add_component = NULL;
GschemAction *action_add_last_component = NULL;
GschemAction *action_add_net = NULL;
GschemAction *action_add_bus = NULL;
GschemAction *action_add_attribute = NULL;
GschemAction *action_add_text = NULL;
GschemAction *action_add_line = NULL;
GschemAction *action_add_path = NULL;
GschemAction *action_add_box = NULL;
GschemAction *action_add_circle = NULL;
GschemAction *action_add_arc = NULL;
GschemAction *action_add_pin = NULL;
GschemAction *action_add_picture = NULL;
GschemAction *action_edit_edit = NULL;
GschemAction *action_edit_properties = NULL;
GschemAction *action_edit_copy = NULL;
GschemAction *action_edit_mcopy = NULL;
GschemAction *action_edit_move = NULL;
GschemAction *action_edit_rotate_90 = NULL;
GschemAction *action_edit_mirror = NULL;
GschemAction *action_edit_text = NULL;
GschemAction *action_attributes_attach = NULL;
GschemAction *action_attributes_detach = NULL;
GschemAction *action_attributes_show_value = NULL;
GschemAction *action_attributes_show_name = NULL;
GschemAction *action_attributes_show_both = NULL;
GschemAction *action_attributes_visibility_toggle = NULL;
GschemAction *action_attributes_overbar_toggle = NULL;
GschemAction *action_edit_embed = NULL;
GschemAction *action_edit_unembed = NULL;
GschemAction *action_edit_update = NULL;
GschemAction *action_edit_lock = NULL;
GschemAction *action_edit_unlock = NULL;
GschemAction *action_edit_slot = NULL;
GschemAction *action_hierarchy_documentation = NULL;
GschemAction *action_edit_show_hidden = NULL;
GschemAction *action_view_show_origin = NULL;
GschemAction *action_view_zoom_in = NULL;
GschemAction *action_view_zoom_out = NULL;
GschemAction *action_view_zoom_extents = NULL;
GschemAction *action_view_zoom_full = NULL;
GschemAction *action_view_zoom_box = NULL;
GschemAction *action_view_pan = NULL;
GschemAction *action_view_redraw = NULL;
GschemAction *action_view_menubar = NULL;
GschemAction *action_view_toolbar = NULL;
GschemAction *action_view_scrollbars = NULL;
GschemAction *action_docking_area_left = NULL;
GschemAction *action_docking_area_bottom = NULL;
GschemAction *action_docking_area_right = NULL;
GschemAction *action_page_update_messages = NULL;
GschemAction *action_hierarchy_down_schematic = NULL;
GschemAction *action_hierarchy_down_symbol = NULL;
GschemAction *action_hierarchy_up = NULL;
GschemAction *action_page_prev = NULL;
GschemAction *action_page_next = NULL;
GschemAction *action_page_manager = NULL;
GschemAction *action_edit_autonumber_text = NULL;
GschemAction *action_edit_translate = NULL;
GschemAction *action_edit_find_patch = NULL;
GschemAction *action_edit_hide_text = NULL;
GschemAction *action_edit_show_text = NULL;
GschemAction *action_edit_select_locked = NULL;
GschemAction *action_options_show_log_window = NULL;
GschemAction *action_options_show_coordinates = NULL;
GschemAction *action_edit_invoke_macro = NULL;
GschemAction *action_file_script = NULL;
GschemAction *action_file_repl = NULL;
GschemAction *action_options_options = NULL;
GschemAction *action_options_rubberband = NULL;
GschemAction *action_options_magneticnet = NULL;
GschemAction *action_options_afeedback = NULL;
GschemAction *action_options_snap_off = NULL;
GschemAction *action_options_snap_grid = NULL;
GschemAction *action_options_snap_resnap = NULL;
GschemAction *action_options_scale_up_snap_size = NULL;
GschemAction *action_options_scale_down_snap_size = NULL;
GschemAction *action_view_dark_colors = NULL;
GschemAction *action_view_light_colors = NULL;
GschemAction *action_view_light_bw_colors = NULL;
GschemAction *action_view_shaded_colors = NULL;
GschemAction *action_view_shaded_bw_colors = NULL;
GschemAction *action_help_hotkeys = NULL;
GschemAction *action_help_about = NULL;
GschemAction *action_buffer_copy1 = NULL;
GschemAction *action_buffer_copy2 = NULL;
GschemAction *action_buffer_copy3 = NULL;
GschemAction *action_buffer_copy4 = NULL;
GschemAction *action_buffer_copy5 = NULL;
GschemAction *action_buffer_cut1 = NULL;
GschemAction *action_buffer_cut2 = NULL;
GschemAction *action_buffer_cut3 = NULL;
GschemAction *action_buffer_cut4 = NULL;
GschemAction *action_buffer_cut5 = NULL;
GschemAction *action_buffer_paste1 = NULL;
GschemAction *action_buffer_paste2 = NULL;
GschemAction *action_buffer_paste3 = NULL;
GschemAction *action_buffer_paste4 = NULL;
GschemAction *action_buffer_paste5 = NULL;
GschemAction *action_options_grid_size = NULL;
GschemAction *action_options_grid = NULL;
GschemAction *action_options_grid_none = NULL;
GschemAction *action_options_grid_dots = NULL;
GschemAction *action_options_grid_mesh = NULL;
GschemAction *action_options_snap = NULL;
GschemAction *action_view_pan_left = NULL;
GschemAction *action_view_pan_right = NULL;
GschemAction *action_view_pan_up = NULL;
GschemAction *action_view_pan_down = NULL;
GschemAction *action_edit_select = NULL;
GschemAction *action_cancel = NULL;
GschemAction *action_page_print = NULL;
GschemAction *action_repeat_last_action = NULL;

static const GschemActionSpec action_specs[] = {

  /* File Menu Callback Functions */
  { "file-new", "gtk-new", N_("New File"), N_("New"), N_("_New"), NULL, ACTUATE,
    action_callback_file_new, &action_file_new },
  { "file-open", "gtk-open", N_("Open File"), N_("Open..."), N_("_Open..."), NULL, ACTUATE,
    action_callback_file_open, &action_file_open },
  { "file-open-recent", NULL, N_("Open Recent File"), N_("Open Recent"), N_("Open Recen_t"), NULL, ACTUATE,
    action_callback_file_open_recent, &action_file_open_recent },
  { "file-save", "gtk-save", N_("Save"), N_("Save"), N_("_Save"), NULL, ACTUATE,
    action_callback_file_save, &action_file_save },
  { "file-save-as", "gtk-save-as", N_("Save As"), N_("Save As..."), N_("Save _As..."), NULL, ACTUATE,
    action_callback_file_save_as, &action_file_save_as },
  { "file-save-all", "gtk-save", N_("Save All"), N_("Save All"), N_("Save All"), NULL, ACTUATE,
    action_callback_file_save_all, &action_file_save_all },
  { "page-revert", "gtk-revert-to-saved", N_("Revert Changes"), N_("Revert"), N_("_Revert"), NULL, ACTUATE,
    action_callback_page_revert, &action_page_revert },
  { "page-close", "gtk-close", N_("Close Page"), N_("Close"), N_("_Close"), NULL, ACTUATE,
    action_callback_page_close, &action_page_close },
  { "file-print", "gtk-print", N_("Print"), N_("Print..."), N_("_Print..."), NULL, ACTUATE,
    action_callback_file_print, &action_file_print },
  { "file-image", NULL, N_("Export Image"), N_("Export Image..."), N_("Export _Image..."), NULL, ACTUATE,
    action_callback_file_write_png, &action_file_write_png },
  { "file-new-window", "window-new", N_("New Window"), N_("New Window"), N_("New Window"), NULL, ACTUATE,
    action_callback_file_new_window, &action_file_new_window },
  { "file-close-window", "gtk-close", N_("Close Window"), N_("Close Window"), N_("Close Window"), NULL, ACTUATE,
    action_callback_file_close, &action_file_close },
  { "file-quit", "gtk-quit", N_("Quit"), N_("Quit"), N_("_Quit"), NULL, ACTUATE,
    action_callback_file_quit, &action_file_quit },

  /* Edit Menu Callback Functions */
  { "edit-undo", "gtk-undo", N_("Undo"), N_("Undo"), N_("_Undo"), NULL, ACTUATE,
    action_callback_edit_undo, &action_edit_undo },
  { "edit-redo", "gtk-redo", N_("Redo"), N_("Redo"), N_("_Redo"), NULL, ACTUATE,
    action_callback_edit_redo, &action_edit_redo },
  { "clipboard-cut", "gtk-cut", N_("Cut"), N_("Cut"), N_("Cu_t"), NULL, ACTUATE,
    action_callback_clipboard_cut, &action_clipboard_cut },
  { "clipboard-copy", "gtk-copy", N_("Copy"), N_("Copy"), N_("_Copy"), NULL, ACTUATE,
    action_callback_clipboard_copy, &action_clipboard_copy },
  { "clipboard-paste", "gtk-paste", N_("Paste"), N_("Paste"), N_("_Paste"), NULL, ACTUATE,
    action_callback_clipboard_paste, &action_clipboard_paste },
  { "edit-delete", "gtk-delete", N_("Delete"), N_("Delete"), N_("_Delete"), NULL, ACTUATE,
    action_callback_edit_delete, &action_edit_delete },
  { "edit-select-all", "gtk-select-all", N_("Select All"), N_("Select All"), N_("Select _All"), NULL, ACTUATE,
    action_callback_edit_select_all, &action_edit_select_all },
  { "edit-deselect", "deselect", N_("Deselect"), N_("Deselect"), N_("Deselect"), NULL, ACTUATE,
    action_callback_edit_deselect, &action_edit_deselect },
  { "edit-find-text", "gtk-find", N_("Find Text"), N_("Find..."), N_("_Find..."), NULL, ACTUATE,
    action_callback_edit_find, &action_edit_find },

  /* Add Menu Callback Functions */
  { "add-component", "insert-symbol", N_("Component Library"), N_("Component Library..."), N_("_Component..."), NULL, TOGGLE_PLAIN,
    action_callback_add_component, &action_add_component },
  { "add-last-component", "insert-symbol", N_("Add Last Component"), N_("Add Last Component"), N_("La_st Component"), NULL, TOGGLE_PLAIN,
    action_callback_add_last_component, &action_add_last_component },
  { "add-net", "insert-net", N_("Add Net"), N_("Add Net"), N_("_Net"), NULL, TOGGLE_PLAIN,
    action_callback_add_net, &action_add_net },
  { "add-bus", "insert-bus", N_("Add Bus"), N_("Add Bus"), N_("B_us"), NULL, TOGGLE_PLAIN,
    action_callback_add_bus, &action_add_bus },
  { "add-attribute", "insert-attribute", N_("Add Attribute"), N_("Add Attribute..."), N_("_Attribute..."), NULL, ACTUATE,
    action_callback_add_attribute, &action_add_attribute },
  { "add-text", "insert-text", N_("Add Text"), N_("Add Text..."), N_("_Text..."), NULL, TOGGLE_PLAIN,
    action_callback_add_text, &action_add_text },
  { "add-line", "insert-line", N_("Add Line"), N_("Add Line"), N_("_Line"), NULL, TOGGLE_PLAIN,
    action_callback_add_line, &action_add_line },
  { "add-path", "insert-path", N_("Add Path"), N_("Add Path"), N_("Pat_h"), NULL, TOGGLE_PLAIN,
    action_callback_add_path, &action_add_path },
  { "add-box", "insert-box", N_("Add Box"), N_("Add Box"), N_("_Box"), NULL, TOGGLE_PLAIN,
    action_callback_add_box, &action_add_box },
  { "add-circle", "insert-circle", N_("Add Circle"), N_("Add Circle"), N_("C_ircle"), NULL, TOGGLE_PLAIN,
    action_callback_add_circle, &action_add_circle },
  { "add-arc", "insert-arc", N_("Add Arc"), N_("Add Arc"), N_("A_rc"), NULL, TOGGLE_PLAIN,
    action_callback_add_arc, &action_add_arc },
  { "add-pin", "insert-pin", N_("Add Pin"), N_("Add Pin"), N_("_Pin"), NULL, TOGGLE_PLAIN,
    action_callback_add_pin, &action_add_pin },
  { "add-picture", "insert-image", N_("Add Picture"), N_("Add Picture..."), N_("Pictu_re..."), NULL, TOGGLE_PLAIN,
    action_callback_add_picture, &action_add_picture },

  /* Object Menu Callback Functions */
  { "edit-edit", "gtk-edit", N_("Edit"), N_("Edit..."), N_("_Edit..."), NULL, ACTUATE,
    action_callback_edit_edit, &action_edit_edit },
  { "edit-properties", "gtk-properties", N_("Object Properties"), N_("Object Properties..."), N_("Object _Properties..."), NULL, ACTUATE,
    action_callback_edit_properties, &action_edit_properties },
  { "edit-copy", "clone", N_("Copy"), N_("Copy"), N_("_Copy Mode"), NULL, TOGGLE_PLAIN,
    action_callback_edit_copy, &action_edit_copy },
  { "edit-mcopy", "multi-clone", N_("Multiple Copy"), N_("Multiple Copy"), N_("Multiple Cop_y Mode"), NULL, TOGGLE_PLAIN,
    action_callback_edit_mcopy, &action_edit_mcopy },
  { "edit-move", NULL, N_("Move"), N_("Move"), N_("_Move Mode"), NULL, ACTUATE,
    action_callback_edit_move, &action_edit_move },
  { "edit-rotate-90", "object-rotate-left", N_("Rotate"), N_("Rotate"), N_("_Rotate 90 Mode"), NULL, TOGGLE_PLAIN,
    action_callback_edit_rotate_90, &action_edit_rotate_90 },
  { "edit-mirror", "object-flip-horizontal", N_("Mirror"), N_("Mirror"), N_("M_irror Mode"), NULL, TOGGLE_PLAIN,
    action_callback_edit_mirror, &action_edit_mirror },
  { "edit-text", "gtk-edit", N_("Edit Text"), N_("Edit Text..."), N_("Edit Te_xt..."), NULL, ACTUATE,
    action_callback_edit_text, &action_edit_text },
  { "attributes-attach", "attribute-attach", N_("Attach Attributes"), N_("Attach Attributes"), N_("_Attach Attributes"), NULL, ACTUATE,
    action_callback_attributes_attach, &action_attributes_attach },
  { "attributes-detach", "attribute-detach", N_("Detach Attributes"), N_("Detach Attributes"), N_("_Detach Attributes"), NULL, ACTUATE,
    action_callback_attributes_detach, &action_attributes_detach },
  { "attributes-show-value", "attribute-show-value", N_("Show Attribute Value"), N_("Show Attribute Value"), N_("Show Attribute _Value"), NULL, ACTUATE,
    action_callback_attributes_show_value, &action_attributes_show_value },
  { "attributes-show-name", "attribute-show-name", N_("Show Attribute Name"), N_("Show Attribute Name"), N_("Show Attribute _Name"), NULL, ACTUATE,
    action_callback_attributes_show_name, &action_attributes_show_name },
  { "attributes-show-both", "attribute-show-both", N_("Show Name & Value"), N_("Show Attribute Name & Value"), N_("Show Attribute Name _& Value"), NULL, ACTUATE,
    action_callback_attributes_show_both, &action_attributes_show_both },
  { "attributes-visibility-toggle", NULL, N_("Toggle Text Visibility"), N_("Toggle Text Visibility"), N_("_Toggle Text Visibility"), NULL, ACTUATE,
    action_callback_attributes_visibility_toggle, &action_attributes_visibility_toggle },
  { "attributes-overbar-toggle", NULL, N_("Toggle Text Overbar"), N_("Toggle Text Overbar"), N_("To_ggle Text Overbar"), NULL, ACTUATE,
    action_callback_attributes_overbar_toggle, &action_attributes_overbar_toggle },
  { "edit-embed", NULL, N_("Embed Symbol/Picture"), N_("Embed Symbol/Picture"), N_("Em_bed Symbol/Picture"), NULL, ACTUATE,
    action_callback_edit_embed, &action_edit_embed },
  { "edit-unembed", NULL, N_("Unembed Symbol/Picture"), N_("Unembed Symbol/Picture"), N_("Unembed Symbol/Picture"), NULL, ACTUATE,
    action_callback_edit_unembed, &action_edit_unembed },
  { "edit-update", "gtk-refresh", N_("Update Symbol"), N_("Update Symbol"), N_("_Update Symbol"), NULL, ACTUATE,
    action_callback_edit_update, &action_edit_update },
  { "edit-lock", NULL, N_("Lock"), N_("Lock Component"), N_("_Lock Component"), NULL, ACTUATE,
    action_callback_edit_lock, &action_edit_lock },
  { "edit-unlock", NULL, N_("Unlock"), N_("Unlock Component"), N_("Unloc_k Component"), NULL, ACTUATE,
    action_callback_edit_unlock, &action_edit_unlock },
  { "edit-slot", NULL, N_("Choose Slot"), N_("Slot..."), N_("_Slot..."), NULL, ACTUATE,
    action_callback_edit_slot, &action_edit_slot },
  { "hierarchy-documentation", "symbol-datasheet", N_("Part Documentation"), N_("Part Documentation..."), N_("Part D_ocumentation..."), N_("View documentation for selected component"), ACTUATE,
    action_callback_hierarchy_documentation, &action_hierarchy_documentation },

  /* View Menu Callback Functions */
  { "edit-show-hidden", NULL, N_("Show/Hide Invisible Text"), N_("Show Invisible Text"), N_("_Show Invisible Text"), NULL, TOGGLE_CHECK,
    action_callback_edit_show_hidden, &action_edit_show_hidden },
  { "view-show-origin", NULL, N_("Toggle Origin Visibility"), N_("Show Origin"), N_("Show Ori_gin"), NULL, TOGGLE_CHECK,
    action_callback_view_show_origin, &action_view_show_origin },
  { "view-zoom-in", "gtk-zoom-in", N_("Zoom In"), N_("Zoom In"), N_("Zoom _In"), NULL, ACTUATE,
    action_callback_view_zoom_in, &action_view_zoom_in },
  { "view-zoom-out", "gtk-zoom-out", N_("Zoom Out"), N_("Zoom Out"), N_("Zoom _Out"), NULL, ACTUATE,
    action_callback_view_zoom_out, &action_view_zoom_out },
  { "view-zoom-extents", "gtk-zoom-fit", N_("Zoom Extents"), N_("Zoom Extents"), N_("Zoom _Extents"), NULL, ACTUATE,
    action_callback_view_zoom_extents, &action_view_zoom_extents },
  { "view-zoom-full", NULL, N_("Zoom Full"), N_("Zoom Full"), N_("Zoom _Full"), NULL, ACTUATE,
    action_callback_view_zoom_full, &action_view_zoom_full },
  { "view-zoom-box", NULL, N_("Zoom Box"), N_("Zoom Box"), N_("Zoom Bo_x"), NULL, TOGGLE_PLAIN,
    action_callback_view_zoom_box, &action_view_zoom_box },
  { "view-pan", NULL, N_("Pan"), N_("Pan"), N_("_Pan"), NULL, TOGGLE_PLAIN,
    action_callback_view_pan, &action_view_pan },
  { "view-redraw", "gtk-refresh", N_("Redraw"), N_("Redraw"), N_("Re_draw"), NULL, ACTUATE,
    action_callback_view_redraw, &action_view_redraw },
  { "view-menubar", NULL, N_("Show/Hide Menubar"), N_("Show Menubar"), N_("Show _Menubar"), NULL, TOGGLE_CHECK,
    action_callback_view_menubar, &action_view_menubar },
  { "view-toolbar", NULL, N_("Show/Hide Toolbar"), N_("Show Toolbar"), N_("Show _Toolbar"), NULL, TOGGLE_CHECK,
    action_callback_view_toolbar, &action_view_toolbar },
  { "view-scrollbars", NULL, N_("Show/Hide Scrollbars"), N_("Show Scrollbars"), N_("Show S_crollbars"), NULL, TOGGLE_CHECK,
    action_callback_view_scrollbars, &action_view_scrollbars },
  { "docking-area-left", NULL, N_("Left Dock Windows"), N_("Left Dock"), N_("_Left Dock"), NULL, ACTUATE,
    action_callback_docking_area_left, &action_docking_area_left },
  { "docking-area-bottom", NULL, N_("Bottom Dock Windows"), N_("Bottom Dock"), N_("_Bottom Dock"), NULL, ACTUATE,
    action_callback_docking_area_bottom, &action_docking_area_bottom },
  { "docking-area-right", NULL, N_("Right Dock Windows"), N_("Right Dock"), N_("_Right Dock"), NULL, ACTUATE,
    action_callback_docking_area_right, &action_docking_area_right },

  /* Pages Menu Callback Functions */
  { "page-update-messages", "gtk-refresh", N_("Update Messages"), N_("Update Messages"), N_("_Update Messages"), NULL, ACTUATE,
    action_callback_page_update_messages, &action_page_update_messages },
  { "hierarchy-down-schematic", "gtk-go-down", N_("Down Schematic"), N_("Edit Subschematic"), N_("Edit _Subschematic"), NULL, ACTUATE,
    action_callback_hierarchy_down_schematic, &action_hierarchy_down_schematic },
  { "hierarchy-down-symbol", "gtk-goto-bottom", N_("Down Symbol"), N_("Edit Symbol"), N_("Edit S_ymbol"), NULL, ACTUATE,
    action_callback_hierarchy_down_symbol, &action_hierarchy_down_symbol },
  { "hierarchy-up", "gtk-go-up", N_("Up"), N_("Leave"), N_("_Leave Subschematic/Symbol"), NULL, ACTUATE,
    action_callback_hierarchy_up, &action_hierarchy_up },
  { "page-prev", "gtk-go-back", N_("Previous Page"), N_("Previous Page"), N_("_Previous Page"), NULL, ACTUATE,
    action_callback_page_prev, &action_page_prev },
  { "page-next", "gtk-go-forward", N_("Next Page"), N_("Next Page"), N_("_Next Page"), NULL, ACTUATE,
    action_callback_page_next, &action_page_next },
  { "page-manager", NULL, N_("Page Manager"), N_("Page Manager..."), N_("Page _Manager..."), NULL, ACTUATE,
    action_callback_page_manager, &action_page_manager },

  /* Tools Menu Callback Functions */
  { "edit-autonumber", NULL, N_("Autonumber Text"), N_("Autonumber..."), N_("A_utonumber..."), NULL, ACTUATE,
    action_callback_edit_autonumber_text, &action_edit_autonumber_text },
  { "edit-translate", NULL, N_("Reset Origin"), N_("Place Origin"), N_("_Place Origin"), NULL, TOGGLE_PLAIN,
    action_callback_edit_translate, &action_edit_translate },
  { "edit-find-patch", NULL, N_("Import Patch And Find Mismatches"), N_("Import Patch..."), N_("_Import Patch..."), NULL, ACTUATE,
    action_callback_edit_find_patch, &action_edit_find_patch },
  { "edit-hide-text", NULL, N_("Hide Specific Text"), N_("Hide Specific Text..."), N_("_Hide Specific Text..."), NULL, ACTUATE,
    action_callback_edit_hide_text, &action_edit_hide_text },
  { "edit-show-text", NULL, N_("Show Specific Text"), N_("Show Specific Text..."), N_("_Show Specific Text..."), NULL, ACTUATE,
    action_callback_edit_show_text, &action_edit_show_text },
  { "edit-select-locked", NULL, N_("Select Locked Objects"), N_("Select Locked Objects"), N_("Select Loc_ked Objects"), NULL, ACTUATE,
    action_callback_edit_select_locked, &action_edit_select_locked },
  { "options-show-log-window", NULL, N_("Show Log Window"), N_("Log Window..."), N_("_Log Window..."), NULL, ACTUATE,
    action_callback_options_show_log_window, &action_options_show_log_window },
  { "options-show-coordinates", NULL, N_("Show/Hide Coordinates"), N_("Show Coordinates"), N_("Show _Coordinates"), NULL, TOGGLE_CHECK,
    action_callback_options_show_coordinates, &action_options_show_coordinates },
  { "edit-invoke-macro", "gtk-execute", N_("Invoke Macro"), N_("Evaluate Guile Expression..."), N_("_Evaluate Guile Expression..."), NULL, ACTUATE,
    action_callback_edit_invoke_macro, &action_edit_invoke_macro },
  { "file-script", "gtk-execute", N_("Run Script"), N_("Execute Guile Script..."), N_("E_xecute Guile Script..."), NULL, ACTUATE,
    action_callback_file_script, &action_file_script },
  { "file-repl", "gtk-execute", N_("Terminal REPL"), N_("Start Guile REPL in Terminal..."), N_("Start Guile _REPL in Terminal..."), NULL, ACTUATE,
    action_callback_file_repl, &action_file_repl },

  /* Options Menu Callback Functions */
  { "options-options", "gtk-preferences", N_("Options"), N_("Options..."), N_("_Options..."), NULL, ACTUATE,
    action_callback_options_options, &action_options_options },
  { "options-rubberband", NULL, N_("Toggle Net Rubber Band"), N_("Net Rubberband Mode"), N_("Net _Rubberband Mode"), N_("When moving components or nets, drag the nets which are "
                 "attached to them"), TOGGLE_CHECK,
    action_callback_options_rubberband, &action_options_rubberband },
  { "options-magneticnet", NULL, N_("Toggle Magnetic Nets"), N_("Magnetic Net Mode"), N_("_Magnetic Net Mode"), N_("When adding nets, the endpoint snaps to the nearest net or "
                 "pin (indicated by a circle)"), TOGGLE_CHECK,
    action_callback_options_magneticnet, &action_options_magneticnet },
  { "options-action-feedback", NULL, N_("Toggle Bounding Box Drawing"), N_("Draw Bounding Box"), N_("Draw _Bounding Box"), N_("While moving objects, show their bounding box only "
                 "(speeds up drawing, may be useful for older machines)"), TOGGLE_CHECK,
    action_callback_options_afeedback, &action_options_afeedback },
  { "options-snap-off", NULL, N_("Snap Off"), N_("Snap Off"), N_("Snap Off"), NULL, TOGGLE_RADIO,
    action_callback_options_snap_off, &action_options_snap_off },
  { "options-snap-grid", NULL, N_("Snap On"), N_("Snap On"), N_("Snap On"), N_("When moving off-grid points, translate them by multiples of "
                 "the grid but preserve their offset relative to the grid"), TOGGLE_RADIO,
    action_callback_options_snap_grid, &action_options_snap_grid },
  { "options-snap-resnap", NULL, N_("Snap Back to Nearest Grid Point"), N_("Snap Back to Nearest Grid Point"), N_("Snap Back to Nearest Grid Point"), N_("When moving off-grid objects, snap them to the nearest "
                 "location on the grid"), TOGGLE_RADIO,
    action_callback_options_snap_resnap, &action_options_snap_resnap },
  { "options-scale-up-snap-size", NULL, N_("Increase Grid Spacing"), N_("Scale up Grid Spacing"), N_("Scale _up Grid Spacing"), NULL, ACTUATE,
    action_callback_options_scale_up_snap_size, &action_options_scale_up_snap_size },
  { "options-scale-down-snap-size", NULL, N_("Decrease Grid Spacing"), N_("Scale down Grid Spacing"), N_("Scale _down Grid Spacing"), NULL, ACTUATE,
    action_callback_options_scale_down_snap_size, &action_options_scale_down_snap_size },
  { "view-dark-colors", NULL, N_("Dark Color Scheme"), N_("Dark Color Scheme"), N_("D_ark Color Scheme"), NULL, ACTUATE,
    action_callback_view_dark_colors, &action_view_dark_colors },
  { "view-light-colors", NULL, N_("Light Color Scheme"), N_("Light Color Scheme"), N_("_Light Color Scheme"), NULL, ACTUATE,
    action_callback_view_light_colors, &action_view_light_colors },
  { "view-light-bw-colors", NULL, N_("Light Monochrome Color Scheme"), N_("Light Color Scheme (B&W)"), N_("Light Color Scheme (B&_W)"), NULL, ACTUATE,
    action_callback_view_light_bw_colors, &action_view_light_bw_colors },
  { "view-shaded-colors", NULL, N_("Shaded Color Scheme"), N_("Shaded Color Scheme"), N_("S_haded Color Scheme"), NULL, ACTUATE,
    action_callback_view_shaded_colors, &action_view_shaded_colors },
  { "view-shaded-bw-colors", NULL, N_("Shaded Monochrome Color Scheme"), N_("Shaded Color Scheme (B&W)"), N_("Shaded Color Scheme (B&W)"), NULL, ACTUATE,
    action_callback_view_shaded_bw_colors, &action_view_shaded_bw_colors },

  /* Help Menu Callback Functions */
  { "help-hotkeys", "preferences-desktop-keyboard-shortcuts", N_("Show Hotkeys"), N_("Hotkeys..."), N_("_Hotkeys..."), NULL, ACTUATE,
    action_callback_help_hotkeys, &action_help_hotkeys },
  { "help-about", "gtk-about", N_("About gschem"), N_("About..."), N_("_About..."), NULL, ACTUATE,
    action_callback_help_about, &action_help_about },

  /* Buffer Menu Callback Functions */
  { "buffer-copy1", "gtk-copy", N_("Copy into 1"), N_("Copy into 1"), N_("Copy into 1"), NULL, ACTUATE,
    action_callback_buffer_copy1, &action_buffer_copy1 },
  { "buffer-copy2", "gtk-copy", N_("Copy into 2"), N_("Copy into 2"), N_("Copy into 2"), NULL, ACTUATE,
    action_callback_buffer_copy2, &action_buffer_copy2 },
  { "buffer-copy3", "gtk-copy", N_("Copy into 3"), N_("Copy into 3"), N_("Copy into 3"), NULL, ACTUATE,
    action_callback_buffer_copy3, &action_buffer_copy3 },
  { "buffer-copy4", "gtk-copy", N_("Copy into 4"), N_("Copy into 4"), N_("Copy into 4"), NULL, ACTUATE,
    action_callback_buffer_copy4, &action_buffer_copy4 },
  { "buffer-copy5", "gtk-copy", N_("Copy into 5"), N_("Copy into 5"), N_("Copy into 5"), NULL, ACTUATE,
    action_callback_buffer_copy5, &action_buffer_copy5 },
  { "buffer-cut1", "gtk-cut", N_("Cut into 1"), N_("Cut into 1"), N_("Cut into 1"), NULL, ACTUATE,
    action_callback_buffer_cut1, &action_buffer_cut1 },
  { "buffer-cut2", "gtk-cut", N_("Cut into 2"), N_("Cut into 2"), N_("Cut into 2"), NULL, ACTUATE,
    action_callback_buffer_cut2, &action_buffer_cut2 },
  { "buffer-cut3", "gtk-cut", N_("Cut into 3"), N_("Cut into 3"), N_("Cut into 3"), NULL, ACTUATE,
    action_callback_buffer_cut3, &action_buffer_cut3 },
  { "buffer-cut4", "gtk-cut", N_("Cut into 4"), N_("Cut into 4"), N_("Cut into 4"), NULL, ACTUATE,
    action_callback_buffer_cut4, &action_buffer_cut4 },
  { "buffer-cut5", "gtk-cut", N_("Cut into 5"), N_("Cut into 5"), N_("Cut into 5"), NULL, ACTUATE,
    action_callback_buffer_cut5, &action_buffer_cut5 },
  { "buffer-paste1", "gtk-paste", N_("Paste from 1"), N_("Paste from 1"), N_("Paste from 1"), NULL, ACTUATE,
    action_callback_buffer_paste1, &action_buffer_paste1 },
  { "buffer-paste2", "gtk-paste", N_("Paste from 2"), N_("Paste from 2"), N_("Paste from 2"), NULL, ACTUATE,
    action_callback_buffer_paste2, &action_buffer_paste2 },
  { "buffer-paste3", "gtk-paste", N_("Paste from 3"), N_("Paste from 3"), N_("Paste from 3"), NULL, ACTUATE,
    action_callback_buffer_paste3, &action_buffer_paste3 },
  { "buffer-paste4", "gtk-paste", N_("Paste from 4"), N_("Paste from 4"), N_("Paste from 4"), NULL, ACTUATE,
    action_callback_buffer_paste4, &action_buffer_paste4 },
  { "buffer-paste5", "gtk-paste", N_("Paste from 5"), N_("Paste from 5"), N_("Paste from 5"), NULL, ACTUATE,
    action_callback_buffer_paste5, &action_buffer_paste5 },

  /* Special Actions */
  { "options-grid-size", NULL, N_("Grid Size"), N_("Grid Size"), N_("Grid Size"), NULL, ACTUATE,
    action_callback_options_grid_size, &action_options_grid_size },
  { "options-grid", NULL, N_("Switch Grid Style"), N_("Cycle Grid Styles"), N_("Cycle _Grid Styles"), NULL, ACTUATE,
    action_callback_options_grid, &action_options_grid },
  { "options-grid-none", NULL, N_("No Grid"), N_("No Grid"), N_("No Grid"), NULL, TOGGLE_RADIO,
    action_callback_options_grid_none, &action_options_grid_none },
  { "options-grid-dots", NULL, N_("Dot Grid"), N_("Dot Grid"), N_("Dot Grid"), NULL, TOGGLE_RADIO,
    action_callback_options_grid_dots, &action_options_grid_dots },
  { "options-grid-mesh", NULL, N_("Mesh Grid"), N_("Mesh Grid"), N_("Mesh Grid"), NULL, TOGGLE_RADIO,
    action_callback_options_grid_mesh, &action_options_grid_mesh },
  { "options-snap", NULL, N_("Switch Snap Mode"), N_("Cycle Snap Modes"), N_("Cycle _Snap Modes"), NULL, ACTUATE,
    action_callback_options_snap, &action_options_snap },
  { "view-pan-left", NULL, N_("Pan Left"), N_("Pan Left"), N_("Pan Left"), NULL, ACTUATE,
    action_callback_view_pan_left, &action_view_pan_left },
  { "view-pan-right", NULL, N_("Pan Right"), N_("Pan Right"), N_("Pan Right"), NULL, ACTUATE,
    action_callback_view_pan_right, &action_view_pan_right },
  { "view-pan-up", NULL, N_("Pan Up"), N_("Pan Up"), N_("Pan Up"), NULL, ACTUATE,
    action_callback_view_pan_up, &action_view_pan_up },
  { "view-pan-down", NULL, N_("Pan Down"), N_("Pan Down"), N_("Pan Down"), NULL, ACTUATE,
    action_callback_view_pan_down, &action_view_pan_down },
  { "edit-select", "select", N_("Select"), N_("Select"), N_("Select Mode"), NULL, TOGGLE_PLAIN,
    action_callback_edit_select, &action_edit_select },
  { "cancel", NULL, N_("Cancel"), N_("Cancel"), N_("Cancel"), NULL, ACTUATE,
    action_callback_cancel, &action_cancel },
  { "page-print", "gtk-print", N_("Print Page"), N_("Print Page"), N_("Print Page"), NULL, ACTUATE,
    action_callback_page_print, &action_page_print },
  { "repeat-last-action", "gtk-redo", N_("Repeat Last Action"), N_("Repeat Last Action"), N_("Repeat Last Action"), NULL, ACTUATE,
    action_callback_repeat_last_action, &action_repeat_last_action },
};


/*! \brief Register every C-defined gschem action.
 *
 * Walks ::action_specs, registering each action and storing the resulting
 * object in the matching ``action_<cid>`` global so that other C files can
 * change its sensitivity or checked state.
 */
void
gschem_register_all_actions (void)
{
  for (gsize i = 0; i < G_N_ELEMENTS (action_specs); i++) {
    const GschemActionSpec *spec = &action_specs[i];
    GschemAction *action = gschem_action_register (spec->id,
                                                   spec->icon_name,
                                                   _(spec->name),
                                                   _(spec->label),
                                                   _(spec->menu_label),
                                                   spec->tooltip,
                                                   spec->type,
                                                   spec->activate);
    *spec->action = action;
  }
}


/*! \section callback-functions Callback Functions */


/*! \section file-menu File Menu Callback Functions */


void
action_callback_file_new (GschemAction *action,
                         GschemToplevel *w_current)
{
  /*! \todo Perhaps this should be renamed to page_new... */
  PAGE *page;

  /* create a new page */
  page = x_highlevel_new_page (w_current, NULL);
  g_return_if_fail (page != NULL);

  s_log_message (_("New page created [%s]\n"), page->page_filename);
}


void
action_callback_file_open (GschemAction *action,
                          GschemToplevel *w_current)
{
  /*! \todo Perhaps this should be renamed to page_open... */
  x_fileselect_open (w_current);
}


void
action_callback_file_open_recent (GschemAction *action,
                                 GschemToplevel *w_current)
{
  /* submenu */
}


void
action_callback_file_save (GschemAction *action,
                          GschemToplevel *w_current)
{
  x_highlevel_save_page (w_current, NULL);
}


void
action_callback_file_save_as (GschemAction *action,
                             GschemToplevel *w_current)
{
  x_fileselect_save (w_current);
}


void
action_callback_file_save_all (GschemAction *action,
                              GschemToplevel *w_current)
{
  x_highlevel_save_all (w_current);
}


void
action_callback_page_revert (GschemAction *action,
                            GschemToplevel *w_current)
{
  x_highlevel_revert_page (w_current, NULL);
}


void
action_callback_page_close (GschemAction *action,
                           GschemToplevel *w_current)
{
  x_highlevel_close_page (w_current, NULL);
}


void
action_callback_file_print (GschemAction *action,
                           GschemToplevel *w_current)
{
  x_print (w_current);
}


void
action_callback_file_write_png (GschemAction *action,
                               GschemToplevel *w_current)
{
  x_image_setup(w_current);
}


void
action_callback_file_new_window (GschemAction *action,
                                GschemToplevel *w_current)
{
  GschemToplevel *w_current_new = NULL;
  PAGE *page = NULL;

  w_current_new = x_window_new (NULL);
  g_return_if_fail (w_current_new != NULL);

  page = x_highlevel_new_page (w_current_new, NULL);
  g_return_if_fail (page != NULL);

  s_log_message (_("New Window created [%s]\n"), page->page_filename);
}


/*! \brief Close a window. */


void
action_callback_file_close (GschemAction *action,
                           GschemToplevel *w_current)
{
  s_log_message(_("Closing Window\n"));
  x_window_close(w_current);
}


void
action_callback_file_quit (GschemAction *action,
                          GschemToplevel *w_current)
{
  x_window_close_all(w_current);
}


/*! \section edit-menu Edit Menu Callback Functions */


void
action_callback_edit_undo (GschemAction *action,
                          GschemToplevel *w_current)
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


void
action_callback_edit_redo (GschemAction *action,
                          GschemToplevel *w_current)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  PAGE *page = gschem_page_view_get_page (page_view);

  if (page != NULL) {
    o_undo_callback (w_current, page, REDO_ACTION);
  }
}


/*! \brief Cut selection to clipboard, via buffer 0. */


void
action_callback_clipboard_cut (GschemAction *action,
                              GschemToplevel *w_current)
{
  if (!o_select_selected (w_current)) return;

  i_update_middle_button (w_current, action, _("Cut to clipboard"));

  o_redraw_cleanstates(w_current);
  o_buffer_cut (w_current, CLIPBOARD_BUFFER);
}


/*! \brief Copy selection to clipboard, via buffer 0. */


void
action_callback_clipboard_copy (GschemAction *action,
                               GschemToplevel *w_current)
{
  if (!o_select_selected (w_current)) return;

  i_update_middle_button (w_current, action, _("Copy to clipboard"));

  o_buffer_copy (w_current, CLIPBOARD_BUFFER);
}


/*! \brief Start pasting clipboard contents, via buffer 0. */


void
action_callback_clipboard_paste (GschemAction *action,
                                GschemToplevel *w_current)
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


void
action_callback_edit_delete (GschemAction *action,
                            GschemToplevel *w_current)
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


/*! \brief Select all objects on page. */


void
action_callback_edit_select_all (GschemAction *action,
                                GschemToplevel *w_current)
{
  o_redraw_cleanstates (w_current);

  o_select_visible_unlocked (w_current);

  i_set_state (w_current, SELECT);
  i_action_stop (w_current);
  i_update_menus (w_current);
}


/*! \brief Deselect all objects on page. */


void
action_callback_edit_deselect (GschemAction *action,
                              GschemToplevel *w_current)
{
  o_redraw_cleanstates (w_current);

  o_select_unselect_all (w_current);

  i_set_state (w_current, SELECT);
  i_action_stop (w_current);
  i_update_menus (w_current);
}


void
action_callback_edit_find (GschemAction *action,
                          GschemToplevel *w_current)
{
  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action)
    return;

  find_text_dialog(w_current);
}


/*! \section add-menu Add Menu Callback Functions */


void
action_callback_add_component (GschemAction *action,
                              GschemToplevel *w_current)
{
  o_redraw_cleanstates (w_current);

  i_set_state(w_current, COMPMODE);
  gschem_dockable_present (w_current->compselect_dockable);

  i_update_middle_button (w_current, action, _("Component"));

  i_set_state(w_current, SELECT);
}


void
action_callback_add_last_component (GschemAction *action,
                                   GschemToplevel *w_current)
{
  GschemPageView *page_view =
    gschem_toplevel_get_current_page_view (w_current);

  x_compselect_select_previous_symbol (w_current);
  x_event_faked_motion (page_view, NULL);
}


void
action_callback_add_net (GschemAction *action,
                        GschemToplevel *w_current)
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


void
action_callback_add_bus (GschemAction *action,
                        GschemToplevel *w_current)
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


void
action_callback_add_attribute (GschemAction *action,
                              GschemToplevel *w_current)
{
  attrib_edit_dialog(w_current, NULL,
                     g_action_get_position (TRUE, NULL, NULL) ? FROM_HOTKEY : FROM_MENU);
  i_update_middle_button (w_current, action, _("Attribute"));

  i_set_state(w_current, SELECT);
}


void
action_callback_add_text (GschemAction *action,
                         GschemToplevel *w_current)
{
  o_redraw_cleanstates(w_current);
  o_invalidate_rubber (w_current);

  i_action_stop (w_current);
  i_set_state(w_current, SELECT);
  i_update_middle_button (w_current, action, _("Text"));

  text_input_dialog(w_current);
}


void
action_callback_add_line (GschemAction *action,
                         GschemToplevel *w_current)
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


void
action_callback_add_path (GschemAction *action,
                         GschemToplevel *w_current)
{
  g_assert (w_current != NULL);

  o_redraw_cleanstates (w_current);
  o_invalidate_rubber (w_current);

  i_set_state (w_current, PATHMODE);
  i_update_middle_button (w_current, action, _("Path"));

  /* Don't start path here since setting of its first point and
   * control point requires the left button click and release */
}


void
action_callback_add_box (GschemAction *action,
                        GschemToplevel *w_current)
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


void
action_callback_add_circle (GschemAction *action,
                           GschemToplevel *w_current)
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


void
action_callback_add_arc (GschemAction *action,
                        GschemToplevel *w_current)
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


void
action_callback_add_pin (GschemAction *action,
                        GschemToplevel *w_current)
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


void
action_callback_add_picture (GschemAction *action,
                            GschemToplevel *w_current)
{
  o_redraw_cleanstates(w_current);
  o_invalidate_rubber (w_current);

  i_set_state(w_current, SELECT);

  picture_selection_dialog(w_current);
}


/*! \brief Rotate all objects in the selection list by 90 degrees. */


void
action_callback_edit_edit (GschemAction *action,
                          GschemToplevel *w_current)
{
  i_update_middle_button (w_current, action, _("Edit"));
  o_edit (w_current, geda_list_get_glist (gschem_toplevel_get_toplevel (w_current)->page_current->selection_list), FALSE);
}


void
action_callback_edit_properties (GschemAction *action,
                                GschemToplevel *w_current)
{
  i_update_middle_button (w_current, action, _("Properties"));

  /* dialogs have been merged */
  gschem_dockable_present (w_current->object_properties_dockable);
}


void
action_callback_edit_copy (GschemAction *action,
                          GschemToplevel *w_current)
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


void
action_callback_edit_mcopy (GschemAction *action,
                           GschemToplevel *w_current)
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


void
action_callback_edit_move (GschemAction *action,
                          GschemToplevel *w_current)
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


void
action_callback_edit_rotate_90 (GschemAction *action,
                               GschemToplevel *w_current)
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


void
action_callback_edit_mirror (GschemAction *action,
                            GschemToplevel *w_current)
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


void
action_callback_edit_text (GschemAction *action,
                          GschemToplevel *w_current)
{
  i_update_middle_button (w_current, action, _("Edit Text"));

  gschem_dockable_present (w_current->text_properties_dockable);
}


void
action_callback_attributes_attach (GschemAction *action,
                                  GschemToplevel *w_current)
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


void
action_callback_attributes_detach (GschemAction *action,
                                  GschemToplevel *w_current)
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


void
action_callback_attributes_show_value (GschemAction *action,
                                      GschemToplevel *w_current)
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


void
action_callback_attributes_show_name (GschemAction *action,
                                     GschemToplevel *w_current)
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


void
action_callback_attributes_show_both (GschemAction *action,
                                     GschemToplevel *w_current)
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


void
action_callback_attributes_visibility_toggle (GschemAction *action,
                                             GschemToplevel *w_current)
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


void
action_callback_attributes_overbar_toggle (GschemAction *action,
                                          GschemToplevel *w_current)
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


/*! \brief Embed all objects in selection list. */


void
action_callback_edit_embed (GschemAction *action,
                           GschemToplevel *w_current)
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


/*! \brief Unembed all objects in selection list. */


void
action_callback_edit_unembed (GschemAction *action,
                             GschemToplevel *w_current)
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


/*! \brief Update components. */


void
action_callback_edit_update (GschemAction *action,
                            GschemToplevel *w_current)
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


/*! \brief Lock all objects in selection list. */


void
action_callback_edit_lock (GschemAction *action,
                          GschemToplevel *w_current)
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


/*! \brief Unlock all objects in selection list. */


void
action_callback_edit_unlock (GschemAction *action,
                            GschemToplevel *w_current)
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


void
action_callback_edit_slot (GschemAction *action,
                          GschemToplevel *w_current)
{
  OBJECT *object;

  object = o_select_return_first_object(w_current);

  i_update_middle_button (w_current, action, _("Slot"));
  if (object) {
    o_slot_start(w_current, object);
  }
}


void
action_callback_hierarchy_documentation (GschemAction *action,
                                        GschemToplevel *w_current)
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


/*! \section view-menu View Menu Callback Functions */


void
action_callback_edit_show_hidden (GschemAction *action,
                                 GschemToplevel *w_current)
{
  i_update_middle_button (w_current, action, _("ShowHidden"));

  o_edit_show_hidden (w_current,
                      s_page_objects (gschem_toplevel_get_toplevel (w_current)->page_current));
}


void
action_callback_view_show_origin (GschemAction *action,
                                 GschemToplevel *w_current)
{
  gschem_options_cycle_show_origin (w_current->options);
  i_show_state (w_current, NULL);
}


void
action_callback_view_zoom_in (GschemAction *action,
                             GschemToplevel *w_current)
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


void
action_callback_view_zoom_out (GschemAction *action,
                              GschemToplevel *w_current)
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


void
action_callback_view_zoom_extents (GschemAction *action,
                                  GschemToplevel *w_current)
{
  /* repeat middle shortcut would get into the way of what user is try to do */
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  gschem_page_view_zoom_extents (page_view, NULL);

  if (w_current->undo_panzoom) {
    o_undo_savestate_old (w_current, UNDO_VIEWPORT_ONLY, _("Zoom Extents"));
  }
}


void
action_callback_view_zoom_full (GschemAction *action,
                               GschemToplevel *w_current)
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


void
action_callback_view_zoom_box (GschemAction *action,
                              GschemToplevel *w_current)
{
  /* repeat middle shortcut would get into the way of what user is try to do */
  gint wx, wy;

  o_redraw_cleanstates(w_current);

  i_set_state(w_current, ZOOMBOX);

  if (g_action_get_position (FALSE, &wx, &wy)) {
    a_zoom_box_start(w_current, wx, wy);
  }
}


void
action_callback_view_pan (GschemAction *action,
                         GschemToplevel *w_current)
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


void
action_callback_view_redraw (GschemAction *action,
                            GschemToplevel *w_current)
{
  /* repeat middle shortcut doesn't make sense on redraw,
     just hit right button */
  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}


/*! \brief Toggle visibility of the menu bar. */


void
action_callback_view_menubar (GschemAction *action,
                             GschemToplevel *w_current)
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

  menu_item = gtk_menu_item_new ();
  g_object_set_data (G_OBJECT (menu_item), "temporary-item", action);
  gtk_menu_shell_append (GTK_MENU_SHELL (w_current->popup_menu), menu_item);
  gtk_widget_show (menu_item);

  menu_item = gschem_action_create_menu_item (action, FALSE, w_current);
  g_object_set_data (G_OBJECT (menu_item), "temporary-item", action);
  gtk_menu_shell_append (GTK_MENU_SHELL (w_current->popup_menu), menu_item);
  gtk_widget_show (menu_item);
}


/*! \brief Toggle visibility of the toolbar. */


void
action_callback_view_toolbar (GschemAction *action,
                             GschemToplevel *w_current)
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


void
action_callback_view_scrollbars (GschemAction *action,
                                GschemToplevel *w_current)
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


void
action_callback_docking_area_left (GschemAction *action,
                                  GschemToplevel *w_current)
{
  /* submenu */
}


void
action_callback_docking_area_bottom (GschemAction *action,
                                    GschemToplevel *w_current)
{
  /* submenu */
}


void
action_callback_docking_area_right (GschemAction *action,
                                   GschemToplevel *w_current)
{
  /* submenu */
}


/*! \section pages-menu Pages Menu Callback Functions */


void
action_callback_page_update_messages (GschemAction *action,
                                     GschemToplevel *w_current)
{
  x_messages_update (w_current);
}


void
action_callback_hierarchy_down_schematic (GschemAction *action,
                                         GschemToplevel *w_current)
{
  OBJECT *object = o_select_return_first_object (w_current);

  if (object != NULL)
    x_hierarchy_down_schematic (w_current, object);
}


void
action_callback_hierarchy_down_symbol (GschemAction *action,
                                      GschemToplevel *w_current)
{
  OBJECT *object = o_select_return_first_object (w_current);

  if (object != NULL)
    x_hierarchy_down_symbol (w_current, object);
}


/*! \brief Return to the parent page in the hierarchy of schematics. */


void
action_callback_hierarchy_up (GschemAction *action,
                             GschemToplevel *w_current)
{
  x_hierarchy_up (w_current);
}


void
action_callback_page_prev (GschemAction *action,
                          GschemToplevel *w_current)
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


void
action_callback_page_next (GschemAction *action,
                          GschemToplevel *w_current)
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


void
action_callback_page_manager (GschemAction *action,
                             GschemToplevel *w_current)
{
  gschem_dockable_present (w_current->pagesel_dockable);
}


/*! \section tools-menu Tools Menu Callback Functions */


void
action_callback_edit_autonumber_text (GschemAction *action,
                                     GschemToplevel *w_current)
{
  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action)
    return;

  autonumber_text_dialog(w_current);
}


void
action_callback_edit_translate (GschemAction *action,
                               GschemToplevel *w_current)
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


void
action_callback_edit_find_patch (GschemAction *action,
                                GschemToplevel *w_current)
{
  x_patch_import (w_current);
}


void
action_callback_edit_hide_text (GschemAction *action,
                               GschemToplevel *w_current)
{
  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action)
    return;

  hide_text_dialog(w_current);
}


void
action_callback_edit_show_text (GschemAction *action,
                               GschemToplevel *w_current)
{
  /* This is a new addition 3/15 to prevent this from executing
   * inside an action */
  if (w_current->inside_action)
    return;

  show_text_dialog(w_current);
}


void
action_callback_edit_select_locked (GschemAction *action,
                                   GschemToplevel *w_current)
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


void
action_callback_options_show_log_window (GschemAction *action,
                                        GschemToplevel *w_current)
{
  gschem_dockable_present (w_current->log_dockable);
}


void
action_callback_options_show_coordinates (GschemAction *action,
                                         GschemToplevel *w_current)
{
  GschemBottomWidget *bottom_widget =
    GSCHEM_BOTTOM_WIDGET (w_current->bottom_widget);
  gboolean do_show_coords =
    gtk_widget_get_visible (bottom_widget->coord_label) == FALSE;

  gtk_widget_set_visible (bottom_widget->coord_separator, do_show_coords);
  gtk_widget_set_visible (bottom_widget->coord_label, do_show_coords);

  gschem_action_set_active (action, do_show_coords, w_current);
}


void
action_callback_edit_invoke_macro (GschemAction *action,
                                  GschemToplevel *w_current)
{
  gtk_widget_show (w_current->macro_widget);
  gtk_widget_grab_focus (gschem_macro_widget_get_entry (GSCHEM_MACRO_WIDGET (w_current->macro_widget)));
}


void
action_callback_file_script (GschemAction *action,
                            GschemToplevel *w_current)
{
  setup_script_selector(w_current);
}


void
action_callback_file_repl (GschemAction *action,
                          GschemToplevel *w_current)
{
  g_scm_eval_protected (scm_list_1 (
                          scm_variable_ref (
                            scm_c_public_variable (
                              "gschem repl",
                              "start-repl-in-background-terminal"))),
                        SCM_UNDEFINED);
}


/*! \section options-menu Options Menu Callback Functions */


void
action_callback_options_options (GschemAction *action,
                                GschemToplevel *w_current)
{
  gschem_dockable_present (w_current->options_dockable);
}


void
action_callback_options_rubberband (GschemAction *action,
                                   GschemToplevel *w_current)
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


/*! \brief Toggle magnetic net mode option ON and OFF. */


void
action_callback_options_magneticnet (GschemAction *action,
                                    GschemToplevel *w_current)
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


void
action_callback_options_afeedback (GschemAction *action,
                                  GschemToplevel *w_current)
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


void
action_callback_options_snap_off (GschemAction *action,
                                 GschemToplevel *w_current)
{
  gschem_options_set_snap_mode (w_current->options, SNAP_OFF);

  i_show_state (w_current, NULL); /* update status on screen */
  i_update_grid_info (w_current);
}


void
action_callback_options_snap_grid (GschemAction *action,
                                  GschemToplevel *w_current)
{
  gschem_options_set_snap_mode (w_current->options, SNAP_GRID);

  i_show_state (w_current, NULL); /* update status on screen */
  i_update_grid_info (w_current);
}


void
action_callback_options_snap_resnap (GschemAction *action,
                                    GschemToplevel *w_current)
{
  gschem_options_set_snap_mode (w_current->options, SNAP_RESNAP);

  i_show_state (w_current, NULL); /* update status on screen */
  i_update_grid_info (w_current);
}


/*! \brief Multiply snap grid size by two. */


void
action_callback_options_scale_up_snap_size (GschemAction *action,
                                           GschemToplevel *w_current)
{
  gschem_options_scale_snap_up (w_current->options);
}


/*! \brief Divide snap grid size by two (if it's and even number). */


void
action_callback_options_scale_down_snap_size (GschemAction *action,
                                             GschemToplevel *w_current)
{
  gschem_options_scale_snap_down (w_current->options);
}


void
action_callback_view_dark_colors (GschemAction *action,
                                 GschemToplevel *w_current)
{
  x_color_free ();
  /* Change the scheme here */
  g_scm_c_eval_string_protected ("(load (build-path geda-rc-path \"gschem-colormap-darkbg\"))");
  x_color_allocate ();

  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}


void
action_callback_view_light_colors (GschemAction *action,
                                  GschemToplevel *w_current)
{
  x_color_free ();
  /* Change the scheme here */
  g_scm_c_eval_string_protected ("(load (build-path geda-rc-path \"gschem-colormap-whitebg\"))");
  x_color_allocate ();

  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}


void
action_callback_view_light_bw_colors (GschemAction *action,
                                     GschemToplevel *w_current)
{
  x_color_free ();
  /* Change the scheme here */
  g_scm_c_eval_string_protected ("(load (build-path geda-rc-path \"gschem-colormap-whitebg-bw\"))");
  x_color_allocate ();

  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}


void
action_callback_view_shaded_colors (GschemAction *action,
                                   GschemToplevel *w_current)
{
  x_color_free ();
  /* Change the scheme here */
  g_scm_c_eval_string_protected ("(load (build-path geda-rc-path \"gschem-colormap-lightbg\"))");
  x_color_allocate ();

  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}


void
action_callback_view_shaded_bw_colors (GschemAction *action,
                                      GschemToplevel *w_current)
{
  x_color_free ();
  /* Change the scheme here */
  g_scm_c_eval_string_protected ("(load (build-path geda-rc-path \"gschem-colormap-bw\"))");
  x_color_allocate ();

  gschem_page_view_invalidate_all (gschem_toplevel_get_current_page_view (w_current));
}


/*! \section help-menu Help Menu Callback Functions */


void
action_callback_help_hotkeys (GschemAction *action,
                             GschemToplevel *w_current)
{
  x_dialog_hotkeys(w_current);
}


void
action_callback_help_about (GschemAction *action,
                           GschemToplevel *w_current)
{
  about_dialog(w_current);
}


/*! \section buffer-menu Buffer Menu Callback Functions */


void
action_callback_buffer_copy1 (GschemAction *action,
                             GschemToplevel *w_current)
{
  i_buffer_copy (w_current, 1, action);
}


void
action_callback_buffer_copy2 (GschemAction *action,
                             GschemToplevel *w_current)
{
  i_buffer_copy (w_current, 2, action);
}


void
action_callback_buffer_copy3 (GschemAction *action,
                             GschemToplevel *w_current)
{
  i_buffer_copy (w_current, 3, action);
}


void
action_callback_buffer_copy4 (GschemAction *action,
                             GschemToplevel *w_current)
{
  i_buffer_copy (w_current, 4, action);
}


void
action_callback_buffer_copy5 (GschemAction *action,
                             GschemToplevel *w_current)
{
  i_buffer_copy (w_current, 5, action);
}


void
action_callback_buffer_cut1 (GschemAction *action,
                            GschemToplevel *w_current)
{
  i_buffer_cut (w_current, 1, action);
}


void
action_callback_buffer_cut2 (GschemAction *action,
                            GschemToplevel *w_current)
{
  i_buffer_cut (w_current, 2, action);
}


void
action_callback_buffer_cut3 (GschemAction *action,
                            GschemToplevel *w_current)
{
  i_buffer_cut (w_current, 3, action);
}


void
action_callback_buffer_cut4 (GschemAction *action,
                            GschemToplevel *w_current)
{
  i_buffer_cut (w_current, 4, action);
}


void
action_callback_buffer_cut5 (GschemAction *action,
                            GschemToplevel *w_current)
{
  i_buffer_cut (w_current, 5, action);
}


void
action_callback_buffer_paste1 (GschemAction *action,
                              GschemToplevel *w_current)
{
  i_buffer_paste (w_current, 1, action);
}


void
action_callback_buffer_paste2 (GschemAction *action,
                              GschemToplevel *w_current)
{
  i_buffer_paste (w_current, 2, action);
}


void
action_callback_buffer_paste3 (GschemAction *action,
                              GschemToplevel *w_current)
{
  i_buffer_paste (w_current, 3, action);
}


void
action_callback_buffer_paste4 (GschemAction *action,
                              GschemToplevel *w_current)
{
  i_buffer_paste (w_current, 4, action);
}


void
action_callback_buffer_paste5 (GschemAction *action,
                              GschemToplevel *w_current)
{
  i_buffer_paste (w_current, 5, action);
}


/*! \section special-actions Special Actions */


void
action_callback_options_grid_size (GschemAction *action,
                                  GschemToplevel *w_current)
{
  /* grid size spinbox */
}


void
action_callback_options_grid (GschemAction *action,
                             GschemToplevel *w_current)
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


void
action_callback_options_grid_none (GschemAction *action,
                                  GschemToplevel *w_current)
{
  gschem_options_set_grid_mode (w_current->options, GRID_MODE_NONE);
}


void
action_callback_options_grid_dots (GschemAction *action,
                                  GschemToplevel *w_current)
{
  gschem_options_set_grid_mode (w_current->options, GRID_MODE_DOTS);
}


void
action_callback_options_grid_mesh (GschemAction *action,
                                  GschemToplevel *w_current)
{
  gschem_options_set_grid_mode (w_current->options, GRID_MODE_MESH);
}


void
action_callback_options_snap (GschemAction *action,
                             GschemToplevel *w_current)
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


void
action_callback_view_pan_left (GschemAction *action,
                              GschemToplevel *w_current)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  gschem_page_view_pan_mouse (page_view, w_current->keyboardpan_gain, 0);
}


void
action_callback_view_pan_right (GschemAction *action,
                               GschemToplevel *w_current)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  /* yes, that's a negative sign there */
  gschem_page_view_pan_mouse (page_view, -w_current->keyboardpan_gain, 0);
}


void
action_callback_view_pan_up (GschemAction *action,
                            GschemToplevel *w_current)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  gschem_page_view_pan_mouse (page_view, 0, w_current->keyboardpan_gain);
}


void
action_callback_view_pan_down (GschemAction *action,
                              GschemToplevel *w_current)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  /* yes, that's a negative sign there */
  gschem_page_view_pan_mouse (page_view, 0, -w_current->keyboardpan_gain);
}


void
action_callback_edit_select (GschemAction *action,
                            GschemToplevel *w_current)
{
  /* Select also does not update the middle button shortcut. */
  o_redraw_cleanstates(w_current);

  /* this is probably the only place this should be */
  i_set_state(w_current, SELECT);
  i_action_stop (w_current);
}


void
action_callback_cancel (GschemAction *action,
                       GschemToplevel *w_current)
{
  i_cancel (w_current);
}


void
action_callback_page_print (GschemAction *action,
                           GschemToplevel *w_current)
{
  s_page_print_all(gschem_toplevel_get_toplevel (w_current));
}


/*! \brief Cause the last action executed to be repeated. */


void
action_callback_repeat_last_action (GschemAction *action,
                                   GschemToplevel *w_current)
{
  if (w_current->last_action != NULL)
    gschem_action_activate (w_current->last_action, w_current);
}
