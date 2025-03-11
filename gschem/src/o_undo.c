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
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "gschem.h"
#include "actions.decl.x"

static int undo_file_index=0;
static int prog_pid=0;

static char* tmp_path = NULL;

/* this is additional number of levels (or history) at which point the */
/* undo stack will be trimmed, it's used a safety to prevent running out */
/* of entries to free */
#define UNDO_PADDING  5

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_undo_init(void)
{
  prog_pid = getpid();

  tmp_path = g_strdup (getenv("TMP"));
  if (tmp_path == NULL) {
     tmp_path = g_strdup ("/tmp");
  }
#if DEBUG
  printf("%s\n", tmp_path);
#endif
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 *
 *  <B>flag</B> can be one of the following values:
 *  <DL>
 *    <DT>*</DT><DD>UNDO_ALL
 *    <DT>*</DT><DD>UNDO_VIEWPORT_ONLY
 *  </DL>
 */
void
o_undo_savestate (GschemToplevel *w_current, PAGE *page, int flag,
                  const gchar *desc)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  char *filename = NULL;
  GList *object_list = NULL;
  int levels;
  UNDO *u_current;
  UNDO *u_current_next;

  GschemPageView *view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (view != NULL);

  g_return_if_fail (page != NULL);

  GschemPageGeometry *geometry = gschem_page_view_get_page_geometry (view);

  /* save autosave backups if necessary */
  o_autosave_backups(w_current);

  if (w_current->undo_control == FALSE) {
    return;
  }

  if (flag == UNDO_ALL) {

    /* Increment the number of operations since last backup if
       auto-save is enabled */
    if (toplevel->auto_save_interval != 0) {
      page->ops_since_last_backup++;
    }

    /* HACK */
    /* Before we save the undo state, consolidate nets as necessary */

    /* This is where the net consolidation call would have been
     * triggered before it was removed from o_save_buffer().
     */
    if (toplevel->net_consolidate == TRUE)
      o_net_consolidate (toplevel, page);
  }

  if (w_current->undo_type == UNDO_DISK && flag == UNDO_ALL) {

    filename = g_strdup_printf("%s%cgschem.save%d_%d.sch",
                               tmp_path, G_DIR_SEPARATOR,
                               prog_pid, undo_file_index++);

    /* Changed from f_save to o_save when adding backup copy creation. */
    /* f_save manages the creaton of backup copies.
       This way, f_save is called only when saving a file, and not when
       saving an undo backup copy */
    o_save (toplevel, s_page_objects (page), filename, NULL);

  } else if (w_current->undo_type == UNDO_MEMORY && flag == UNDO_ALL) {
    object_list = o_glist_copy_all (toplevel,
                                    s_page_objects (page),
                                    object_list);
  }

  /* Clear Anything above current */
  if (page->undo_current) {
    s_undo_remove_rest(toplevel, page->undo_current->next);
    page->undo_current->next = NULL;
  } else { /* undo current is NULL */
    s_undo_remove_rest(toplevel, page->undo_bottom);
    page->undo_bottom = NULL;
  }

  page->undo_tos = page->undo_current;

  /* Don't save a page geometry if this is the root undo node.  The
   * initial "Zoom to Extents" hasn't been performed yet, and if the
   * node doesn't specify a page geometry, a "Zoom to Extents" is
   * performed on undo, which is exactly what we want. */
  if (geometry != NULL && page->undo_current != NULL) {
    page->undo_tos = s_undo_add(page->undo_tos,
                                flag, filename, object_list,
                                (geometry->viewport_left + geometry->viewport_right) / 2,
                                (geometry->viewport_top + geometry->viewport_bottom) / 2,
                                /* scale */
                                max (((double) abs (geometry->viewport_right - geometry->viewport_left) / geometry->screen_width),
                                  ((double) abs (geometry->viewport_top - geometry->viewport_bottom) / geometry->screen_height)),
                                page->page_control,
                                page->up,
                                page->CHANGED,
                                desc);
  } else {
    page->undo_tos = s_undo_add(page->undo_tos,
                                flag, filename, object_list,
                                0, /* center x */
                                0, /* center y */
                                0, /* scale */
                                page->page_control,
                                page->up,
                                page->CHANGED,
                                desc);
  }

  page->undo_current =
      page->undo_tos;

  if (page->undo_bottom == NULL) {
    page->undo_bottom =
        page->undo_tos;
  }

#if DEBUG
  printf("\n\n---Undo----\n");
  s_undo_print_all(page->undo_bottom);
  printf("BOTTOM: %s\n", page->undo_bottom->filename);
  printf("TOS: %s\n", page->undo_tos->filename);
  printf("CURRENT: %s\n", page->undo_current->filename);
  printf("----\n");
#endif

  g_free(filename);

  o_undo_update_actions (w_current, page);

  /* Now go through and see if we need to free/remove some undo levels */
  /* so we stay within the limits */

  /* only check history every 10 undo savestates */
  if (undo_file_index % 10) {
    return;
  }

  levels = s_undo_levels(page->undo_bottom);

#if DEBUG
  printf("levels: %d\n", levels);
#endif

  if (levels >= w_current->undo_levels + UNDO_PADDING) {
    levels = levels - w_current->undo_levels;

#if DEBUG
    printf("Trimming: %d levels\n", levels);
#endif

    u_current = page->undo_bottom;

    while (levels > 0) {
      /* Because we use a pad you are always guaranteed to never */
      /* exhaust the list */
      g_assert (u_current != NULL);

      u_current_next = u_current->next;

      if (u_current->filename) {
#if DEBUG
        printf("Freeing: %s\n", u_current->filename);
#endif
        unlink(u_current->filename);
        g_free(u_current->filename);
      }

      if (u_current->object_list) {
        s_delete_object_glist (toplevel, u_current->object_list);
        u_current->object_list = NULL;
      }

      u_current->next = NULL;
      u_current->prev = NULL;
      g_free(u_current);

      u_current = u_current_next;
      levels--;
    }

    g_assert (u_current != NULL);
    u_current->prev = NULL;
    page->undo_bottom = u_current;

#if DEBUG
    printf("New current is: %s\n", u_current->filename);
#endif
  }

#if DEBUG
  printf("\n\n---Undo----\n");
  s_undo_print_all(page->undo_bottom);
  printf("BOTTOM: %s\n", page->undo_bottom->filename);
  printf("TOS: %s\n", page->undo_tos->filename);
  printf("CURRENT: %s\n", page->undo_current->filename);
  printf("----\n");
#endif

}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 *
 *  <B>flag</B> can be one of the following values:
 *  <DL>
 *    <DT>*</DT><DD>UNDO_ALL
 *    <DT>*</DT><DD>UNDO_VIEWPORT_ONLY
 *  </DL>
 */
void
o_undo_savestate_old (GschemToplevel *w_current, int flag, const gchar *desc)
{
  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (page_view != NULL);

  PAGE *page = gschem_page_view_get_page (page_view);

  o_undo_savestate (w_current, page, flag, desc);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
char *o_undo_find_prev_filename(UNDO *start)
{
  UNDO *u_current;

  u_current = start->prev;

  while(u_current) {
    if (u_current->filename) {
      return(u_current->filename);
    }
    u_current = u_current->prev;
  }

  return(NULL);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
GList *o_undo_find_prev_object_head (UNDO *start)
{
  UNDO *u_current;

  u_current = start->prev;

  while(u_current) {
    if (u_current->object_list) {
      return u_current->object_list;
    }
    u_current = u_current->prev;
  }

  return(NULL);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 *  <B>type</B> can be one of the following values:
 *  <DL>
 *    <DT>*</DT><DD>UNDO_ACTION
 *    <DT>*</DT><DD>REDO_ACTION
 *  </DL>
 */
void
o_undo_callback (GschemToplevel *w_current, PAGE *page, int type)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  UNDO *u_current;
  UNDO *u_next;
  UNDO *save_bottom;
  UNDO *save_tos;
  UNDO *save_current;
  int save_logging;
  int find_prev_data=FALSE;

  char *save_filename;
  gboolean save_untitled;

  g_return_if_fail (w_current != NULL);
  g_return_if_fail (page != NULL);

  if (w_current->undo_control == FALSE) {
    s_log_message(_("Undo/Redo disabled in rc file\n"));
    return;
  }

  if (page->undo_current == NULL) {
    return;
  }

  if (type == UNDO_ACTION) {
    u_current = page->undo_current->prev;
  } else {
    u_current = page->undo_current->next;
  }

  u_next = page->undo_current;

  if (u_current == NULL) {
    return;
  }

  if (u_next->type == UNDO_ALL && u_current->type == UNDO_VIEWPORT_ONLY) {
#if DEBUG
    printf("Type: %d\n", u_current->type);
    printf("Current is an undo all, next is viewport only!\n");
#endif
    find_prev_data = TRUE;

    if (w_current->undo_type == UNDO_DISK) {
      u_current->filename = o_undo_find_prev_filename(u_current);
    } else {
      u_current->object_list = o_undo_find_prev_object_head (u_current);
    }
  }

  /* save filename */
  save_filename = g_strdup (page->page_filename);
  save_untitled = page->is_untitled;

  /* save structure so it's not nuked */
  save_bottom = page->undo_bottom;
  save_tos = page->undo_tos;
  save_current = page->undo_current;
  page->undo_bottom = NULL;
  page->undo_tos = NULL;
  page->undo_current = NULL;

  o_select_unselect_all (w_current);

  if (w_current->undo_type == UNDO_DISK && u_current->filename) {
    /* delete objects of page */
    s_page_delete_objects (toplevel, page);

    /* Free the objects in the place list. */
    s_delete_object_glist (toplevel, page->place_list);
    page->place_list = NULL;
  } else if (w_current->undo_type == UNDO_MEMORY && u_current->object_list) {
    /* delete objects of page */
    s_page_delete_objects (toplevel, page);

    /* Free the objects in the place list. */
    s_delete_object_glist (toplevel, page->place_list);
    page->place_list = NULL;
  }


  /* temporarily disable logging */
  save_logging = do_logging;
  do_logging = FALSE;

  if (w_current->undo_type == UNDO_DISK && u_current->filename) {

    f_open(toplevel, page, u_current->filename, NULL);

  } else if (w_current->undo_type == UNDO_MEMORY && u_current->object_list) {

    s_page_append_list (toplevel, page,
                        o_glist_copy_all (toplevel, u_current->object_list,
                                          NULL));
  }

  page->page_control = u_current->page_control;
  page->up = u_current->up;
  page->CHANGED = u_current->CHANGED;

  /* set filename right */
  g_free(page->page_filename);
  page->page_filename = save_filename;
  page->is_untitled = save_untitled;

  x_pagesel_update (w_current);
  if (page == toplevel->page_current)
    /* change    "Do you want to drop your changes and reload the file?"
       back into "Do you want to reload it?" */
    x_window_update_file_change_notification (w_current, page);

  GschemPageView *view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (view != NULL);

  GschemPageGeometry *geometry = gschem_page_view_get_page_geometry (view);

  if (w_current->undo_panzoom) {
    if (u_current->scale != 0) {
      gschem_page_geometry_set_viewport (geometry,
                                         u_current->x,
                                         u_current->y,
                                         u_current->scale);
      g_signal_emit_by_name (view, "update-grid-info");
      gschem_page_view_update_scroll_adjustments (view);
      gschem_page_view_invalidate_all (view);
    } else {
      gschem_page_view_zoom_extents (view, u_current->object_list);
    }
  } else {
    /* Don't pan/zoom on undo.  However, "Tools / Place Origin"
       translates the whole page contents to somewhere else, so we
       need to counter for that translation.  tx/ty default to zero,
       so this doesn't do anything for other operations. */

    UNDO *u_ognrst;
    int sign;

    if (type == UNDO_ACTION) {
      u_ognrst = u_next;
      sign = -1;
    } else {
      u_ognrst = u_current;
      sign = +1;
    }

    gschem_page_view_pan_general (
      view,
      (geometry->viewport_right +
       geometry->viewport_left) / 2 + sign * u_ognrst->tx,
      (geometry->viewport_top +
       geometry->viewport_bottom) / 2 + sign * u_ognrst->ty, 1.);
    gschem_page_view_invalidate_all (view);
  }

  /* restore logging */
  do_logging = save_logging;

  /* final redraw */
  x_pagesel_update (w_current);
  x_multiattrib_update (w_current);
  i_update_menus(w_current);

  /* restore saved undo structures */
  page->undo_bottom = save_bottom;
  page->undo_tos = save_tos;
  page->undo_current = save_current;

  if (type == UNDO_ACTION) {
    if (page->undo_current) {
      page->undo_current = page->undo_current->prev;
      if (page->undo_current == NULL) {
        page->undo_current = page->undo_bottom;
      }
    }
  } else { /* type is REDO_ACTION */
    if (page->undo_current) {
      page->undo_current = page->undo_current->next;
      if (page->undo_current == NULL) {
        page->undo_current = page->undo_tos;
      }
    }
  }

  /* don't have to free data here since filename, object_list are */
  /* just pointers to the real data (lower in the stack) */
  if (find_prev_data) {
    u_current->filename = NULL;
    u_current->object_list = NULL;
  }

  o_undo_update_actions (w_current, page);

#if DEBUG
  printf("\n\n---Undo----\n");
  s_undo_print_all(page->undo_bottom);
  printf("TOS: %s\n", page->undo_tos->filename);
  printf("CURRENT: %s\n", page->undo_current->filename);
  printf("----\n");
#endif
}

/*! \brief Update sensitivity and text of undo/redo actions.
 */
void
o_undo_update_actions (GschemToplevel *w_current, PAGE *page)
{
  gchar *label, *menu_label;

  gschem_action_set_sensitive (action_edit_undo,
                               page->undo_current != NULL &&
                               page->undo_current->prev != NULL, w_current);
  if (page->undo_current == NULL ||
      page->undo_current->prev == NULL ||
      page->undo_current->desc == NULL) {
    label = NULL;
    menu_label = NULL;
  } else {
    label = g_strdup_printf (_("Undo %s"), page->undo_current->desc);
    menu_label = g_strdup_printf (_("_Undo %s"), page->undo_current->desc);
  }
  gschem_action_set_strings (action_edit_undo, label, label, menu_label,
                             w_current);
  g_free (label);
  g_free (menu_label);

  gschem_action_set_sensitive (action_edit_redo,
                               page->undo_current != NULL &&
                               page->undo_current->next != NULL, w_current);
  if (page->undo_current == NULL ||
      page->undo_current->next == NULL ||
      page->undo_current->next->desc == NULL) {
    label = NULL;
    menu_label = NULL;
  } else {
    label = g_strdup_printf (_("Redo %s"), page->undo_current->next->desc);
    menu_label = g_strdup_printf (_("_Redo %s"),
                                  page->undo_current->next->desc);
  }
  gschem_action_set_strings (action_edit_redo, label, label, menu_label,
                             w_current);
  g_free (label);
  g_free (menu_label);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_undo_cleanup(void)
{
  int i;
  char *filename;

  for (i = 0 ; i < undo_file_index; i++) {
    filename = g_strdup_printf("%s%cgschem.save%d_%d.sch", tmp_path,
                               G_DIR_SEPARATOR, prog_pid, i);
    unlink(filename);
    g_free(filename);
  }

  g_free(tmp_path);
  tmp_path = NULL;
}
