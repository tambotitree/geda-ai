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
#include <math.h>

#include "libgeda_priv.h"

/*! \file o_pin_basic.c
 *  \brief functions for the pin object
 */

/*! \brief calculate and return the boundaries of a pin object
 *  \par Function Description
 *  This function calculates the object boudaries of a pin \a object.
 *
 *  \param [in]  toplevel  The TOPLEVEL object.
 *  \param [in]  object    a pin object
 *  \param [out] left      the left world coord
 *  \param [out] top       the top world coord
 *  \param [out] right     the right world coord
 *  \param [out] bottom    the bottom world coord
 */
void world_get_pin_bounds(TOPLEVEL *toplevel, OBJECT *object, int *left, int *top,
			  int *right, int *bottom)
{
  world_get_line_bounds( toplevel, object, left, top, right, bottom );
}

/*! \brief get the position of a whichend of the pin object
 *  \par Function Description
 *  This function gets the position of the whichend side of a pin object.
 *
 *  \param [in] object   The object to get the position.
 *  \param [out] x       pointer to the x-position
 *  \param [out] y       pointer to the y-position
 *  \return TRUE if successfully determined the position, FALSE otherwise
 */
gboolean o_pin_get_position (OBJECT *object, gint *x, gint *y)
{
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (object->type == OBJ_PIN, FALSE);
  g_return_val_if_fail (object->line != NULL, FALSE);
  g_return_val_if_fail (object->whichend >= 0, FALSE);
  g_return_val_if_fail (object->whichend < 2, FALSE);

  if (x != NULL) {
    *x = object->line->x[object->whichend];
  }

  if (y != NULL) {
    *y = object->line->y[object->whichend];
  }

  return TRUE;
}

/*! \brief create a new pin object
 *  \par Function Description
 *  This function creates and returns a new pin object.
 *  
 *  \param [in]     toplevel    The TOPLEVEL object.
 *  \param [in]     color       The color of the pin
 *  \param [in]     x1          x-coord of the first point
 *  \param [in]     y1          y-coord of the first point
 *  \param [in]     x2          x-coord of the second point
 *  \param [in]     y2          y-coord of the second point
 *  \param [in]     pin_type    type of pin (PIN_TYPE_NET or PIN_TYPE_BUS)
 *  \param [in]     whichend    The connectable end of the pin
 *  \return A new pin OBJECT
 */
OBJECT *o_pin_new(TOPLEVEL *toplevel,
		  int color,
		  int x1, int y1, int x2, int y2, int pin_type, int whichend)
{
  OBJECT *new_node;

  new_node = s_basic_new_object(OBJ_PIN, "pin");
  new_node->color = color;

  new_node->line = (LINE *) g_malloc(sizeof(LINE));

  new_node->line->x[0] = x1;
  new_node->line->y[0] = y1;
  new_node->line->x[1] = x2;
  new_node->line->y[1] = y2;

  o_pin_set_type (toplevel, new_node, pin_type);

  new_node->w_bounds_valid_for = NULL;

  new_node->whichend = whichend;

  return new_node;
}

/*! \brief read a pin object from a char buffer
 *  \par Function Description
 *  This function reads a pin object from the buffer \a buf.
 *  If the pin object was read successfully, a new pin object is
 *  allocated and appended to the \a object_list.
 *
 *  \param [in] toplevel     The TOPLEVEL object
 *  \param [in] buf          a text buffer (usually a line of a schematic file)
 *  \param [in] release_ver  The release number gEDA
 *  \param [in] fileformat_ver a integer value of the file format
 *  \return The object list, or NULL on error.
 */
OBJECT *o_pin_read (TOPLEVEL *toplevel, const char buf[],
                    unsigned int release_ver, unsigned int fileformat_ver, GError **err)
{
  OBJECT *new_obj;
  char type; 
  int x1, y1;
  int x2, y2;
  int color;
  int pin_type;
  int whichend;

  if (release_ver <= VERSION_20020825) {
    if (sscanf (buf, "%c %d %d %d %d %d\n", &type, &x1, &y1, &x2, &y2, &color) != 6) {
      g_set_error(err, EDA_ERROR, EDA_ERROR_PARSE, _("Failed to parse pin object"));
      return NULL;
    }
    pin_type = PIN_TYPE_NET;
    whichend = -1;
  } else {
    if (sscanf (buf, "%c %d %d %d %d %d %d %d\n", &type, &x1, &y1, &x2, &y2,
		&color, &pin_type, &whichend) != 8) {
      g_set_error(err, EDA_ERROR, EDA_ERROR_PARSE, _("Failed to parse pin object"));
      return NULL;
    }
  }

  if (whichend == -1) {
    s_log_message (_("Found a pin which did not have the whichend field set.\n"
                     "Verify and correct manually.\n"));
  } else if (whichend < -1 || whichend > 1) {
    s_log_message (_("Found an invalid whichend on a pin (reseting to zero): %d\n"),
                   whichend);
    whichend = 0;
  }

  if (color < 0 || color >= MAX_OBJECT_COLORS) {
    s_log_message (_("Found an invalid color [ %s ]\n"), buf);
    s_log_message (_("Setting color to default color\n"));
    color = DEFAULT_COLOR;
  }

  new_obj = o_pin_new (toplevel, color, x1, y1, x2, y2,
                       pin_type, whichend);

  return new_obj;
}

/*! \brief Create a string representation of the pin object
 *
 *  This function takes a pin \a object and returns a string
 *  according to the file format definition.
 *
 *  On failure, this function returns NULL.
 *
 *  The caller must free the returned string when no longer needed using
 *  g_free().
 *
 *  \param [in] object a pin object
 *  \return a string representation of the pin object
 */
char *o_pin_save(OBJECT *object)
{
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (object->line != NULL, NULL);
  g_return_val_if_fail (object->type == OBJ_PIN, NULL);

  return g_strdup_printf ("%c %d %d %d %d %d %d %d",
                          OBJ_PIN,
                          object->line->x[0],
                          object->line->y[0],
                          object->line->x[1],
                          object->line->y[1],
                          object->color,
                          object->pin_type,
                          object->whichend);
}

/*! \brief move a pin object
 *  \par Function Description
 *  This function changes the position of a pin \a object.
 *
 *  \param [ref] object      The pin OBJECT to be moved
 *  \param [in] dx           The x-distance to move the object
 *  \param [in] dy           The y-distance to move the object
 */
void o_pin_translate_world(OBJECT *object, int dx, int dy)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->line != NULL);
  g_return_if_fail (object->type == OBJ_PIN);

  /* Update world coords */
  object->line->x[0] = object->line->x[0] + dx;
  object->line->y[0] = object->line->y[0] + dy;
  object->line->x[1] = object->line->x[1] + dx;
  object->line->y[1] = object->line->y[1] + dy;

  /* Update bounding box */
  object->w_bounds_valid_for = NULL;
}

/*! \brief create a copy of a pin object
 *  \par Function Description
 *  This function creates a copy of the pin object \a o_current.
 *
 *  \param [in] toplevel     The TOPLEVEL object
 *  \param [in] o_current    The object that is copied
 *  \return a new pin object
 */
OBJECT *o_pin_copy(TOPLEVEL *toplevel, OBJECT *o_current)
{
  OBJECT *new_obj;

  g_return_val_if_fail (o_current != NULL, NULL);
  g_return_val_if_fail (o_current->line != NULL, NULL);
  g_return_val_if_fail (o_current->type == OBJ_PIN, NULL);

  new_obj = o_pin_new (toplevel, o_current->color,
                       o_current->line->x[0], o_current->line->y[0],
                       o_current->line->x[1], o_current->line->y[1],
                       o_current->pin_type, o_current->whichend);

  return new_obj;
}

/*! \brief rotate a pin object around a centerpoint
 *  \par Function Description
 *  This function rotates a pin \a object around the point
 *  (\a world_centerx, \a world_centery).
 *  
 *  \param [in] toplevel      The TOPLEVEL object
 *  \param [in] world_centerx x-coord of the rotation center
 *  \param [in] world_centery y-coord of the rotation center
 *  \param [in] angle         The angle to rotat the pin object
 *  \param [in] object        The pin object
 *  \note only steps of 90 degrees are allowed for the \a angle
 */
void o_pin_rotate_world(TOPLEVEL *toplevel, int world_centerx,
			int world_centery, int angle,
			OBJECT *object)
{
  int newx, newy;

  g_return_if_fail (object != NULL);
  g_return_if_fail (object->line != NULL);
  g_return_if_fail (object->type == OBJ_PIN);

  if (angle == 0)
    return;

  /* translate object to origin */
  o_pin_translate_world(object, -world_centerx, -world_centery);

  rotate_point_90(object->line->x[0], object->line->y[0], angle,
                  &newx, &newy);

  object->line->x[0] = newx;
  object->line->y[0] = newy;

  rotate_point_90(object->line->x[1], object->line->y[1], angle,
                  &newx, &newy);

  object->line->x[1] = newx;
  object->line->y[1] = newy;

  o_pin_translate_world(object, world_centerx, world_centery);
}

/*! \brief mirror a pin object horizontaly at a centerpoint
 *  \par Function Description
 *  This function mirrors a pin \a object horizontaly at the point
 *  (\a world_centerx, \a world_centery).
 *  
 *  \param [in] toplevel      The TOPLEVEL object
 *  \param [in] world_centerx x-coord of the mirror position
 *  \param [in] world_centery y-coord of the mirror position
 *  \param [in] object        The pin object
 */
void o_pin_mirror_world(TOPLEVEL *toplevel,
			int world_centerx, int world_centery, OBJECT *object)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->line != NULL);
  g_return_if_fail (object->type == OBJ_PIN);

  /* translate object to origin */
  o_pin_translate_world(object, -world_centerx, -world_centery);

  object->line->x[0] = -object->line->x[0];

  object->line->x[1] = -object->line->x[1];

  o_pin_translate_world(object, world_centerx, world_centery);
}

/*! \brief modify one point of a pin object
 *  \par Function Description
 *  This function modifies one point of a pin \a object. The point
 *  is specified by the \a whichone variable and the new coordinate
 *  is (\a x, \a y).
 *  
 *  \param toplevel   The TOPLEVEL object
 *  \param object     The pin OBJECT to modify
 *  \param x          new x-coord of the pin point
 *  \param y          new y-coord of the pin point
 *  \param whichone   pin point to modify
 *
 */
void o_pin_modify(TOPLEVEL *toplevel, OBJECT *object,
		  int x, int y, int whichone)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->line != NULL);
  g_return_if_fail (object->type == OBJ_PIN);
  g_return_if_fail (object->whichend >= 0);
  g_return_if_fail (object->whichend < 2);

  object->line->x[whichone] = x;
  object->line->y[whichone] = y;

  object->w_bounds_valid_for = NULL;
}

/*! \brief guess the whichend of pins of object list
 *  \par Function Description
 *  This function determines the whichend of the pins in the \a object_list.
 *  In older libgeda file format versions there was no information about the 
 *  active end of pins.
 *  This function calculates the bounding box of all pins in the object list.
 *  The side of the pins that are closer to the boundary of the box are
 *  set as active ends of the pins.
 *  
 *  \param toplevel    The TOPLEVEL object
 *  \param object_list list of OBJECTs
 *  \param num_pins    pin count in the object list
 *
 */
void o_pin_update_whichend(TOPLEVEL *toplevel,
                           GList *object_list, int num_pins)
{
  OBJECT *o_current;
  GList *iter;
  int top = 0, left = 0;
  int right = 0, bottom = 0;
  int d1, d2, d3, d4;
  int min0, min1;
  int min0_whichend, min1_whichend;
  int rleft, rtop, rright, rbottom;
  int found;

  if (object_list && num_pins) {
    if (num_pins == 1 || toplevel->force_boundingbox) {
      world_get_object_glist_bounds (toplevel, object_list,
                                     &left, &top, &right, &bottom);
    } else {
      found = 0;

      /* only look at the pins to calculate bounds of the symbol */
      iter = object_list;
      while (iter != NULL) {
        o_current = (OBJECT *)iter->data;
        if (o_current->type == OBJ_PIN) {
          (void) world_get_single_object_bounds(
            toplevel, o_current, &rleft, &rtop, &rright, &rbottom);

          if ( found ) {
            left = min( left, rleft );
            top = min( top, rtop );
            right = max( right, rright );
            bottom = max( bottom, rbottom );
          } else {
            left = rleft;
            top = rtop;
            right = rright;
            bottom = rbottom;
            found = 1;
          }
        }
        iter = g_list_next (iter);
      }

    }
  } else {
    return;
  }

  iter = object_list;
  while (iter != NULL) {
    o_current = (OBJECT *)iter->data;
    /* Determine which end of the pin is on or nearest the boundary */
    if (o_current->type == OBJ_PIN && o_current->whichend == -1) {
      if (o_current->line->y[0] == o_current->line->y[1]) {
        /* horizontal */
        
        d1 = abs(o_current->line->x[0] - left);
        d2 = abs(o_current->line->x[1] - left);
        d3 = abs(o_current->line->x[0] - right);
        d4 = abs(o_current->line->x[1] - right);

        if (d1 <= d2) {
          min0 = d1;
          min0_whichend = 0;
        } else {
          min0 = d2;
          min0_whichend = 1;
        }

        if (d3 <= d4) {
          min1 = d3;
          min1_whichend = 0;
        } else {
          min1 = d4;
          min1_whichend = 1;
        }

        if (min0 <= min1) {
          o_current->whichend = min0_whichend;
        } else {
          o_current->whichend = min1_whichend;
        }
           
      } else if (o_current->line->x[0] == o_current->line->x[1]) {
        /* vertical */
        
        d1 = abs(o_current->line->y[0] - top);
        d2 = abs(o_current->line->y[1] - top);
        d3 = abs(o_current->line->y[0] - bottom);
        d4 = abs(o_current->line->y[1] - bottom);

        if (d1 <= d2) {
          min0 = d1;
          min0_whichend = 0;
        } else {
          min0 = d2;
          min0_whichend = 1;
        }

        if (d3 <= d4) {
          min1 = d3;
          min1_whichend = 0;
        } else {
          min1 = d4;
          min1_whichend = 1;
        }

        if (min0 <= min1) {
          o_current->whichend = min0_whichend;
        } else {
          o_current->whichend = min1_whichend;
        }
      }
    }
    iter = g_list_next (iter);
  }
}


/*! \brief Sets the type, and corresponding width of a pin
 *
 *  \par Function Description
 *  Sets the pin's type and width to a particular style.
 *
 *  \param [in] toplevel   The TOPLEVEL object
 *  \param [in] o_current  The pin OBJECT being modified
 *  \param [in] pin_type   The new type of this pin
 */
void o_pin_set_type (TOPLEVEL *toplevel, OBJECT *o_current, int pin_type)
{
  g_return_if_fail (o_current != NULL);
  g_return_if_fail (o_current->type == OBJ_PIN);

  o_emit_pre_change_notify (toplevel, o_current);
  switch (pin_type) {
    default:
      g_critical ("o_pin_set_type: Got invalid pin type %i\n", pin_type);
      /* Fall through */
    case PIN_TYPE_NET:
      o_current->line_width = PIN_WIDTH_NET;
      o_current->pin_type = PIN_TYPE_NET;
      break;
    case PIN_TYPE_BUS:
      o_current->line_width = PIN_WIDTH_BUS;
      o_current->pin_type = PIN_TYPE_BUS;
      break;
  }
  o_emit_change_notify (toplevel, o_current);
}
