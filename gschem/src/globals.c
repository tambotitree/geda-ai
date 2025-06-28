/**
 * \file globals.h
 * \brief Global variables used throughout gschem
 *
 * This file declares global variables that maintain application-wide state,
 * including UI behavior, command-line flags, configuration paths, logging levels,
 * and GUI object references.
 */
/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2020 gEDA Contributors see ChangeLog for details)
 * Copyright (C) 2021-2025 John Ryan, maintainer geda-ai
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

#include "gschem.h"

/*-----------------------------------------
 * Window & Widget Globals
 *----------------------------------------*/

/** 
 * Global list of all main windows (GschemToplevels).
 * Used to broadcast events or manage multi-window workflows.
 */
extern GList *global_window_list;

/**
 * Recent files manager, used by the GTK file chooser.
 */
extern GtkRecentManager *recent_manager;

/*-----------------------------------------
 * Configuration & Output
 *----------------------------------------*/

/**
 * Path to an additional user-specified configuration file (via -r).
 */
extern char *rc_filename;

/**
 * Output file path (e.g., for printing, if -o is used).
 */
extern char *output_filename;

/*-----------------------------------------
 * Color Definitions
 *----------------------------------------*/

/**
 * Global color: white (used in themes and background).
 */
extern GdkRGBA white;

/**
 * Global color: black (used in themes and default text).
 */
extern GdkRGBA black;

/*-----------------------------------------
 * Logging and Hooks
 *----------------------------------------*/

/**
 * Destination for runtime logging (e.g., console, file, window).
 * Defined as LOG_WINDOW, LOG_FILE, etc.
 */
extern int logging_dest;

/**
 * Guile hook: invoked when the list of placeable complex components changes.
 */
extern SCM complex_place_list_changed_hook;

/*-----------------------------------------
 * Command-line Flags and Modes
 *----------------------------------------*/

/**
 * Quiet mode (suppress most output); set via -q.
 */
extern int quiet_mode;

/**
 * Verbose mode (increases runtime output); set via -v.
 *
 * Every -v you add increases verbosity by one level.
 * Like Nigel Tufnel’s amp, we go to 11. Because why stop at 10?
 * Use -vvvv (4 v's) if you want to see the whole circus.
 * Use -vvvvvvvvvvv (11 v's) if you want to *run* the circus.
 */
extern int verbose_mode;

/**
 * If true, the main window is auto-placed at startup; set via -p.
 */
extern int auto_place_mode;

/**
 * Verbosity level: incremented with each -v flag (max 11).
 *
 * Measured in decibels of developer self-loathing.
 * If you're at level 11 and still don’t know what's wrong—
 * it’s probably hardware.
 */
extern int verbosity_level;