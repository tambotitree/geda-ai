/* x_window.h
 *
 * Gschem main window roster system for runtime widget verification and diagnostics.
 * 
 * This header defines the data structures used to track, inspect, and debug the main
 * window layout in gschem. Each major widget added to the GTK UI hierarchy is recorded
 * along with metadata such as its screen location, type, and visibility state.
 * 
 * This aids layout validation, diagnostics, and debugging of rendering behavior.
 * 
 * Copyleft 2025 John Ryan, maintainer geda-ai
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

 #ifndef GSCHEM_X_WINDOW_H
 #define GSCHEM_X_WINDOW_H
 
 #include <gtk/gtk.h>
 
 #ifdef __cplusplus
 extern "C" {
 #endif
 
 /* Enumeration of trackable main window widgets */
 typedef enum {
     XWIN_MAIN = 0,
     XWIN_MENU_BAR,
     XWIN_TOOL_BAR,
     XWIN_LEFT_NOTEBOOK,
     XWIN_RIGHT_NOTEBOOK,
     XWIN_BOTTOM_NOTEBOOK,
     XWIN_WORK_BOX,
     XWIN_SCROLLED_WINDOW,
     XWIN_DRAWING_AREA,
     XWIN_FILE_CHANGE_BAR,
     XWIN_PATCH_CHANGE_BAR,
     XWIN_FIND_TEXT_WIDGET,
     XWIN_HIDE_TEXT_WIDGET,
     XWIN_SHOW_TEXT_WIDGET,
     XWIN_MACRO_WIDGET,
     XWIN_BOTTOM_WIDGET,
     XWIN_LAST  /* Always keep this last as a sentinel */
 } XWindowRosterItem;
 
 /* Simple roster record per major UI widget */
 typedef struct {
     XWindowRosterItem id;
     const char *name;
     GtkWidget *widget;
     gint top_left_x;
     gint top_left_y;
     gint width;
     gint height;
     gboolean visible;
     gboolean mapped;
 } GschemMainWindowRoster;

 
void add_roster_item(XWindowRosterItem id, const char *name, GtkWidget *widget);
void print_roster(void);
void x_window_main_check_roster(GschemToplevel *w_current);
gboolean x_window_check_roster(GschemMainWindowRoster *roster, int count);
 #ifdef __cplusplus
 }
 #endif
 
 #endif /* GSCHEM_X_WINDOW_H */