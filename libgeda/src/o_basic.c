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

/*! \file o_basic.c
 *  \brief functions for the basic object type
 *
 *  This file contains the code used to handle OBJECTs (st_object).
 *  The object is the basic type of all elements stored in schematic
 *  and symbol files.
 *
 *  The object be extended to become concrete objects like a line,
 *  a pin, text, a circle or a picture. These extentions are substructures
 *  in the object struct.
 *  The subobjects are picture (st_picture), path (st_path), arcs (st_arc),
 *  a line (st_line), box (st_box), circle (st_circle), text (st_text) and
 *  a complex type (st_complex).
 *
 *  Pins, nets and busses are just a kind of a line.
 *
 *  The complex object can carry many primary objects. If the complex
 *  object is a symbol, then the complex symbol contains all the pins,
 *  the text and the graphics.
 *
 *  \image html o_object_relations.png
 *  \image latex o_object_relations.pdf "object relations" width=14cm
 */

#include <config.h>

#include <stdio.h>

/* instrumentation code */
#if 0
#include <sys/time.h>
#include <unistd.h>
#endif

#include "libgeda_priv.h"


/*! \brief Check if point is inside a region
 *  \par Function Description
 *  This function takes a rectangular region and a point.  It will check
 *  if the point is located in the region or not.
 *
 *  \param [in] xmin    Smaller x coordinate of the region.
 *  \param [in] ymin    Smaller y coordinate of the region.
 *  \param [in] xmax    Larger x coordinate of the region.
 *  \param [in] ymax    Larger y coordinate of the region.
 *  \param [in] x       x coordinate of the point to check.
 *  \param [in] y       y coordinate of the point to check.
 *  \return 1 if the point is inside the region, 0 otherwise.
 */
int inside_region(int xmin, int ymin, int xmax, int ymax, int x, int y)
{
  return ((x >= xmin && x <= xmax && y >= ymin && y <= ymax) ? 1 : 0);
}


/*! \brief Set an #OBJECT's line options.
 *  \par Function Description
 *  This function allows a line's end, type, width, length and space to be set.
 *  See #OBJECT_END and #OBJECT_TYPE for information on valid
 *  object end and type values.
 *
 *  \param [in]     toplevel  The TOPLEVEL object.
 *  \param [in,out] o_current  OBJECT to set line options on.
 *  \param [in]     end        An OBJECT_END.
 *  \param [in]     type       An OBJECT_TYPE.
 *  \param [in]     width      Line width.
 *  \param [in]     length     Line length.
 *  \param [in]     space      Spacing between dashes/dots. Cannot be negative.
 *
 *  \todo Make space an unsigned int and check for a max value instead.
 *        If a max value is not required, then it would simplify the code.
 */
void o_set_line_options(TOPLEVEL *toplevel, OBJECT *o_current,
			OBJECT_END end, OBJECT_TYPE type,
			int width, int length, int space)
{
  g_return_if_fail (o_current != NULL);

  /* do some error checking / correcting */
  switch(type) {
    default:
      s_log_message (_("Found invalid line type [ %d %d %d ], "
                       "resetting to default\n"),
                     type, length, space);
      type = TYPE_SOLID;
      /* fallthrough */

    case(TYPE_SOLID):
      length = -1;
      space = -1;
      break;
    case(TYPE_DOTTED):
      length = -1;
      if (space < 1) {
        space = 100;
      }
    break;
    case(TYPE_DASHED):
    case(TYPE_CENTER):
    case(TYPE_PHANTOM):
      if (length < 1) {
        length = 100;
      }
      if (space < 1) {
        space = 100;
      }
    break;
  }

  o_emit_pre_change_notify (toplevel, o_current);

  o_current->line_width = width;
  o_current->line_end   = end;
  o_current->line_type  = type;

  o_current->line_length = length;
  o_current->line_space  = space;

  /* Recalculate the object's bounding box */
  o_current->w_bounds_valid_for = NULL;
  o_emit_change_notify (toplevel, o_current);

}

/*! \brief get #OBJECT's line properties.
 *  \par Function Description
 *  This function get's the #OBJECT's line options.
 *  See #OBJECT_END and #OBJECT_TYPE for information on valid
 *  object end and type values.
 *
 *  \param [in]   object    OBJECT to read the properties
 *  \param [out]  end       An OBJECT_END.
 *  \param [out]  type      An OBJECT_TYPE.
 *  \param [out]  width     Line width.
 *  \param [out]  length    Line length.
 *  \param [out]  space     Spacing between dashes/dots.
 *  \return TRUE on succes, FALSE otherwise
 *
 */
gboolean o_get_line_options(OBJECT *object,
                            OBJECT_END *end, OBJECT_TYPE *type,
                            int *width, int *length, int *space)
{
  if (object->type != OBJ_LINE
      && object->type != OBJ_ARC
      && object->type != OBJ_BOX
      && object->type != OBJ_CIRCLE
      && object->type != OBJ_PATH)
    return FALSE;

  *end = object->line_end;
  *type = object->line_type;
  *width = object->line_width;
  *length = object->line_length;
  *space = object->line_space;

  return TRUE;
}

/*! \brief Set #OBJECT's fill options.
 *  \par Function Description
 *  This function allows an #OBJECT's fill options to be configured.
 *  See #OBJECT_FILLING for information on valid fill types.
 *
 *  \param [in]      toplevel  The TOPLEVEL object.
 *  \param [in,out]  o_current  OBJECT to be updated.
 *  \param [in]      type       OBJECT_FILLING type.
 *  \param [in]      width      fill width.
 *  \param [in]      pitch1     cross hatch line distance
 *  \param [in]      angle1     cross hatch angle
 *  \param [in]      pitch2     cross hatch line distance
 *  \param [in]      angle2     cross hatch angle
 *
 */
void o_set_fill_options(TOPLEVEL *toplevel, OBJECT *o_current,
			OBJECT_FILLING type, int width,
			int pitch1, int angle1,
			int pitch2, int angle2)
{
  if(o_current == NULL) {
    return;
  }

  /* do some error checking / correcting */
  switch(type) {
    case(FILLING_MESH):
      if (width < 0) {
        width = 1;
      }
      if (angle1 < 0) {
        angle1 = 45;
      }
      if (pitch1 < 0) {
        pitch1 = 100;
      }
      if (angle2 < 0) {
        angle2 = 135;
      }
      if (pitch2 < 0) {
        pitch2 = 100;
      }
      break;

    case(FILLING_HATCH):
      if (width < 0) {
        width = 1;
      }
      if (angle1 < 0) {
        angle1 = 45;
      }
      if (pitch1 < 0) {
        pitch1 = 100;
      }
      angle2 = -1;
      pitch2 = -1;
      break;

    default:
      s_log_message (_("Found invalid fill type [ %d %d %d %d %d %d ], "
                       "resetting to default\n"),
                     type, width, angle1, pitch1, angle2, pitch2);
      type = FILLING_HOLLOW;
      /* fallthrough */

    case(FILLING_HOLLOW):
    case(FILLING_FILL):
      width = -1;
      angle1 = -1;
      pitch1 = -1;
      angle2 = -1;
      pitch2 = -1;
      break;
  }

  o_emit_pre_change_notify (toplevel, o_current);

  o_current->fill_type = type;
  o_current->fill_width = width;

  o_current->fill_pitch1 = pitch1;
  o_current->fill_angle1 = angle1;

  o_current->fill_pitch2 = pitch2;
  o_current->fill_angle2 = angle2;

  o_emit_change_notify (toplevel, o_current);
}

/*! \brief get #OBJECT's fill properties.
 *  \par Function Description
 *  This function get's the #OBJECT's fill options.
 *  See #OBJECT_FILLING for information on valid fill types.
 *
 *  \param [in]   object    OBJECT to read the properties
 *  \param [out]  type      OBJECT_FILLING type
 *  \param [out]  width     fill width.
 *  \param [out]  pitch1    cross hatch line distance
 *  \param [out]  angle1    cross hatch angle
 *  \param [out]  pitch2    cross hatch line distance
 *  \param [out]  angle2    cross hatch angle
 *  \return TRUE on succes, FALSE otherwise
 *
 */
gboolean o_get_fill_options(OBJECT *object,
                            OBJECT_FILLING *type, int *width,
                            int *pitch1, int *angle1,
                            int *pitch2, int *angle2)
{
  if (object->type != OBJ_BOX
      && object->type != OBJ_CIRCLE
      && object->type != OBJ_PATH)
    return FALSE;

  *type = object->fill_type;
  *width = object->fill_width;
  *pitch1 = object->fill_pitch1;
  *angle1 = object->fill_angle1;
  *pitch2 = object->fill_pitch2;
  *angle2 = object->fill_angle2;

  return TRUE;
}

/*! \brief get the base position of an object
 *  \par Function Description
 *  This function gets the position of an object in world coordinates.
 *
 *  \param [in] object   The object to get the position.
 *  \param [out] x       pointer to the x-position
 *  \param [out] y       pointer to the y-position
 *  \return TRUE if successfully determined the position, FALSE otherwise
 */
gboolean o_get_position (OBJECT *object, gint *x, gint *y)
{
  gboolean (*func) (OBJECT*, int*, int*) = NULL;

  g_return_val_if_fail (object != NULL, FALSE);

  switch (object->type) {
      case OBJ_LINE:    func = o_line_get_position;    break;
      case OBJ_NET:     func = o_net_get_position;     break;
      case OBJ_BUS:     func = o_bus_get_position;     break;
      case OBJ_BOX:     func = o_box_get_position;     break;
      case OBJ_PICTURE: func = o_picture_get_position; break;
      case OBJ_CIRCLE:  func = o_circle_get_position;  break;
      case OBJ_PLACEHOLDER:
      case OBJ_COMPLEX: func = o_complex_get_position; break;
      case OBJ_TEXT:    func = o_text_get_position;    break;
      case OBJ_PATH:    func = o_path_get_position;    break;
      case OBJ_PIN:     func = o_pin_get_position;     break;
      case OBJ_ARC:     func = o_arc_get_position;     break;
      default:
        g_critical ("o_get_position: object %p has bad type '%c'\n",
                    object, object->type);
  }

  if (func != NULL) {
    return (*func) (object, x, y);
  }
  return FALSE;
}


/*! \brief Translates an object in world coordinates
 *  \par Function Description
 *  This function translates the object <B>object</B> by
 *  <B>dx</B> and <B>dy</B>.
 *
 *  \param [in] object   The object to translate.
 *  \param [in] dx       Amount to horizontally translate object
 *  \param [in] dy       Amount to vertically translate object
 */
void o_translate_world (OBJECT *object, gint dx, gint dy)
{
  void (*func) (OBJECT*, int, int) = NULL;

  switch (object->type) {
      case OBJ_LINE:    func = o_line_translate_world;    break;
      case OBJ_NET:     func = o_net_translate_world;     break;
      case OBJ_BUS:     func = o_bus_translate_world;     break;
      case OBJ_BOX:     func = o_box_translate_world;     break;
      case OBJ_PICTURE: func = o_picture_translate_world; break;
      case OBJ_CIRCLE:  func = o_circle_translate_world;  break;
      case OBJ_PLACEHOLDER:
      case OBJ_COMPLEX: func = o_complex_translate_world; break;
      case OBJ_TEXT:    func = o_text_translate_world;    break;
      case OBJ_PATH:    func = o_path_translate_world;    break;
      case OBJ_PIN:     func = o_pin_translate_world;     break;
      case OBJ_ARC:     func = o_arc_translate_world;     break;
      default:
        g_critical ("o_translate_world: object %p has bad type '%c'\n",
                    object, object->type);
  }

  if (func != NULL) {
    (*func) (object, dx, dy);
  }
}


/*! \brief Rotates an object in world coordinates
 *  \par Function Description
 *  This function rotates the object <B>object</B> about the coordinates
 *  <B>world_centerx</B> and <B>world_centery</B>, by <B>angle</B>degrees.
 *
 *  \param [in] toplevel The toplevel environment.
 *  \param [in] world_centerx  X coordinate of rotation center (world coords)
 *  \param [in] world_centery  Y coordinate of rotation center (world coords)
 *  \param [in] angle          Angle of rotation (degrees)
 *  \param [in] object         The object to rotate.
 */
void o_rotate_world (TOPLEVEL *toplevel, int world_centerx, int world_centery, int angle, OBJECT *object)
{
  void (*func) (TOPLEVEL*, int, int, int, OBJECT*) = NULL;

  switch (object->type) {
      case OBJ_LINE:    func = o_line_rotate_world;       break;
      case OBJ_NET:     func = o_net_rotate_world;        break;
      case OBJ_BUS:     func = o_bus_rotate_world;        break;
      case OBJ_BOX:     func = o_box_rotate_world;        break;
      case OBJ_PICTURE: func = o_picture_rotate_world;    break;
      case OBJ_CIRCLE:  func = o_circle_rotate_world;     break;
      case OBJ_PLACEHOLDER:
      case OBJ_COMPLEX: func = o_complex_rotate_world;    break;
      case OBJ_TEXT:    func = o_text_rotate_world;       break;
      case OBJ_PATH:    func = o_path_rotate_world;       break;
      case OBJ_PIN:     func = o_pin_rotate_world;        break;
      case OBJ_ARC:     func = o_arc_rotate_world;        break;
      default:
        g_critical ("o_rotate_world: object %p has bad type '%c'\n",
                    object, object->type);
  }

  if (func != NULL) {
    (*func) (toplevel, world_centerx, world_centery, angle, object);
  }
}


/*! \brief Mirrors an object in world coordinates
 *  \par Function Description
 *  This function mirrors an object about the point
 *  (<B>world_centerx</B>,<B>world_centery</B>) in world units.
 *
 *  \param [in]     toplevel       The TOPLEVEL object.
 *  \param [in]     world_centerx  Origin x coordinate in WORLD units.
 *  \param [in]     world_centery  Origin y coordinate in WORLD units.
 *  \param [in,out] object         The OBJECT to mirror.
 */
void o_mirror_world (TOPLEVEL *toplevel, int world_centerx, int world_centery, OBJECT *object)
{
  void (*func) (TOPLEVEL*, int, int, OBJECT*) = NULL;

  switch (object->type) {
      case OBJ_LINE:    func = o_line_mirror_world;       break;
      case OBJ_NET:     func = o_net_mirror_world;        break;
      case OBJ_BUS:     func = o_bus_mirror_world;        break;
      case OBJ_BOX:     func = o_box_mirror_world;        break;
      case OBJ_PICTURE: func = o_picture_mirror_world;    break;
      case OBJ_CIRCLE:  func = o_circle_mirror_world;     break;
      case OBJ_PLACEHOLDER:
      case OBJ_COMPLEX: func = o_complex_mirror_world;    break;
      case OBJ_TEXT:    func = o_text_mirror_world;       break;
      case OBJ_PATH:    func = o_path_mirror_world;       break;
      case OBJ_PIN:     func = o_pin_mirror_world;        break;
      case OBJ_ARC:     func = o_arc_mirror_world;        break;
      default:
        g_critical ("o_mirror_world: object %p has bad type '%c'\n",
                    object, object->type);
  }

  if (func != NULL) {
    (*func) (toplevel, world_centerx, world_centery, object);
  }
}


/*! \brief Calculates the distance between the given point and the closest
 * point on the given object.
 *
 *  \param [in] toplevel     The TOPLEVEL object.
 *  \param [in] object       The given object.
 *  \param [in] x            The x coordinate of the given point.
 *  \param [in] y            The y coordinate of the given point.
 *  \return The shortest distance from the object to the point. If the
 *  distance cannot be calculated, this function returns a really large
 *  number (G_MAXDOUBLE).  If an error occurs, this function returns
 *  G_MAXDOUBLE.
 */
double o_shortest_distance (TOPLEVEL *toplevel, OBJECT *object, int x, int y)
{
  return o_shortest_distance_full (toplevel, object, x, y, FALSE);
}

/*! \brief Calculates the distance between the given point and the closest
 * point on the given object. Allows forcing objects to solid.
 *
 *  \param [in] toplevel     The TOPLEVEL object.
 *  \param [in] object       The given object.
 *  \param [in] x            The x coordinate of the given point.
 *  \param [in] y            The y coordinate of the given point.
 *  \param [in] force_solid  If true, force treating the object as solid.
 *  \return The shortest distance from the object to the point. If the
 *  distance cannot be calculated, this function returns a really large
 *  number (G_MAXDOUBLE).  If an error occurs, this function returns
 *  G_MAXDOUBLE.
 */
double o_shortest_distance_full (TOPLEVEL *toplevel, OBJECT *object,
                                 int x, int y, int force_solid)
{
  double shortest_distance = G_MAXDOUBLE;
  double (*func) (TOPLEVEL *, OBJECT *, int, int, int) = NULL;

  g_return_val_if_fail (object != NULL, G_MAXDOUBLE);

  switch(object->type) {
    case OBJ_BUS:
    case OBJ_NET:
    case OBJ_PIN:
    case OBJ_LINE:        func = o_line_shortest_distance;     break;
    case OBJ_BOX:         func = o_box_shortest_distance;      break;
    case OBJ_PICTURE:     func = o_picture_shortest_distance;  break;
    case OBJ_CIRCLE:      func = o_circle_shortest_distance;   break;
    case OBJ_PLACEHOLDER:
    case OBJ_COMPLEX:     func = o_complex_shortest_distance;  break;
    case OBJ_TEXT:        func = o_text_shortest_distance;     break;
    case OBJ_PATH:        func = o_path_shortest_distance;     break;
    case OBJ_ARC:         func = o_arc_shortest_distance;      break;
    default:
      g_critical ("o_shortest_distance: object %p has bad type '%c'\n",
                  object, object->type);
  }

  if (func != NULL) {
    shortest_distance = (*func) (toplevel, object, x, y, force_solid);
  }

  return shortest_distance;
}

/*! \brief Mark an OBJECT's cached bounds as invalid
 *  \par Function Description
 *  Recursively marks the cached bounds of the given OBJECT and its
 *  parents as having been invalidated and in need of an update. They
 *  will be recalculated next time the OBJECT's bounds are requested
 *  (e.g. via world_get_single_object_bounds() ).
 *  \param [in] toplevel
 *  \param [in] object
 *
 *  \todo Turn this into a macro?
 */
void o_bounds_invalidate (TOPLEVEL *toplevel, OBJECT *object)
{
  OBJECT *iter = object;

  while (iter != NULL) {
    iter->w_bounds_valid_for = NULL;
    iter = iter->parent;
  }
}


/*! \brief Change the color of an object
 *
 *  \par Function Description
 *  This function changes the color of an object.
 *
 *  \param [in] toplevel  The TOPLEVEL structure.
 *  \param [in] object    The OBJECT to change color.
 *  \param [in] color     The new color.
 */
void o_set_color (TOPLEVEL *toplevel, OBJECT *object, int color)
{
  g_return_if_fail (object != NULL);

  object->color = color;

  if (object->type == OBJ_COMPLEX ||
      object->type == OBJ_PLACEHOLDER)
    o_glist_set_color (toplevel, object->complex->prim_objs, color);
}


/*! \brief Get an object's parent PAGE.
 *
 * \par Function Description
 * Returns the PAGE structure which owns \a object. If \a object is
 * not currently associated with a PAGE, returns NULL. If \a object is
 * part of a compound object, recurses upward.
 *
 * \param [in] toplevel  The TOPLEVEL structure.
 * \param [in] object    The OBJECT for which to retrieve the parent PAGE.
 * \return The PAGE which owns \a object or NULL.
 *
 * \sa s_page_append_object() s_page_append() s_page_remove()
 */
PAGE *
o_get_page (TOPLEVEL *toplevel, OBJECT *object)
{
  if (object->parent != NULL) {
    return o_get_page (toplevel, object->parent);
  }
  return object->page;
}

/*! \brief Get an object's containing complex object.
 *
 * \par Function Description
 * If \a object is part of a complex #OBJECT, returns that
 * #OBJECT. Otherwise, returns NULL.
 *
 * \param [in] toplevel  The TOPLEVEL structure.
 * \param [in] object    The OBJECT for which to get the containing OBJECT.
 * \return The complex OBJECT which owns \a object, or NULL.
 */
OBJECT *
o_get_parent (TOPLEVEL *toplevel, OBJECT *object)
{
  g_return_val_if_fail ((object != NULL), NULL);

  if (object->parent != NULL) {
    return object->parent;
  }
  return NULL;
}

/* Structure for each entry in a TOPLEVEL's list of registered change
 * notification handlers */
struct change_notify_entry {
  ChangeNotifyFunc pre_change_func;
  ChangeNotifyFunc change_func;
  void *user_data;
};

/*! \brief Add change notification handlers to a TOPLEVEL.
 * \par Function Description
 * Adds a set of change notification handlers to a #TOPLEVEL instance.
 * \a pre_change_func will be called just before an object is
 * modified, and \a change_func will be called just after an object is
 * modified, with the affected object and the given \a user_data.
 *
 * \param toplevel #TOPLEVEL structure to add handlers to.
 * \param pre_change_func Function to be called just before changes.
 * \param change_func Function to be called just after changes.
 * \param user_data User data to be passed to callback functions.
 */
void
o_add_change_notify (TOPLEVEL *toplevel,
                     ChangeNotifyFunc pre_change_func,
                     ChangeNotifyFunc change_func,
                     void *user_data)
{
  struct change_notify_entry *entry = g_new0 (struct change_notify_entry, 1);
  entry->pre_change_func = pre_change_func;
  entry->change_func = change_func;
  entry->user_data = user_data;
  toplevel->change_notify_funcs =
    g_list_prepend (toplevel->change_notify_funcs, entry);
}

/*! \brief Remove change notification handlers from a TOPLEVEL.
 * \par Function Description
 * Removes a set of change notification handlers and their associated
 * \a user_data from \a toplevel.  If no registered set of handlers
 * matches the given \a pre_change_func, \a change_func and \a
 * user_data, does nothing.
 *
 * \see o_add_change_notify()
 *
 * \param toplevel #TOPLEVEL structure to remove handlers from.
 * \param pre_change_func Function called just before changes.
 * \param change_func Function called just after changes.
 * \param user_data User data passed to callback functions.
 */
void
o_remove_change_notify (TOPLEVEL *toplevel,
                        ChangeNotifyFunc pre_change_func,
                        ChangeNotifyFunc change_func,
                        void *user_data)
{
  GList *iter;
  for (iter = toplevel->change_notify_funcs;
       iter != NULL; iter = g_list_next (iter)) {

    struct change_notify_entry *entry =
      (struct change_notify_entry *) iter->data;

    if ((entry != NULL)
        && (entry->pre_change_func == pre_change_func)
        && (entry->change_func == change_func)
        && (entry->user_data == user_data)) {
      g_free (entry);
      iter->data = NULL;
    }
  }
  toplevel->change_notify_funcs =
    g_list_remove_all (toplevel->change_notify_funcs, NULL);
}

/*! \brief Emit an object pre-change notification.
 * \par Function Description
 * Calls each pre-change callback function registered with #TOPLEVEL
 * to notify listeners that \a object is about to be modified.  All
 * libgeda functions that modify #OBJECT structures should call this
 * just before making a change to an #OBJECT.
 *
 * \param toplevel #TOPLEVEL structure to emit notifications from.
 * \param object   #OBJECT structure to emit notifications for.
 */
void
o_emit_pre_change_notify (TOPLEVEL *toplevel, OBJECT *object)
{
  GList *iter;
  for (iter = toplevel->change_notify_funcs;
       iter != NULL; iter = g_list_next (iter)) {

    struct change_notify_entry *entry =
      (struct change_notify_entry *) iter->data;

    if ((entry != NULL) && (entry->pre_change_func != NULL)) {
      entry->pre_change_func (entry->user_data, object);
    }
  }
}

/*! \brief Emit an object change notification.
 * \par Function Description
 * Calls each change callback function registered with #TOPLEVEL to
 * notify listeners that \a object has just been modified.  All
 * libgeda functions that modify #OBJECT structures should call this
 * just after making a change to an #OBJECT.
 *
 * \param toplevel #TOPLEVEL structure to emit notifications from.
 * \param object   #OBJECT structure to emit notifications for.
 */
void
o_emit_change_notify (TOPLEVEL *toplevel, OBJECT *object)
{
  GList *iter;
  for (iter = toplevel->change_notify_funcs;
       iter != NULL; iter = g_list_next (iter)) {

    struct change_notify_entry *entry =
      (struct change_notify_entry *) iter->data;

    if ((entry != NULL) && (entry->change_func != NULL)) {
      entry->change_func (entry->user_data, object);
    }
  }
}

/*! \brief Query visibility of the object.
 *  \par Function Description
 *  Attribute getter for the visible field within the object.
 *
 *  \param object   The OBJECT structure to be queried
 *  \return TRUE when VISIBLE, FALSE otherwise
 */
gboolean
o_is_visible (OBJECT *object)
{
  g_return_val_if_fail (object != NULL, FALSE);
  return object->visibility == VISIBLE;
}

/*! \brief Set visibility of the object.
 *  \par Function Description
 *  Set value of visibility field within the object.
 *  If resulting visibility value is changed,
 *  invalidate the bounds of the object and parent objects.
 *
 *  \param toplevel The #TOPLEVEL structure
 *  \param object   The #OBJECT structure to be modified
 */
void
o_set_visibility (TOPLEVEL *toplevel, OBJECT *object, int visibility)
{
  g_return_if_fail (object != NULL);
  if (object->visibility != visibility) {
    object->visibility = visibility;
    o_bounds_invalidate (toplevel, object);
  }
}
