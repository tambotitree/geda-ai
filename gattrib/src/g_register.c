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
 * \brief Functions to register Scheme functions
 *
 * Functions to register Scheme functions
 */

#include <config.h>

#include <stdio.h>
#include <sys/stat.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

/*------------------------------------------------------------------
 * Gattrib specific includes
 *------------------------------------------------------------------*/
#include <libgeda/libgeda.h>       /* geda library fcns  */
#include "../include/struct.h"     /* typdef and struct declarations */
#include "../include/prototype.h"  /* function prototypes */
#include "../include/globals.h"

/* GtkWidget *w_main; */


/* ---------------------------------------------------------------------- */
/*! \brief Register Scheme functions
 *
 * This function registers the Scheme functions required to use
 * gattrib.  They are mostly unnecessary, except for reading in the gattribrc
 * file at the beginning of the prog which gives the library search paths.
 */
/*! \brief Register a zero-argument callback with its C signature checked.
 * \param name Scheme name to bind.
 * \param callback Typed callback; conversion occurs only at the Guile call.
 */
static void
register_subr_0 (const char *name, SCM (*callback) (void))
{
  scm_c_define_gsubr (name, 0, 0, 0, (scm_t_subr) callback);
}

/*! \brief Register a one-argument callback with its C signature checked.
 * \param name Scheme name to bind.
 * \param callback Typed callback taking one Scheme value.
 */
static void
register_subr_1 (const char *name, SCM (*callback) (SCM))
{
  scm_c_define_gsubr (name, 1, 0, 0, (scm_t_subr) callback);
}

/*! \brief Register the exit and RC version-check procedures. */
void g_register_funcs (void)
{
  /* general functions */
  register_subr_0 ("quit", g_quit);
  register_subr_0 ("exit", g_quit);

  /* gattrib functions */
  register_subr_1 ("gattrib-version", g_rc_gattrib_version);

}

/*! \brief Scheme function to quit the application
 *
 * Quit the application from within Scheme.
 */
SCM g_quit (void)
{
#ifdef DEBUG
  printf("In g_quit, calling exit(0)\n");
#endif

  gattrib_quit(0); 
  /*  exit(0);  */  /* Necessary? */

  /* we don't really get here, but otherwise gcc complains */
  return SCM_BOOL_F;
}

