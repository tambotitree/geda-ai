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
#include <string.h>

#include "libgeda_priv.h"


typedef void (*DRAW_FUNC) (TOPLEVEL *toplevel, FILE *fp, PATH *path,
                           int line_width, int length, int space,
                           int origin_x, int origin_y);


typedef void (*FILL_FUNC) (TOPLEVEL *toplevel, FILE *fp, PATH *path,
                           int fill_width,
                           int angle1, int pitch1, int angle2, int pitch2,
                           int origin_x, int origin_y);


/*! \brief Create a new path object.
 *  \par Function Description
 *  This function creates a new object representing a path.
 *  This object is added to the end of the list <B>object_list</B>
 *  pointed object belongs to.
 *  The <B>type</B> parameter must be equal to #OBJ_PATH.
 *  The <B>color</B> parameter corresponds to the color the path
 *  will be drawn with.
 *  The path shape is created by parsing \a path_string.
 *
 *  The #OBJECT structure is allocated with the
 *  #s_basic_init_object() function. The structure describing
 *  the path is allocated and initialized with the parameters given
 *  to the function.
 *
 *  Both the path type and the filling type are set to default
 *  values : solid path type with a width of 0, and no filling.
 *  It can be changed after with the #o_set_line_options() and
 *  #o_set_fill_options().
 *
 *  The object is added to the end of the list described by the
 *  <B>object_list</B> parameter by the #s_basic_link_object().
 *
 *  \param [in]     toplevel     The TOPLEVEL object.
 *  \param [in]     type         Must be OBJ_PATH.
 *  \param [in]     color        The path color.
 *  \param [in]     path_string  The string representation of the path
 *  \return A pointer to the new end of the object list.
 */
OBJECT *o_path_new (TOPLEVEL *toplevel,
                    char type, int color, const char *path_string)
{
  return o_path_new_take_path (toplevel, type, color,
                               s_path_parse (path_string));
}


/*! \brief Create a new path object.
 *  \par Function Description
 *  This function creates and returns a new OBJECT representing a path
 *  using the path shape data stored in \a path_data.  The \a
 *  path_data is subsequently owned by the returned OBJECT.
 *
 *  \see o_path_new().
 *
 *  \param [in]     toplevel     The TOPLEVEL object.
 *  \param [in]     type         Must be OBJ_PATH.
 *  \param [in]     color        The path color.
 *  \param [in]     path_data    The #PATH data structure to use.
 *  \return A pointer to the new end of the object list.
 */
OBJECT *o_path_new_take_path (TOPLEVEL *toplevel,
                              char type, int color, PATH *path_data)
{
  OBJECT *new_node;

  /* create the object */
  new_node        = s_basic_new_object (type, "path");
  new_node->color = color;

  new_node->path  = path_data;

  /* path type and filling initialized to default */
  o_set_line_options (toplevel, new_node,
                      DEFAULT_OBJECT_END, TYPE_SOLID, 0, -1, -1);
  o_set_fill_options (toplevel, new_node,
                      FILLING_HOLLOW, -1, -1, -1, -1, -1);

  /* compute bounding box */
  new_node->w_bounds_valid_for = NULL;

  return new_node;
}


/*! \brief Create a copy of a path.
 *  \par Function Description
 *  This function creates a verbatim copy of the
 *  object pointed by <B>o_current</B> describing a path. The new object
 *  is added at the end of the list following the <B>list_tail</B>
 *  parameter.
 *
 *  \param [in]  toplevel  The TOPLEVEL object.
 *  \param [in]  o_current  Line OBJECT to copy.
 *  \return A new pointer to the end of the object list.
 */
OBJECT *o_path_copy (TOPLEVEL *toplevel, OBJECT *o_current)
{
  OBJECT *new_obj;
  char *path_string;

  path_string = s_path_string_from_path (o_current->path);
  new_obj = o_path_new (toplevel, OBJ_PATH, o_current->color, path_string);
  g_free (path_string);

  /* copy the path type and filling options */
  o_set_line_options (toplevel, new_obj, o_current->line_end,
                      o_current->line_type, o_current->line_width,
                      o_current->line_length, o_current->line_space);
  o_set_fill_options (toplevel, new_obj,
                      o_current->fill_type, o_current->fill_width,
                      o_current->fill_pitch1, o_current->fill_angle1,
                      o_current->fill_pitch2, o_current->fill_angle2);

  /* calc the bounding box */
  o_current->w_bounds_valid_for = NULL;

  /* return the new tail of the object list */
  return new_obj;
}


/*! \brief Create path OBJECT from character string.
 *  \par Function Description
 *  This function creates a path OBJECT from the character string
 *  <B>*buf</B> and a number of lines following that describing the
 *  path, read from <B>*tb</B>.
 *
 *  Depending on <B>*version</B>, the correct file format is considered.
 *  Currently two file format revisions are supported :
 *  <DL>
 *    <DT>*</DT><DD>the file format used until 20010704 release.
 *    <DT>*</DT><DD>the file format used for the releases after 20010704.
 *  </DL>
 *
 *  \param [in]  toplevel       The TOPLEVEL object.
 *  \param [in]  first_line      Character string with path description.
 *  \param [in]  tb              Text buffer containing the path string.
 *  \param [in]  release_ver     libgeda release version number.
 *  \param [in]  fileformat_ver  libgeda file format version number.
 *  \return A pointer to the new path object, or NULL on error;
 */
OBJECT *o_path_read (TOPLEVEL *toplevel,
                     const char *first_line, TextBuffer *tb,
                     unsigned int release_ver, unsigned int fileformat_ver, GError **err)
{
  OBJECT *new_obj;
  char type;
  int color;
  int line_width, line_space, line_length;
  int line_end;
  int line_type;
  int fill_type, fill_width, angle1, pitch1, angle2, pitch2;
  int num_lines = 0;
  int i;
  char *string;
  GString *pathstr;

  /*
   * The current path format to describe a line is a space separated
   * list of characters and numbers in plain ASCII on a single path.
   * The meaning of each item is described in the file format documentation.
   */
  /* Allocate enough space */
  if (sscanf (first_line, "%c %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
	      &type, &color, &line_width, &line_end, &line_type,
	      &line_length, &line_space, &fill_type, &fill_width, &angle1,
	      &pitch1, &angle2, &pitch2, &num_lines) != 14) {
    g_set_error(err, EDA_ERROR, EDA_ERROR_PARSE, _("Failed to parse path object"));
    return NULL;
  }

  /*
   * Checks if the required color is valid.
   */
  if (color < 0 || color >= MAX_OBJECT_COLORS) {
    s_log_message (_("Found an invalid color [ %s ]\n"), first_line);
    s_log_message (_("Setting color to default color\n"));
    color = DEFAULT_COLOR;
  }

  /*
   * A path is internally described by its two ends. A new object is
   * allocated, initialized and added to the list of objects. Its path
   * type is set according to the values of the fields on the path.
   */

  pathstr = g_string_new ("");
  for (i = 0; i < num_lines; i++) {
    const gchar *line;

    line = s_textbuffer_next_line (tb);

    if (line == NULL) {
      g_set_error (err, EDA_ERROR, EDA_ERROR_PARSE, _("Unexpected end-of-file when reading path"));
      return NULL;
    }

    pathstr = g_string_append (pathstr, line);
  }

  /* retrieve the character string from the GString */
  string = g_string_free (pathstr, FALSE);
  string = remove_last_nl (string);

  /* create a new path */
  new_obj = o_path_new (toplevel, type, color, string);
  g_free (string);

  /* set its line options */
  o_set_line_options (toplevel, new_obj,
                      line_end, line_type, line_width, line_length, line_space);
  /* set its fill options */
  o_set_fill_options (toplevel, new_obj,
                      fill_type, fill_width, pitch1, angle1, pitch2, angle2);

  return new_obj;
}


/*! \brief Create a character string representation of a path OBJECT.
 *  \par Function Description
 *  The function formats a string in the buffer <B>*buff</B> to describe
 *  the path object <B>*object</B>.
 *
 *  \param [in] object  path OBJECT to create string from.
 *  \return A pointer to the path OBJECT character string.
 *
 *  \note
 *  Caller must g_free returned character string.
 *
 */
char *o_path_save (OBJECT *object)
{
  int line_width, line_space, line_length;
  char *buf;
  int num_lines;
  OBJECT_END line_end;
  OBJECT_TYPE line_type;
  OBJECT_FILLING fill_type;
  int fill_width, angle1, pitch1, angle2, pitch2;
  char *path_string;

  /* description of the line type */
  line_width  = object->line_width;
  line_end    = object->line_end;
  line_type   = object->line_type;
  line_length = object->line_length;
  line_space  = object->line_space;

  /* filling parameters */
  fill_type    = object->fill_type;
  fill_width   = object->fill_width;
  angle1       = object->fill_angle1;
  pitch1       = object->fill_pitch1;
  angle2       = object->fill_angle2;
  pitch2       = object->fill_pitch2;

  path_string = s_path_string_from_path (object->path);
  num_lines = o_text_num_lines (path_string);
  buf = g_strdup_printf ("%c %d %d %d %d %d %d %d %d %d %d %d %d %d\n%s",
                         object->type, object->color, line_width, line_end,
                         line_type, line_length, line_space, fill_type,
                         fill_width, angle1, pitch1, angle2, pitch2,
                         num_lines, path_string);
  g_free (path_string);

  return buf;
}


/*! \brief Modify controol point location
 *
 *  \par Function Description
 *  This function modifies a control point location of the path object
 *  *object. The control point being modified is selected according to
 *  the whichone parameter.
 *
 *  The new position is given by <B>x</B> and <B>y</B>.
 *
 *  \param [in]     toplevel  The TOPLEVEL object.
 *  \param [in,out] object    The path OBJECT
 *  \param [in]     x         New x coordinate for the control point
 *  \param [in]     y         New y coordinate for the control point
 *  \param [in]     whichone  Which control point is being modified
 */
void o_path_modify (TOPLEVEL *toplevel, OBJECT *object,
                    int x, int y, int whichone)
{
  int i;
  int grip_no = 0;
  PATH_SECTION *section;

  o_emit_pre_change_notify (toplevel, object);

  for (i = 0; i <  object->path->num_sections; i++) {
    section = &object->path->sections[i];

    switch (section->code) {
    case PATH_CURVETO:
      /* Two control point grips */
      if (whichone == grip_no++) {
        section->x1 = x;
        section->y1 = y;
      }
      if (whichone == grip_no++) {
        section->x2 = x;
        section->y2 = y;
      }
      /* Fall through */
    case PATH_MOVETO:
    case PATH_MOVETO_OPEN:
    case PATH_LINETO:
      /* Destination point grip */
      if (whichone == grip_no++) {
        section->x3 = x;
        section->y3 = y;
      }
      break;
    case PATH_END:
      break;
    }
  }

  /* Update bounding box */
  object->w_bounds_valid_for = NULL;
  o_emit_change_notify (toplevel, object);
}


/*! \brief Translate a path position in WORLD coordinates by a delta.
 *  \par Function Description
 *  This function applies a translation of (<B>x1</B>,<B>y1</B>) to the path
 *  described by <B>*object</B>. <B>x1</B> and <B>y1</B> are in world unit.
 *
 *  \param [in,out] object     Line OBJECT to translate.
 *  \param [in]     dx         x distance to move.
 *  \param [in]     dy         y distance to move.
 */
void o_path_translate_world (OBJECT *object, int dx, int dy)
{
  PATH_SECTION *section;
  int i;

  g_return_if_fail (object != NULL);
  g_return_if_fail (object->path != NULL);
  g_return_if_fail (object->type == OBJ_PATH);

  for (i = 0; i < object->path->num_sections; i++) {
    section = &object->path->sections[i];

    switch (section->code) {
    case PATH_CURVETO:
      section->x1 += dx;
      section->y1 += dy;
      section->x2 += dx;
      section->y2 += dy;
      /* Fall through */
    case PATH_MOVETO:
    case PATH_MOVETO_OPEN:
    case PATH_LINETO:
      section->x3 += dx;
      section->y3 += dy;
      break;
    case PATH_END:
      break;
    }
  }

  /* Update bounding box */
  object->w_bounds_valid_for = NULL;
}


/*! \brief Rotate Line OBJECT using WORLD coordinates.
 *  \par Function Description
 *  This function rotates the path described by
 *  <B>*object</B> around the (<B>world_centerx</B>,<B>world_centery</B>)
 *  point by <B>angle</B> degrees.
 *  The center of rotation is in world units.
 *
 *  \param [in]      toplevel      The TOPLEVEL object.
 *  \param [in]      world_centerx  Rotation center x coordinate in WORLD units.
 *  \param [in]      world_centery  Rotation center y coordinate in WORLD units.
 *  \param [in]      angle          Rotation angle in degrees (See note below).
 *  \param [in,out]  object         Line OBJECT to rotate.
 */
void o_path_rotate_world (TOPLEVEL *toplevel,
                          int world_centerx, int world_centery, int angle,
                          OBJECT *object)
{
  PATH_SECTION *section;
  int i;

  g_return_if_fail (object != NULL);
  g_return_if_fail (object->path != NULL);
  g_return_if_fail (object->type == OBJ_PATH);

  for (i = 0; i < object->path->num_sections; i++) {
    section = &object->path->sections[i];

    switch (section->code) {
    case PATH_CURVETO:
      /* Two control point grips */
      section->x1 -= world_centerx; section->y1 -= world_centery;
      section->x2 -= world_centerx; section->y2 -= world_centery;
      rotate_point_90 (section->x1, section->y1, angle, &section->x1, &section->y1);
      rotate_point_90 (section->x2, section->y2, angle, &section->x2, &section->y2);
      section->x1 += world_centerx; section->y1 += world_centery;
      section->x2 += world_centerx; section->y2 += world_centery;
      /* Fall through */
    case PATH_MOVETO:
    case PATH_MOVETO_OPEN:
    case PATH_LINETO:
      /* Destination point grip */
      section->x3 -= world_centerx; section->y3 -= world_centery;
      rotate_point_90 (section->x3, section->y3, angle, &section->x3, &section->y3);
      section->x3 += world_centerx; section->y3 += world_centery;
      break;
    case PATH_END:
      break;
    }
  }
  object->w_bounds_valid_for = NULL;
}


/*! \brief Mirror a path using WORLD coordinates.
 *  \par Function Description
 *  This function mirrors the path from the point
 *  (<B>world_centerx</B>,<B>world_centery</B>) in world unit.
 *
 *  \param [in]     toplevel      The TOPLEVEL object.
 *  \param [in]     world_centerx  Origin x coordinate in WORLD units.
 *  \param [in]     world_centery  Origin y coordinate in WORLD units.
 *  \param [in,out] object         Line OBJECT to mirror.
 */
void o_path_mirror_world (TOPLEVEL *toplevel, int world_centerx,
                          int world_centery, OBJECT *object)
{
  PATH_SECTION *section;
  int i;

  g_return_if_fail (object != NULL);
  g_return_if_fail (object->path != NULL);
  g_return_if_fail (object->type == OBJ_PATH);

  for (i = 0; i < object->path->num_sections; i++) {
    section = &object->path->sections[i];

    switch (section->code) {
    case PATH_CURVETO:
      /* Two control point grips */
      section->x1 = 2 * world_centerx - section->x1;
      section->x2 = 2 * world_centerx - section->x2;
      /* Fall through */
    case PATH_MOVETO:
    case PATH_MOVETO_OPEN:
    case PATH_LINETO:
      /* Destination point grip */
      section->x3 = 2 * world_centerx - section->x3;
      break;
    case PATH_END:
      break;
    }
  }

  object->w_bounds_valid_for = NULL;
}


/*! \brief Get path bounding rectangle in WORLD coordinates.
 *  \par Function Description
 *  This function sets the <B>left</B>, <B>top</B>, <B>right</B> and
 *  <B>bottom</B> parameters to the boundings of the path object described
 *  in <B>*path</B> in world units.
 *
 *  \note Bounding box for bezier curves is loose because we just consider
 *        the convex hull of the curve control and end-points.
 *
 *  \param [in]  toplevel  The TOPLEVEL object.
 *  \param [in]  object     Line OBJECT to read coordinates from.
 *  \param [out] left       Left path coordinate in WORLD units.
 *  \param [out] top        Top path coordinate in WORLD units.
 *  \param [out] right      Right path coordinate in WORLD units.
 *  \param [out] bottom     Bottom path coordinate in WORLD units.
 */
void world_get_path_bounds (TOPLEVEL *toplevel, OBJECT *object,
                            int *left, int *top, int *right, int *bottom)
{
  PATH_SECTION *section;
  int halfwidth;
  int i;
  int found_bound = FALSE;

  /* Find the bounds of the path region */
  for (i = 0; i < object->path->num_sections; i++) {
    section = &object->path->sections[i];
    switch (section->code) {
      case PATH_CURVETO:
        /* Bezier curves with this construction of control points will lie
         * within the convex hull of the control and curve end points */
        *left   = (found_bound) ? MIN (*left,   section->x1) : section->x1;
        *top    = (found_bound) ? MIN (*top,    section->y1) : section->y1;
        *right  = (found_bound) ? MAX (*right,  section->x1) : section->x1;
        *bottom = (found_bound) ? MAX (*bottom, section->y1) : section->y1;
        found_bound = TRUE;
        *left   = MIN (*left,   section->x2);
        *top    = MIN (*top,    section->y2);
        *right  = MAX (*right,  section->x2);
        *bottom = MAX (*bottom, section->y2);
        /* Fall through */
      case PATH_MOVETO:
      case PATH_MOVETO_OPEN:
      case PATH_LINETO:
        *left   = (found_bound) ? MIN (*left,   section->x3) : section->x3;
        *top    = (found_bound) ? MIN (*top,    section->y3) : section->y3;
        *right  = (found_bound) ? MAX (*right,  section->x3) : section->x3;
        *bottom = (found_bound) ? MAX (*bottom, section->y3) : section->y3;
        found_bound = TRUE;
        break;
      case PATH_END:
        break;
    }
  }

  if (found_bound) {
    /* This isn't strictly correct, but a 1st order approximation */
    halfwidth = object->line_width / 2;
    *left   -= halfwidth;
    *top    -= halfwidth;
    *right  += halfwidth;
    *bottom += halfwidth;
  }
}

/*! \brief get the position of the first path point
 *  \par Function Description
 *  This function gets the position of the first point of an path object.
 *
 *  \param [in] object   The object to get the position.
 *  \param [out] x       pointer to the x-position
 *  \param [out] y       pointer to the y-position
 *  \return TRUE if successfully determined the position, FALSE otherwise
 */
gboolean o_path_get_position (OBJECT *object, gint *x, gint *y)
{
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (object->type == OBJ_PATH, FALSE);
  g_return_val_if_fail (object->path != NULL, FALSE);

  if (object->path->num_sections == 0) {
    return FALSE;
  }

  if (x != NULL) {
    *x = object->path->sections[0].x3;
  }

  if (y != NULL) {
    *y = object->path->sections[0].y3;
  }

  return TRUE;
}

/*! \brief Calculates the distance between the given point and the closest
 *  point on the given path segment.
 *
 *  \param [in] toplevel     The TOPLEVEL object.
 *  \param [in] object       The path OBJECT.
 *  \param [in] x            The x coordinate of the given point.
 *  \param [in] y            The y coordinate of the given point.
 *  \param [in] force_solid  If true, force treating the object as solid.
 *  \return The shortest distance from the object to the point.  With an
 *  invalid parameter, this function returns G_MAXDOUBLE.
 */
double o_path_shortest_distance (TOPLEVEL *toplevel, OBJECT *object,
                                 int x, int y, int force_solid)
{
  int solid;

  solid = force_solid || object->fill_type != FILLING_HOLLOW;

  return s_path_shortest_distance (object->path, x, y, solid);
}

