/* gEDA - GPL Electronic Design Automation
 * libgeda - gEDA's library
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

#include <config.h>

#include <stdio.h>
#include <sys/stat.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "libgeda_priv.h"

/*! \brief Register a 0-argument procedure after checking its C signature. */
static void
register_subr_0 (const char *name, int required, int optional,
                   SCM (*callback) (void))
{
  g_return_if_fail (required + optional == 0);
  /* Guile's generic registration API erases the checked callback type. */
  scm_c_define_gsubr (name, required, optional, 0, (scm_t_subr) callback);
}

/*! \brief Register a 1-argument procedure after checking its C signature. */
static void
register_subr_1 (const char *name, int required, int optional,
                   SCM (*callback) (SCM))
{
  g_return_if_fail (required + optional == 1);
  /* Guile's generic registration API erases the checked callback type. */
  scm_c_define_gsubr (name, required, optional, 0, (scm_t_subr) callback);
}

/*! \brief Register a 2-argument procedure after checking its C signature. */
static void
register_subr_2 (const char *name, int required, int optional,
                   SCM (*callback) (SCM, SCM))
{
  g_return_if_fail (required + optional == 2);
  /* Guile's generic registration API erases the checked callback type. */
  scm_c_define_gsubr (name, required, optional, 0, (scm_t_subr) callback);
}

/*! \brief Register a 3-argument procedure after checking its C signature. */
static void
register_subr_3 (const char *name, int required, int optional,
                   SCM (*callback) (SCM, SCM, SCM))
{
  g_return_if_fail (required + optional == 3);
  /* Guile's generic registration API erases the checked callback type. */
  scm_c_define_gsubr (name, required, optional, 0, (scm_t_subr) callback);
}

/*! \brief Register libgeda procedures with explicit argument contracts. */
void
g_register_libgeda_funcs (void)
{
  register_subr_2 ("eval-protected", 1, 1, g_scm_eval_protected);
  register_subr_1 ("eval-string-protected", 1, 0, g_scm_eval_string_protected);
  register_subr_2 ("component-library", 1, 1, g_rc_component_library);
  register_subr_3 ("component-library-command", 3, 0, g_rc_component_library_command);
  register_subr_3 ("component-library-funcs", 3, 0, g_rc_component_library_funcs);
  register_subr_1 ("source-library", 1, 0, g_rc_source_library);
  register_subr_1 ("source-library-search", 1, 0, g_rc_source_library_search);
  register_subr_3 ("world-size", 3, 0, g_rc_world_size);
  register_subr_0 ("reset-component-library", 0, 0, g_rc_reset_component_library);
  register_subr_0 ("reset-source-library", 0, 0, g_rc_reset_source_library);
  register_subr_1 ("scheme-directory", 1, 0, g_rc_scheme_directory);
  register_subr_1 ("bitmap-directory", 1, 0, g_rc_bitmap_directory);
  register_subr_1 ("bus-ripper-symname", 1, 0, g_rc_bus_ripper_symname);
  register_subr_1 ("attribute-promotion", 1, 0, g_rc_attribute_promotion);
  register_subr_1 ("promote-invisible", 1, 0, g_rc_promote_invisible);
  register_subr_1 ("keep-invisible", 1, 0, g_rc_keep_invisible);
  register_subr_1 ("always-promote-attributes", 1, 0, g_rc_always_promote_attributes);
  register_subr_1 ("make-backup-files", 1, 0, g_rc_make_backup_files);
  register_subr_1 ("print-color-map", 0, 1, g_rc_print_color_map);
  register_subr_0 ("rc-filename", 0, 0, g_rc_rc_filename);
  register_subr_0 ("rc-config", 0, 0, g_rc_rc_config);
}

/*! \brief Register some libgeda directories with Scheme.
 * \par Function Description
 * Ensures that the default gEDA Scheme directory is added to the
 * Guile load path.
 */
void
g_register_libgeda_dirs ()
{
  char *scheme_dir;

  scheme_dir = g_build_filename (s_path_sys_data (), "scheme", NULL);
  g_rc_scheme_directory (scm_from_utf8_string (scheme_dir));
  g_free (scheme_dir);
}
