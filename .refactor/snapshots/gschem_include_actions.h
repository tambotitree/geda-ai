/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 2025 John Ryan
 *
 * Copyleft 2025 John Ryan, maintainer geda-ai
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef GSCHEM_ACTIONS_H
#define GSCHEM_ACTIONS_H

#include "gschem_action.h"
#include "gschem.h"

G_BEGIN_DECLS
typedef struct _GschemAction GschemAction;

/* Initialization */
void gschem_register_all_actions(void);

/* Callback declarations for File menu actions */
void action_callback_file_new(GschemAction *action, GschemToplevel *w_current);
void action_callback_file_open(GschemAction *action, GschemToplevel *w_current);
void action_callback_file_open_recent(GschemAction *action, GschemToplevel *w_current);
void action_callback_file_save(GschemAction *action, GschemToplevel *w_current);
void action_callback_file_save_as(GschemAction *action, GschemToplevel *w_current);
void action_callback_file_save_all(GschemAction *action, GschemToplevel *w_current);
void action_callback_page_revert(GschemAction *action, GschemToplevel *w_current);
void action_callback_page_close(GschemAction *action, GschemToplevel *w_current);
void action_callback_file_print(GschemAction *action, GschemToplevel *w_current);
void action_callback_file_write_png(GschemAction *action, GschemToplevel *w_current);
void action_callback_file_new_window(GschemAction *action, GschemToplevel *w_current);
void action_callback_file_close(GschemAction *action, GschemToplevel *w_current);
void action_callback_file_quit(GschemAction *action, GschemToplevel *w_current);

/* Callback declarations for Edit menu actions */
void action_callback_edit_undo(GschemAction *action, GschemToplevel *w_current);
void action_callback_edit_redo(GschemAction *action, GschemToplevel *w_current);
void action_callback_clipboard_cut(GschemAction *action, GschemToplevel *w_current);
void action_callback_clipboard_copy(GschemAction *action, GschemToplevel *w_current);
void action_callback_clipboard_paste(GschemAction *action, GschemToplevel *w_current);
void action_callback_edit_delete(GschemAction *action, GschemToplevel *w_current);
void action_callback_edit_select_all(GschemAction *action, GschemToplevel *w_current);
void action_callback_edit_deselect(GschemAction *action, GschemToplevel *w_current);
void action_callback_edit_find(GschemAction *action, GschemToplevel *w_current);

/* Callback declarations for Add menu actions */
void action_callback_add_component(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_last_component(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_net(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_bus(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_attribute(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_text(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_line(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_path(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_box(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_circle(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_arc(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_pin(GschemAction *action, GschemToplevel *w_current);
void action_callback_add_picture(GschemAction *action, GschemToplevel *w_current);

/* File menu actions */
extern GschemAction *action_file_new;
extern GschemAction *action_file_open;
extern GschemAction *action_file_open_recent;
extern GschemAction *action_file_save;
extern GschemAction *action_file_save_as;
extern GschemAction *action_file_save_all;
extern GschemAction *action_page_revert;
extern GschemAction *action_page_close;
extern GschemAction *action_file_print;
extern GschemAction *action_file_write_png;
extern GschemAction *action_file_new_window;
extern GschemAction *action_file_close;
extern GschemAction *action_file_quit;
extern GschemAction *action_file_repl;

/* View menu actions */
extern GschemAction *action_view_menubar;
extern GschemAction *action_view_toolbar;
extern GschemAction *action_view_scrollbars;

/* Page navigation actions */
extern GschemAction *action_page_prev;
extern GschemAction *action_page_next;

/* Hierarchy actions */
extern GschemAction *action_hierarchy_up;

/* Edit menu actions */
extern GschemAction *action_edit_undo;
extern GschemAction *action_edit_redo;
extern GschemAction *action_clipboard_cut;
extern GschemAction *action_clipboard_copy;
extern GschemAction *action_clipboard_paste;
extern GschemAction *action_edit_delete;
extern GschemAction *action_edit_select_all;
extern GschemAction *action_edit_deselect;
extern GschemAction *action_edit_find;

/* Add menu actions */
extern GschemAction *action_add_component;
extern GschemAction *action_add_last_component;
extern GschemAction *action_add_net;
extern GschemAction *action_add_bus;
extern GschemAction *action_add_attribute;
extern GschemAction *action_add_text;
extern GschemAction *action_add_line;
extern GschemAction *action_add_path;
extern GschemAction *action_add_box;
extern GschemAction *action_add_circle;
extern GschemAction *action_add_arc;
extern GschemAction *action_add_pin;
extern GschemAction *action_add_picture;

/* TODO: Add any additional action declarations here */

G_END_DECLS

#endif /* GSCHEM_ACTIONS_H */
