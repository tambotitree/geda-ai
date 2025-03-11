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
#include <config.h>

#include <stdio.h>
#include <math.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "gschem.h"

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_complex_prepare_place(GschemToplevel *w_current, const CLibSymbol *sym)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  GList *temp_list;
  OBJECT *o_current;
  char *buffer;
  const gchar *sym_name = s_clib_symbol_get_name (sym);
  GError *err = NULL;

  i_set_state (w_current, COMPMODE);
  i_action_start (w_current);

  /* remove the old place list if it exists */
  s_delete_object_glist(toplevel, toplevel->page_current->place_list);
  toplevel->page_current->place_list = NULL;

  /* Insert the new object into the buffer at world coordinates (0,0).
   * It will be translated to the mouse coordinates during placement. */

  w_current->first_wx = 0;
  w_current->first_wy = 0;

  if (w_current->include_complex) {

    temp_list = NULL;

    buffer = s_clib_symbol_get_data (sym);
    temp_list = o_read_buffer (toplevel,
                               temp_list,
                               buffer, -1,
                               sym_name,
                               &err);
    g_free (buffer);

    if (err) {
      /* If an error occurs here, we can assume that the preview also has failed to load,
         and the error message is displayed there. We therefore ignore this error, but
         end the component insertion.
         */

      g_error_free(err);
      i_set_state (w_current, SELECT);
      i_action_stop (w_current);
      return;
    }

    /* Take the added objects */
    toplevel->page_current->place_list =
      g_list_concat (toplevel->page_current->place_list, temp_list);

  } else { /* if (w_current->include_complex) {..} else { */
    OBJECT *new_object;

    new_object = o_complex_new (toplevel, OBJ_COMPLEX, DEFAULT_COLOR,
                                0, 0, 0, 0, sym, sym_name, 1);

    if (new_object->type == OBJ_PLACEHOLDER) {
      /* If created object is a placeholder, the loading failed and we end the insert action */

      s_delete_object(toplevel, new_object);
      i_set_state (w_current, SELECT);
      i_action_stop (w_current);
      return;
    }
    else {

      toplevel->page_current->place_list =
          g_list_concat (toplevel->page_current->place_list,
                         o_complex_promote_attribs (toplevel, new_object));
      toplevel->page_current->place_list =
          g_list_append (toplevel->page_current->place_list, new_object);

      /* Flag the symbol as embedded if necessary */
      o_current = (g_list_last (toplevel->page_current->place_list))->data;
      if (w_current->embed_complex) {
        o_current->complex_embedded = TRUE;
      }
    }
  }

  /* Run the complex place list changed hook without redrawing */
  /* since the place list is going to be redrawn afterwards */
  o_complex_place_changed_run_hook (w_current);
}


/*! \brief Run the complex place list changed hook.
 *  \par Function Description
 *  The complex place list is usually used when placing new components
 *  in the schematic. This function should be called whenever that list
 *  is modified.
 *  \param [in] w_current GschemToplevel structure.
 *
 */
void o_complex_place_changed_run_hook(GschemToplevel *w_current) {
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  GList *ptr = NULL;

  /* Run the complex place list changed hook */
  if (scm_is_false (scm_hook_empty_p (complex_place_list_changed_hook)) &&
      toplevel->page_current->place_list != NULL) {
    ptr = toplevel->page_current->place_list;

    scm_dynwind_begin (0);
    g_dynwind_window (w_current);
    while (ptr) {
      SCM expr = scm_list_3 (scm_from_utf8_symbol ("run-hook"),
                             complex_place_list_changed_hook,
                             edascm_from_object ((OBJECT *) ptr->data));
      g_scm_eval_protected (expr, scm_interaction_environment ());
      ptr = g_list_next(ptr);
    }
    scm_dynwind_end ();
  }
}
