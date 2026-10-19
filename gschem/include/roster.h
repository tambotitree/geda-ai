/* gEDA - gschem Widget Roster Definition
 * This header defines the XWindowRosterItem enum and the GschemMainWindowRoster struct,
 * which are used to track and validate important top-level GUI widgets in gschem.
 *
 * Copyright (C) 2025 John Ryan, maintainer geda-ai
 * Licensed under the GNU General Public License, version 2 or later.
 */
#ifndef GSCHEM_ROSTER_H
#define GSCHEM_ROSTER_H

#include <gtk/gtk.h>  // needed for GtkWidget etc.
typedef enum {
    XWIN_MAIN_BOX = 0,
    XWIN_DRAWING_AREA,
    XWIN_MENUBAR,
    XWIN_RECENT_CHOOSER_MENU,
    XWIN_LEFT_DOCKING_AREA_MENU,
    XWIN_BOTTOM_DOCKING_AREA_MENU,
    XWIN_RIGHT_DOCKING_AREA_MENU,
    XWIN_POPUP_MENU,
    XWIN_TOOLBAR,
    XWIN_FILE_CHANGE_NOTIFICATION,
    XWIN_PATCH_CHANGE_NOTIFICATION,
    XWIN_FIND_TEXT_WIDGET,
    XWIN_HIDE_TEXT_WIDGET,
    XWIN_SHOW_TEXT_WIDGET,
    XWIN_MACRO_WIDGET,
    XWIN_BOTTOM_WIDGET,
    XWIN_LEFT_NOTEBOOK,
    XWIN_RIGHT_NOTEBOOK,

    XWIN_LAST  // always keep this last!
} XWindowRosterItem;
typedef struct _GschemMainWindowRoster {
    XWindowRosterItem id;
    const char *name;
    GtkWidget *widget;

    int top_left_x, top_left_y;
    int width, height;
    gboolean visible;
    gboolean mapped;
} GschemMainWindowRoster;

extern GschemMainWindowRoster gschem_widget_roster[XWIN_LAST];

#endif /* GSCHEM_ROSTER_H */

/* end of roster.h */
