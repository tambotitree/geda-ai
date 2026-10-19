/* gEDA - GPL Electronic Design Automation
 * gsymcheck - gEDA Symbol Check
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02111-1301 USA.
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

#include <libgeda/libgeda.h>

#include "../include/struct.h"
#include "../include/prototype.h"

/*! \brief Register a Scheme procedure with a checked zero-argument signature.
 * \param name Scheme binding to create.
 * \param callback C implementation; only the Guile ABI boundary erases its type.
 */
static void
register_subr_0 (const char *name, SCM (*callback) (void))
{
  scm_c_define_gsubr (name, 0, 0, 0, (scm_t_subr) callback);
}

/*! \brief Register a Scheme procedure with a checked one-argument signature.
 * \param name Scheme binding to create.
 * \param callback C implementation taking exactly one Scheme value.
 */
static void
register_subr_1 (const char *name, SCM (*callback) (SCM))
{
  scm_c_define_gsubr (name, 1, 0, 0, (scm_t_subr) callback);
}

/*! \brief Register exit procedures and the RC version-check procedure. */
void
g_register_funcs (void)
{
  register_subr_0 ("quit", g_quit);
  register_subr_0 ("exit", g_quit);
  register_subr_1 ("gsymcheck-version", g_rc_gsymcheck_version);
}

/*! \brief Release checker resources and terminate successfully. */
SCM
g_quit (void)
{
  gsymcheck_quit ();
  exit (0);
}
