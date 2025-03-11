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
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "gschem.h"
#include <gdk/gdkkeysyms.h>


/* used for the stroke stuff */
static int DOING_STROKE = FALSE;

/*! \brief Redraws the view when widget is exposed.
 *
 *  \param [in] view      The GschemPageView.
 *  \param [in] event     The event structure.
 *  \param [in] w_current The GschemToplevel.
 *  \returns FALSE to propagate the event further.
 */

gint
x_event_expose(GschemPageView *view, GdkEventExpose *event, GschemToplevel *w_current)
{
  gschem_page_view_redraw (view, event, w_current);

  return(0);
}



/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
gint
x_event_raise_dialog_boxes (GschemPageView *view, GdkEventExpose *event, GschemToplevel *w_current)
{
  g_return_val_if_fail (w_current != NULL, 0);

  /* raise the dialog boxes if this feature is enabled */
  if (w_current->raise_dialog_boxes) {
    x_dialog_raise_all(w_current);
  }

  return 0;
}




/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
gint
x_event_button_pressed(GschemPageView *page_view, GdkEventButton *event, GschemToplevel *w_current)
{
  PAGE *page = gschem_page_view_get_page (page_view);
  int w_x, w_y;
  int unsnapped_wx, unsnapped_wy;

  g_return_val_if_fail ((w_current != NULL), 0);

  if (page == NULL) {
    return TRUE; /* terminate event */
  }

  if (!gtk_widget_has_focus (GTK_WIDGET (page_view))) {
    gtk_widget_grab_focus (GTK_WIDGET (page_view));
  }

  scm_dynwind_begin (0);
  g_dynwind_window (w_current);

#if DEBUG
  printf("pressed button %d! \n", event->button);
  printf("event state: %d \n", event->state);
  printf("w_current state: %d \n", w_current->event_state);
  printf("Selection is:\n");
  o_selection_print_all(&(page->selection_list));
  printf("\n");
#endif

  gschem_page_view_SCREENtoWORLD (page_view, (int) event->x, (int) event->y,
                                  &unsnapped_wx, &unsnapped_wy);
  w_x = snap_grid (w_current, unsnapped_wx);
  w_y = snap_grid (w_current, unsnapped_wy);

  if (event->type == GDK_2BUTTON_PRESS &&
      w_current->event_state == SELECT) {
    /* Don't re-select an object (lp-912978) */
    /* o_find_object(w_current, w_x, w_y, TRUE); */

    /* GDK_BUTTON_EVENT is emitted before GDK_2BUTTON_EVENT, which
     * leads to setting of the inside_action flag.  If o_edit()
     * brings up a modal window (e.g., the edit attribute dialog),
     * it intercepts the release button event and thus doesn't
     * allow resetting of the inside_action flag so we do it
     * manually here before processing the double-click event. */
    i_action_stop (w_current);
    o_edit (w_current, geda_list_get_glist (page->selection_list), TRUE);
    scm_dynwind_end ();
    return(0);
  }

  w_current->SHIFTKEY   = (event->state & GDK_SHIFT_MASK  ) ? 1 : 0;
  w_current->CONTROLKEY = (event->state & GDK_CONTROL_MASK) ? 1 : 0;
  w_current->ALTKEY     = (event->state & GDK_MOD1_MASK) ? 1 : 0;

  /* Huge switch statement to evaluate state transitions. Jump to
   * end_button_pressed label to escape the state evaluation rather than
   * returning from the function directly. */

  if (event->button == 1) {
    if (w_current->inside_action) {
      /* End action */
      if (page->place_list != NULL) {
        switch(w_current->event_state) {
          case (COMPMODE)   : o_place_end(w_current, w_x, w_y,
                                w_current->continue_component_place, FALSE,
                                "%add-objects-hook", _("Add Component")); break;
          case (TEXTMODE)   : o_place_end(w_current, w_x, w_y, FALSE, FALSE,
                                "%add-objects-hook", _("Add Text")); break;
          case (PASTEMODE)  : o_place_end(w_current, w_x, w_y, FALSE, TRUE,
                                "%paste-objects-hook", _("Paste")); break;
          default: break;
        }
      } else {
        switch(w_current->event_state) {
          case (ARCMODE)    : o_arc_end1(w_current, w_x, w_y); break;
          case (BOXMODE)    : o_box_end(w_current, w_x, w_y); break;
          case (BUSMODE)    : o_bus_end(w_current, w_x, w_y); break;
          case (CIRCLEMODE) : o_circle_end(w_current, w_x, w_y); break;
          case (LINEMODE)   : o_line_end(w_current, w_x, w_y); break;
          case (NETMODE)    : o_net_end(w_current, w_x, w_y); break;
          case (PATHMODE)   : o_path_continue (w_current, w_x, w_y); break;
          case (PICTUREMODE): o_picture_end(w_current, w_x, w_y); break;
          case (PINMODE)    : o_pin_end (w_current, w_x, w_y); break;
          case (OGNRSTMODE) : o_ognrst_end (w_current, w_x, w_y); break;
          default: break;
        }
      }
    } else {
      /* Start action */
      switch(w_current->event_state) {
        case (ARCMODE)    : o_arc_start(w_current, w_x, w_y); break;
        case (BOXMODE)    : o_box_start(w_current, w_x, w_y); break;
        case (BUSMODE)    : o_bus_start(w_current, w_x, w_y); break;
        case (CIRCLEMODE) : o_circle_start(w_current, w_x, w_y); break;
        case (LINEMODE)   : o_line_start(w_current, w_x, w_y); break;
        case (NETMODE)    : o_net_start(w_current, w_x, w_y); break;
        case (PATHMODE)   : o_path_start (w_current, w_x, w_y); break;
        case (PICTUREMODE): o_picture_start(w_current, w_x, w_y); break;
        case (PINMODE)    : o_pin_start (w_current, w_x, w_y); break;
        case (ZOOMBOX)    : a_zoom_box_start(w_current, unsnapped_wx, unsnapped_wy); break;
        case (SELECT)     : o_select_start(w_current, w_x, w_y); break;

        case (COPYMODE)   :
        case (MCOPYMODE)  : o_copy_start(w_current, w_x, w_y); break;
        case (MOVEMODE)   : o_move_start(w_current, w_x, w_y); break;
        default: break;
      }
    }

    switch(w_current->event_state) {
      case(ROTATEMODE):   o_rotate_world_update(w_current, w_x, w_y, 90,
                            geda_list_get_glist(page->selection_list)); break;
      case(MIRRORMODE):   o_mirror_world_update(w_current, w_x, w_y,
                            geda_list_get_glist(page->selection_list)); break;

      case(PAN):
        gschem_page_view_pan (page_view, w_x, w_y);
        if (w_current->undo_panzoom)
          o_undo_savestate (w_current, page, UNDO_VIEWPORT_ONLY, _("Pan"));
        i_set_state(w_current, SELECT);
        break;
    }
  } else if (event->button == 2) {

    /* try this out and see how it behaves */
    if (w_current->inside_action) {
      if (w_current->event_state == OGNRSTMODE &&
          w_current->middle_button == MID_MOUSEPAN_ENABLED)
        gschem_page_view_pan_start (page_view, (int) event->x, (int) event->y);
      else if (
          !(w_current->event_state == COMPMODE||
            w_current->event_state == TEXTMODE||
            w_current->event_state == MOVEMODE||
            w_current->event_state == COPYMODE  ||
            w_current->event_state == MCOPYMODE ||
            w_current->event_state == PASTEMODE )) {
        i_cancel (w_current);
      }
      goto end_button_pressed;
    }

    switch(w_current->middle_button) {

      case(ACTION):

      /* don't want to search if shift */
      /* key is pressed */
      if (!w_current->SHIFTKEY) {
        o_find_object(w_current, unsnapped_wx, unsnapped_wy, TRUE);
      }

      /* make sure the list is not empty */
      if (!o_select_selected(w_current)) {
        /* this means the above find did not
         * find anything */
        i_action_stop (w_current);
        i_set_state(w_current, SELECT);
        goto end_button_pressed;
      }

      /* determine here if copy or move */
      if (w_current->ALTKEY) {
        i_set_state(w_current, COPYMODE);
        o_copy_start(w_current, w_x, w_y);
      } else {
        o_move_start(w_current, w_x, w_y);
      }
      break;

      case(REPEAT):
      if (w_current->last_action != NULL) {
        scm_dynwind_begin (0);
        g_dynwind_window (w_current);
        g_scm_eval_protected (scm_list_2 (
                                scm_variable_ref (
                                  scm_c_public_variable (
                                    "gschem action",
                                    "eval-action-at-point!")),
                                w_current->last_action->smob),
                              SCM_UNDEFINED);
        scm_dynwind_end ();
      }
      break;
      case(STROKE):
      DOING_STROKE=TRUE;
      break;

      case(MID_MOUSEPAN_ENABLED):
      gschem_page_view_pan_start (page_view, (int) event->x, (int) event->y);
      break;
    }

  } else if (event->button == 3) {
    if (!w_current->inside_action) {
      if (w_current->third_button == POPUP_ENABLED) {
        /* (third-button "popup") */
        i_update_menus(w_current);  /* update menus before popup  */
        if (w_current->popup_menu != NULL)
          gtk_menu_popup (GTK_MENU (w_current->popup_menu),
                          NULL, NULL, NULL, NULL,
                          event->button, event->time);
      } else {
        /* (third-button "mousepan") */
        gschem_page_view_pan_start (page_view, (int) event->x, (int) event->y);
      }
    } else {
      if ((w_current->third_button == MOUSEPAN_ENABLED) &&
          (!w_current->third_button_cancel)) {
        gschem_page_view_pan_start (page_view, (int) event->x, (int) event->y);
      } else { /* this is the default cancel */

        /* reset all draw and place actions */

        switch (w_current->event_state) {

          case (ARCMODE)    : o_arc_invalidate_rubber     (w_current); break;
          case (BOXMODE)    : o_box_invalidate_rubber     (w_current); break;
          case (BUSMODE)    : o_bus_invalidate_rubber     (w_current); break;
          case (CIRCLEMODE) : o_circle_invalidate_rubber  (w_current); break;
          case (LINEMODE)   : o_line_invalidate_rubber    (w_current); break;
          case (NETMODE)    : o_net_reset                 (w_current); break;
          case (PATHMODE)   : o_path_end_path             (w_current); break;
          case (PICTUREMODE): o_picture_invalidate_rubber (w_current); break;
          case (PINMODE)    : o_pin_invalidate_rubber     (w_current); break;

          default:
            i_cancel (w_current);
            break;
        }
      }
    }
  }

 end_button_pressed:
  scm_dynwind_end ();

  return(0);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
gint
x_event_button_released (GschemPageView *page_view, GdkEventButton *event, GschemToplevel *w_current)
{
  PAGE *page = gschem_page_view_get_page (page_view);
  int unsnapped_wx, unsnapped_wy;
  int w_x, w_y;

  g_return_val_if_fail ((page_view != NULL), 0);
  g_return_val_if_fail ((w_current != NULL), 0);

  if (page == NULL) {
    return TRUE; /* terminate event */
  }

#if DEBUG
  printf("released! %d \n", w_current->event_state);
#endif

  w_current->SHIFTKEY   = (event->state & GDK_SHIFT_MASK  ) ? 1 : 0;
  w_current->CONTROLKEY = (event->state & GDK_CONTROL_MASK) ? 1 : 0;
  w_current->ALTKEY     = (event->state & GDK_MOD1_MASK) ? 1 : 0;

  gschem_page_view_SCREENtoWORLD (page_view, (int) event->x, (int) event->y,
                                  &unsnapped_wx, &unsnapped_wy);
  w_x = snap_grid (w_current, unsnapped_wx);
  w_y = snap_grid (w_current, unsnapped_wy);

  /* Huge switch statement to evaluate state transitions. Jump to
   * end_button_released label to escape the state evaluation rather
   * than returning from the function directly. */
  scm_dynwind_begin (0);
  g_dynwind_window (w_current);

  if (event->button == 1) {

    if (w_current->inside_action) {
      if (page->place_list != NULL) {
        switch(w_current->event_state) {
          case (COPYMODE)  :
          case (MCOPYMODE) : o_copy_end(w_current); break;
          case (MOVEMODE)  : o_move_end(w_current); break;
          default: break;
        }
      } else {
        switch(w_current->event_state) {
          case (GRIPS)     : o_grips_end(w_current); break;
          case (PATHMODE)  : o_path_end (w_current, w_x, w_y); break;
          case (SBOX)      : o_select_box_end(w_current, unsnapped_wx, unsnapped_wy); break;
          case (SELECT)    : o_select_end(w_current, unsnapped_wx, unsnapped_wy); break;
          case (ZOOMBOX)   : a_zoom_box_end(w_current, unsnapped_wx, unsnapped_wy); break;
          default: break;
        }
      }
    }
  } else if (event->button == 2) {

    if (w_current->inside_action) {
      if (w_current->event_state == COMPMODE||
          w_current->event_state == TEXTMODE||
          w_current->event_state == MOVEMODE||
          w_current->event_state == COPYMODE  ||
          w_current->event_state == MCOPYMODE ||
          w_current->event_state == PASTEMODE ) {

        if (w_current->event_state == MOVEMODE) {
          o_move_invalidate_rubber (w_current, FALSE);
        } else {
          o_place_invalidate_rubber (w_current, FALSE);
        }
        w_current->rubber_visible = 0;

        o_place_rotate(w_current);

        if (w_current->event_state == COMPMODE) {
          o_complex_place_changed_run_hook (w_current);
        }

        if (w_current->event_state == MOVEMODE) {
          o_move_invalidate_rubber (w_current, TRUE);
        } else {
          o_place_invalidate_rubber (w_current, TRUE);
        }
        w_current->rubber_visible = 1;
        goto end_button_released;
      }
    }

    switch(w_current->middle_button) {
      case(ACTION):
        if (w_current->inside_action && (page->place_list != NULL)) {
          switch(w_current->event_state) {
            case (COPYMODE): o_copy_end(w_current); break;
            case (MOVEMODE): o_move_end(w_current); break;
          }
        }
      break;

      case(STROKE):
        DOING_STROKE = FALSE;
        x_stroke_translate_and_execute (w_current);
      break;

      case(MID_MOUSEPAN_ENABLED):
        if (gschem_page_view_pan_end (page_view) && w_current->undo_panzoom) {
          o_undo_savestate_old (w_current, UNDO_VIEWPORT_ONLY, _("Pan"));
        }
      break;
    }

  } else if (event->button == 3) {
      /* just for ending a mouse pan */
      if (gschem_page_view_pan_end (page_view) && w_current->undo_panzoom) {
        o_undo_savestate_old (w_current, UNDO_VIEWPORT_ONLY, _("Pan"));
      }
  }
 end_button_released:
  scm_dynwind_end ();

  return(0);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
gint
x_event_motion (GschemPageView *page_view, GdkEventMotion *event, GschemToplevel *w_current)
{
  PAGE *page = gschem_page_view_get_page (page_view);
  int w_x, w_y;
  int unsnapped_wx, unsnapped_wy;
  int skip_event=0;
  GdkEvent *test_event;

  g_return_val_if_fail ((w_current != NULL), 0);

  if (page == NULL) {
    return TRUE; /* terminate event */
  }

  w_current->SHIFTKEY   = (event->state & GDK_SHIFT_MASK  ) ? 1 : 0;
  w_current->CONTROLKEY = (event->state & GDK_CONTROL_MASK) ? 1 : 0;
  w_current->ALTKEY     = (event->state & GDK_MOD1_MASK) ? 1 : 0;

#if DEBUG
  /*  printf("MOTION!\n");*/
#endif

  if (DOING_STROKE == TRUE) {
    x_stroke_record (w_current, event->x, event->y);
    return(0);
  }

  /* skip the moving event if there are other moving events in the
     gdk event queue (Werner)
     Only skip the event if is the same event and no buttons or modifier
     keys changed*/
  if ((test_event = gdk_event_get()) != NULL) {
    if (test_event->type == GDK_MOTION_NOTIFY
        && ((GdkEventMotion *) test_event)->state == event->state) {
      skip_event= 1;
    }
    gdk_event_put(test_event); /* put it back in front of the queue */
    gdk_event_free(test_event);
    if (skip_event == 1)
      return 0;
  }

  gschem_page_view_SCREENtoWORLD (page_view, (int) event->x, (int) event->y,
                                  &unsnapped_wx, &unsnapped_wy);
  w_x = snap_grid (w_current, unsnapped_wx);
  w_y = snap_grid (w_current, unsnapped_wy);

  gschem_bottom_widget_set_coordinates (
    GSCHEM_BOTTOM_WIDGET (w_current->bottom_widget), w_x, w_y);

  gschem_page_view_pan_motion (page_view, w_current->mousepan_gain, (int) event->x, (int) event->y);

  /* Huge switch statement to evaluate state transitions. Jump to
   * end_motion label to escape the state evaluation rather
   * than returning from the function directly. */
  scm_dynwind_begin (0);
  g_dynwind_window (w_current);

  if (w_current->inside_action) {
    if (page->place_list != NULL) {
      switch(w_current->event_state) {
        case (COPYMODE)   :
        case (MCOPYMODE)  :
        case (COMPMODE)   :
        case (PASTEMODE)  :
        case (TEXTMODE)   : o_place_motion (w_current, w_x, w_y); break;
        case (MOVEMODE)   : o_move_motion (w_current, w_x, w_y); break;
        default: break;
      }
    } else {
      switch(w_current->event_state) {
        case (ARCMODE)    : o_arc_motion (w_current, w_x, w_y, ARC_RADIUS); break;
        case (BOXMODE)    : o_box_motion  (w_current, w_x, w_y); break;
        case (BUSMODE)    : o_bus_motion (w_current, w_x, w_y); break;
        case (CIRCLEMODE) : o_circle_motion (w_current, w_x, w_y); break;
        case (LINEMODE)   : o_line_motion (w_current, w_x, w_y); break;
        case (NETMODE)    : o_net_motion (w_current, w_x, w_y); break;
        case (PATHMODE)   : o_path_motion (w_current, w_x, w_y); break;
        case (PICTUREMODE): o_picture_motion (w_current, w_x, w_y); break;
        case (PINMODE)    : o_pin_motion (w_current, w_x, w_y); break;
        case (GRIPS)      : o_grips_motion(w_current, w_x, w_y); break;
        case (SBOX)       : o_select_box_motion (w_current, unsnapped_wx, unsnapped_wy); break;
        case (ZOOMBOX)    : a_zoom_box_motion (w_current, unsnapped_wx, unsnapped_wy); break;
        case (SELECT)     : o_select_motion (w_current, w_x, w_y); break;
        case (OGNRSTMODE) : o_ognrst_motion (w_current, w_x, w_y); break;
        default: break;
      }
    }
  } else {
    switch(w_current->event_state) {
      case(NETMODE)    :   o_net_start_magnetic(w_current, w_x, w_y); break;
      default: break;
    }
  }

  scm_dynwind_end ();

  return(0);
}

/*! \brief Updates the display when drawing area is configured.
 *  \par Function Description
 *  This is the callback function connected to the configure event of
 *  the GschemPageView of the main window.
 *
 *  It re-pans each of its pages to keep their contents centered in the
 *  GschemPageView.
 *
 *  When the window is maximised, the zoom of every page is changed to
 *  best fit the previously displayed area of the page in the new
 *  area. Otherwise the current zoom level is left unchanged.
 *
 *  \param [in] widget    The GschemPageView which received the signal.
 *  \param [in] event     The event structure of signal configure-event.
 *  \param [in] unused
 *  \returns FALSE to propagate the event further.
 */
gboolean
x_event_configure (GschemPageView    *page_view,
                   GdkEventConfigure *event,
                   gpointer           unused)
{
  GtkAllocation current_allocation;
  GList *iter;
  PAGE *p_current = gschem_page_view_get_page (page_view);

  if (p_current == NULL) {
    /* don't want to call this if the current page isn't setup yet */
    return FALSE;
  }

  g_return_val_if_fail (p_current->toplevel != NULL, FALSE);

  gtk_widget_get_allocation (GTK_WIDGET(page_view), &current_allocation);

  if ((current_allocation.width == page_view->previous_allocation.width) &&
      (current_allocation.height == page_view->previous_allocation.height)) {
    /* the size of the drawing area has not changed -- nothing to do here */
    return FALSE;
  }

  page_view->previous_allocation = current_allocation;

  /* re-pan each page of the TOPLEVEL */
  for ( iter = geda_list_get_glist (p_current->toplevel->pages);
        iter != NULL;
        iter = g_list_next (iter) ) {

    gschem_page_view_set_page (page_view, (PAGE *)iter->data);

    if (page_view->configured) {
      gschem_page_view_pan_mouse (page_view, 0, 0);
    } else {
      gschem_page_view_zoom_extents (page_view, NULL);
    }
  }

  page_view->configured = TRUE;

  gschem_page_view_set_page (page_view, p_current);

  return FALSE;
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
gint x_event_enter(GtkWidget *widget, GdkEventCrossing *event,
                   GschemToplevel *w_current)
{
  g_return_val_if_fail ((w_current != NULL), 0);
  /* do nothing or now */
  return(0);
}

/*! \brief Callback to handle key events in the drawing area.
 *  \par Function Description
 *
 *  GTK+ callback function (registered in x_window_setup_draw_events() ) which
 *  handles key press and release events from the GTK+ system.
 *
 * \param [in] widget     the widget that generated the event
 * \param [in] event      the event itself
 * \param      w_current  the toplevel environment
 * \returns TRUE if the event has been handled.
 */
gboolean
x_event_key (GschemPageView *page_view, GdkEventKey *event, GschemToplevel *w_current)
{
  gboolean retval = FALSE;
  int pressed;
  gboolean special = FALSE;

  g_return_val_if_fail (page_view != NULL, FALSE);

#if DEBUG
  printf("x_event_key_pressed: Pressed key %i.\n", event->keyval);
#endif

  /* update the state of the modifiers */
  w_current->ALTKEY     = (event->state & GDK_MOD1_MASK)    ? 1 : 0;
  w_current->SHIFTKEY   = (event->state & GDK_SHIFT_MASK)   ? 1 : 0;
  w_current->CONTROLKEY = (event->state & GDK_CONTROL_MASK) ? 1 : 0;

  pressed = (event->type == GDK_KEY_PRESS) ? 1 : 0;

  switch (event->keyval) {
    case GDK_Alt_L:
    case GDK_Alt_R:
      w_current->ALTKEY = pressed;
      break;

    case GDK_Shift_L:
    case GDK_Shift_R:
      w_current->SHIFTKEY = pressed;
      special = TRUE;
      break;

    case GDK_Control_L:
    case GDK_Control_R:
      w_current->CONTROLKEY = pressed;
      special = TRUE;
      break;
  }

  scm_dynwind_begin (0);
  g_dynwind_window (w_current);

  /* Special case to update the object being drawn or placed after
   * scrolling when Shift or Control were pressed */
  if (special) {
    x_event_faked_motion (page_view, event);
  }

  if (pressed)
    retval = g_keys_execute (w_current, event) ? TRUE : FALSE;

  scm_dynwind_end ();

  return retval;
}


/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 *  \param [in] widget The GschemPageView with the scroll event.
 *  \param [in] event
 *  \param [in] w_current
 */
gint x_event_scroll (GtkWidget *widget, GdkEventScroll *event,
                     GschemToplevel *w_current)
{
  GtkAdjustment *adj;
  gboolean pan_xaxis = FALSE;
  gboolean pan_yaxis = FALSE;
  gboolean zoom = FALSE;
  int pan_direction = 1;
  int zoom_direction = ZOOM_IN;
  GschemPageView *view = NULL;
  PAGE *page = NULL;

  g_return_val_if_fail ((w_current != NULL), 0);

  view = GSCHEM_PAGE_VIEW (widget);
  g_return_val_if_fail ((view != NULL), 0);

  page = gschem_page_view_get_page (view);

  if (page == NULL) {
    return FALSE; /* we cannot zoom page if it doesn't exist :) */
  }

  /* update the state of the modifiers */
  w_current->SHIFTKEY   = (event->state & GDK_SHIFT_MASK  ) ? 1 : 0;
  w_current->CONTROLKEY = (event->state & GDK_CONTROL_MASK) ? 1 : 0;
  w_current->ALTKEY     = (event->state & GDK_MOD1_MASK) ? 1 : 0;

  if (w_current->scroll_wheel == SCROLL_WHEEL_CLASSIC) {
    /* Classic gschem behaviour */
    zoom =      !w_current->CONTROLKEY && !w_current->SHIFTKEY;
    pan_yaxis = !w_current->CONTROLKEY &&  w_current->SHIFTKEY;
    pan_xaxis =  w_current->CONTROLKEY && !w_current->SHIFTKEY;
  } else {
    /* GTK style behaviour */
    zoom =       w_current->CONTROLKEY && !w_current->SHIFTKEY;
    pan_yaxis = !w_current->CONTROLKEY && !w_current->SHIFTKEY;
    pan_xaxis = !w_current->CONTROLKEY &&  w_current->SHIFTKEY;
  }

  /* If the user has a left/right scroll wheel, always scroll the y-axis */
  if (event->direction == GDK_SCROLL_LEFT ||
      event->direction == GDK_SCROLL_RIGHT) {
    zoom = FALSE;
    pan_yaxis = FALSE;
    pan_xaxis = TRUE;
  }

  /* You must have scrollbars enabled if you want to use the scroll wheel to pan */
  if (!w_current->scrollbars_flag) {
    pan_xaxis = FALSE;
    pan_yaxis = FALSE;
  }

  switch (event->direction) {
    case GDK_SCROLL_UP:
    case GDK_SCROLL_LEFT:
      pan_direction = -1;
      zoom_direction = ZOOM_IN;
      break;
    case GDK_SCROLL_DOWN:
    case GDK_SCROLL_RIGHT:
      pan_direction =  1;
      zoom_direction = ZOOM_OUT;
      break;
  }

  if (zoom) {
    /*! \todo Change "HOTKEY" TO new "MOUSE" specifier? */
    a_zoom(w_current, GSCHEM_PAGE_VIEW (widget), zoom_direction, HOTKEY);
  }

  if (pan_xaxis) {
    adj = gschem_page_view_get_hadjustment (GSCHEM_PAGE_VIEW (widget));
    g_return_val_if_fail (adj != NULL, TRUE);
    gtk_adjustment_set_value(adj, min(adj->value + pan_direction *
                                        (adj->page_increment /
                                         w_current->scrollpan_steps),
                                      adj->upper - adj->page_size));
  }

  if (pan_yaxis) {
    adj = gschem_page_view_get_vadjustment (GSCHEM_PAGE_VIEW (widget));
    g_return_val_if_fail (adj != NULL, TRUE);
    gtk_adjustment_set_value(adj, min(adj->value + pan_direction *
                                        (adj->page_increment /
                                         w_current->scrollpan_steps),
                                      adj->upper - adj->page_size));
  }

  if (w_current->undo_panzoom && (zoom || pan_xaxis || pan_yaxis)) {
    o_undo_savestate_old (w_current, UNDO_VIEWPORT_ONLY,
                          zoom ? _("Zoom") : _("Pan"));
  }

  x_event_faked_motion (view, NULL);
  /* Stop further processing of this signal */
  return TRUE;
}


/*! \brief get the pointer position of a given GschemToplevel
 *  \par Function Description
 *  This function gets the pointer position of the drawing area of the
 *  current workspace <b>GschemToplevel</b>. The flag <b>snapped</b> specifies
 *  whether the pointer position should be snapped to the current grid.
 *
 *  \param [in] w_current  The GschemToplevel object.
 *  \param [in] snapped    An option flag to specify the wished coords
 *  \param [out] wx        snapped/unsnapped world x coordinate
 *  \param [out] wy        snapped/unsnapped world y coordinate
 *
 *  \return Returns TRUE if the pointer position is inside the drawing area.
 *
 */
gboolean
x_event_get_pointer_position (GschemToplevel *w_current, gboolean snapped, gint *wx, gint *wy)
{
  int width;
  int height;
  int sx;
  int sy;
  int x;
  int y;

  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_val_if_fail (page_view != NULL, FALSE);

  g_return_val_if_fail (GTK_WIDGET (page_view)->window != NULL, FALSE);

  /* \todo The following line is depricated in GDK 2.24 */
  gdk_drawable_get_size (GTK_WIDGET (page_view)->window, &width, &height);

  gtk_widget_get_pointer(GTK_WIDGET (page_view), &sx, &sy);

  /* check if we are inside the drawing area */
  if ((sx < 0) || (sx >= width) || (sy < 0) || (sy >= height)) {
    return FALSE;
  }

  gschem_page_view_SCREENtoWORLD (page_view, sx, sy, &x, &y);

  if (snapped) {
    x = snap_grid (w_current, x);
    y = snap_grid (w_current, y);
  }

  *wx = x;
  *wy = y;

  return TRUE;
}

/*! \brief Emits a faked motion event to update objects being drawn or placed
 *  \par Function Description
 *  This function emits an additional "motion-notify-event" to
 *  update objects being drawn or placed while zooming, scrolling, or
 *  panning.
 *
 *  If its event parameter is not NULL, the current state of Shift
 *  and Control is preserved to correctly deal with special cases.
 *
 *  \param [in] view      The GschemPageView object which received the signal.
 *  \param [in] event     The event structure of the signal or NULL.
 *  \returns FALSE to propagate the event further.
 */
gboolean
x_event_faked_motion (GschemPageView *view, GdkEventKey *event) {
  gint x, y;
  gboolean ret;
  GdkEventMotion *newevent;

  gtk_widget_get_pointer (GTK_WIDGET (view), &x, &y);
  newevent = (GdkEventMotion*)gdk_event_new(GDK_MOTION_NOTIFY);
  newevent->x = x;
  newevent->y = y;

  if (event != NULL ) {
    switch (event->keyval) {
      case GDK_Control_L:
      case GDK_Control_R:
        if (event->type == GDK_KEY_PRESS) {
          newevent->state |= GDK_CONTROL_MASK;
        } else {
          newevent->state &= ~GDK_CONTROL_MASK;
        }
        break;

      case GDK_Shift_L:
      case GDK_Shift_R:
        if (event->type == GDK_KEY_PRESS) {
          newevent->state |= GDK_SHIFT_MASK;
        } else {
          newevent->state &= ~GDK_SHIFT_MASK;
        }
        break;
    }
  }

  g_signal_emit_by_name (view, "motion-notify-event", newevent, &ret);

  gdk_event_free((GdkEvent*)newevent);

  return FALSE;
}
