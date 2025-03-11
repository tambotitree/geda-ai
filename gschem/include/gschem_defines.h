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

#ifndef _GSCHEM_DEFINES_H_INCL
#define _GSCHEM_DEFINES_H_INCL

#define ZOOM_OUT 0
#define ZOOM_IN 1
#define ZOOM_FULL 2

#define ZOOM_EXTENTS_PADDING_PX 5

/* For grip size in pixels (i.e. device units) */
#define GRIP_SIZE		10.0

/* for bus_ripper_type */
#define COMP_BUS_RIPPER         0
#define NET_BUS_RIPPER          1

#define FREE        1
#define CONSTRAINED 2

/* for attrib_edit_dialog invocation flag */
#define FROM_MENU		0
#define FROM_HOTKEY		1

/* for text cap style */
#define LOWER 0
#define UPPER 1
#define BOTH  2

/* These modes are for actionfeedback_mode */
/* there's a hack in i_keypress.c dealing with the 0 and 1 (has to be these */
/* values */
#define OUTLINE         0
#define BOUNDINGBOX     1

/* This is an additional mode for last_drawb_mode, to indicate there was no
 * last bounding box drawn. last_drawb_mode also takes actionfeedback_mode
 * constants, so be sure not to clash with those */
#define LAST_DRAWB_MODE_NONE -1

/* used in o_undo_callback */
#define UNDO_ACTION		0
#define REDO_ACTION		1

/* used for undo_type */
#define UNDO_DISK		0
#define UNDO_MEMORY		1

/* selection types */
/* used in o_select_object */
#define SINGLE                  0
#define MULTIPLE                1

/* for grid */
typedef enum
{
  GRID_MODE_NONE,
  GRID_MODE_DOTS,
  GRID_MODE_MESH,
  GRID_MODE_COUNT
} GRID_MODE;

/* for dots_grid_mode */
#define DOTS_GRID_VARIABLE_MODE 0
#define DOTS_GRID_FIXED_MODE    1

/* for log-window keyword */
#define MAP_LATER		0
#define MAP_ON_STARTUP		1

/* for log-window-type */
#define DECORATED		0
#define TRANSIENT		1

/* for third-mouse */
#define POPUP_ENABLED		0
#define MOUSEPAN_ENABLED	1

/* for middle-mouse */
#define STROKE			0
#define REPEAT			1
#define ACTION			2
#define MID_MOUSEPAN_ENABLED	3

/* for scroll-wheel */
#define SCROLL_WHEEL_CLASSIC 0
#define SCROLL_WHEEL_GTK     1

/* for selected_from */
#define DONTCARE		0
#define MENU			1
#define HOTKEY			2

#endif /* !_GSCHEM_DEFINES_H_INCL */
