/* gEDA - GPL Electronic Design Automation
 * gattrib -- gEDA component and net attribute manipulation using spreadsheet.
 * Copyright (C) 2003-2010 Stuart D. Brorson.
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

/*! \file
 *  \brief Global declarations
 *
 * Global declarations
 */

#include <config.h>
#include <stdio.h>

/*------------------------------------------------------------------
 * Gattrib specific includes
 *------------------------------------------------------------------*/
#include <libgeda/libgeda.h>       /* geda library fcns  */
#include "../include/struct.h"     /* typdef and struct declarations */
#include "../include/prototype.h"  /* function prototypes */
#include "../include/globals.h"

TOPLEVEL *pr_current;

SHEET_DATA *sheet_head;

GtkWidget *window;
GtkWidget *notebook;

GtkSheet **sheets;

GtkWidget **scrolled_windows;
GtkWidget *entry;
GtkWidget *location;
GtkWidget *left_button;
GtkWidget *center_button;
GtkWidget *right_button;
GtkWidget *label;

/* command line arguments */
int verbose_mode=FALSE; //!< Reflects the value of the command line flag
int quiet_mode=FALSE;   //!< Reflects the value of the command line flag

/*!
 * these are required by libgeda
 */
void (*variable_set_func)() = i_vars_set;
