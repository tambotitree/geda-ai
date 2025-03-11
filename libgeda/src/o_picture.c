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

/*! \file o_picture.c
 *  \brief functions for the picture object
 */

#include <config.h>

#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <math.h>

#include <gio/gio.h>

#include "libgeda_priv.h"

/*! \brief Create picture OBJECT from character string.
 *  \par Function Description
 *  Parses \a first_line and subsequent lines from \a tb, and returns
 *  a newly-created picture #OBJECT.
 *
 *  \param [in]  toplevel       The TOPLEVEL object.
 *  \param [in]  first_line      Character string with picture description.
 *  \param [in]  tb              Text buffer to load embedded data from.
 *  \param [in]  release_ver     libgeda release version number.
 *  \param [in]  fileformat_ver  libgeda file format version number.
 *  \return A pointer to the new picture object, or NULL on error.
 */
OBJECT *o_picture_read (TOPLEVEL *toplevel,
                        const char *first_line,
                        TextBuffer *tb,
                        unsigned int release_ver,
                        unsigned int fileformat_ver,
                        GError **err)
{
  OBJECT *new_obj;
  int x1, y1;
  int width, height, angle;
  int mirrored, embedded;
  int num_conv;
  gchar type;
  const gchar *line = NULL;
  gchar *filename;
  gchar *file_content = NULL;
  guint file_length = 0;

  num_conv = sscanf(first_line, "%c %d %d %d %d %d %d %d\n",
                    &type, &x1, &y1, &width, &height, &angle, &mirrored, &embedded);

  if (num_conv != 8) {
    g_set_error(err, EDA_ERROR, EDA_ERROR_PARSE, _("Failed to parse picture definition"));
    return NULL;
  }

  if (width == 0 || height == 0) {
    s_log_message(_("Found a zero width/height picture [ %c %d %d %d %d ]\n"),
                  type, x1, y1, width, height);
  }

  if ( (mirrored > 1) || (mirrored < 0)) {
    s_log_message(_("Found a picture with a wrong 'mirrored' parameter: %d.\n"),
                  mirrored);
    s_log_message(_("Setting mirrored to 0\n"));
    mirrored = 0;
  }

  if ( (embedded > 1) || (embedded < 0)) {
    s_log_message(_("Found a picture with a wrong 'embedded' parameter: %d.\n"),
                  embedded);
    s_log_message(_("Setting embedded to 0\n"));
    embedded = 0;
  }

  switch(angle) {

    case(0):
    case(90):
    case(180):
    case(270):
    break;

    default:
      s_log_message(_("Found an unsupported picture angle [ %d ]\n"), angle);
      s_log_message(_("Setting angle to 0\n"));
      angle=0;
      break;

  }

  filename = g_strdup(s_textbuffer_next_line(tb));
  filename = remove_last_nl(filename);

  /* Handle empty filenames */
  if (strlen (filename) == 0) {
    s_log_message (_("Found an image with no filename."));
    g_free (filename);
    filename = NULL;
  }

  if (embedded == 1) {
    GString *encoded_picture=g_string_new("");
    char finished = 0;

    /* Read the encoded picture */
    do {

      line = s_textbuffer_next_line(tb);
      if (line == NULL) break;

      if (strcmp (line, ".\n") != 0) {
        encoded_picture = g_string_append (encoded_picture, line);
      } else {
        finished = 1;
      }
    } while (finished == 0);

    /* Decode the picture */
    if (encoded_picture != NULL) {
      file_content = s_encoding_base64_decode(encoded_picture->str,
                                              encoded_picture->len,
                                              &file_length);
      g_string_free (encoded_picture, TRUE);
    }

    if (file_content == NULL) {
      s_log_message (_("Failed to load image from embedded data [%s]: %s\n"),
                     filename, _("Base64 decoding failed."));
      s_log_message (_("Falling back to file loading. Picture unembedded.\n"));
      embedded = 0;
    }
  }

  /* create the picture */
  /* The picture is described by its upper left and lower right corner */
  new_obj = o_picture_new (toplevel, file_content, file_length, filename,
                           type,
                           x1, y1+height, x1+width, y1,
                           angle, mirrored, embedded);

  g_free (file_content);
  g_free (filename);

  return new_obj;
}

/*! \brief Create a character string representation of a picture OBJECT.
 *  \par Function Description
 *  This function formats a string in the buffer <B>*buff</B> to describe
 *  the picture object <B>*object</B>.
 *
 *  \param [in] object  Picture OBJECT to create string from.
 *  \return A pointer to the picture OBJECT character string.
 *
 *  \note
 *  Caller must g_free returned character string.
 *
 */
char *o_picture_save(OBJECT *object)
{
  int width, height, x1, y1;
  gchar *encoded_picture=NULL;
  gchar *out=NULL;
  guint encoded_picture_length;
  const gchar *filename = NULL;

  /* calculate the width and height of the box */
  width  = abs(object->picture->lower_x - object->picture->upper_x);
  height = abs(object->picture->upper_y - object->picture->lower_y);

  /* calculate the lower left corner of the box */
  x1 = object->picture->upper_x;
  y1 = object->picture->upper_y - height; /* move the origin to 0, 0*/

#if DEBUG
  printf("picture: %d %d %d %d\n", x1, y1, width, height);
#endif

  /* Encode the picture if it's embedded */
  if (o_picture_is_embedded (object)) {
    encoded_picture =
      s_encoding_base64_encode( (char *)object->picture->file_content,
                                object->picture->file_length,
                                &encoded_picture_length,
                                TRUE);
    if (encoded_picture == NULL) {
      s_log_message(_("ERROR: o_picture_save: unable to encode the picture.\n"));
    }
  }

  /* Cope with null filename */
  filename = o_picture_get_filename (object);
  if (filename == NULL) filename = "";

  if (o_picture_is_embedded (object) &&
      encoded_picture != NULL) {
    out = g_strdup_printf("%c %d %d %d %d %d %c %c\n%s\n%s\n%s",
                          object->type,
                          x1, y1, width, height,
                          object->picture->angle,
                          /* Convert the (0,1) chars to ASCII */
                          (object->picture->mirrored)+0x30,
                          '1',
                          filename,
                          encoded_picture,
                          ".");
  }
  else {
    out = g_strdup_printf("%c %d %d %d %d %d %c %c\n%s",
                          object->type,
                          x1, y1, width, height,
                          object->picture->angle,
                          /* Convert the (0,1) chars to ASCII */
                          (object->picture->mirrored)+0x30,
                          '0',
                          filename);
  }
  g_free(encoded_picture);

  return(out);
}


/*! \brief Create a picture object.
 *  \par Function Description
 *  This function creates a new object representing a picture.
 *
 *  The picture is described by its upper left corner (\a x1, \a y1)
 *  and its lower right corner (\a x2, \a y2).  The \a type parameter
 *  must be equal to #OBJ_PICTURE.
 *
 *  If \a file_content is non-NULL, it must be a pointer to a buffer
 *  containing raw image data.  If loading data from \a file_content
 *  is unsuccessful, and \a filename is non-NULL, an image will
 *  attempt to be loaded from \a filename.  Otherwise, the picture
 *  object will be initially empty.
 *
 *  \param [in]     toplevel      The TOPLEVEL object.
 *  \param [in]     file_content  Raw data of the image file, or NULL.
 *  \param [in]     file_length   Length of raw data buffer
 *  \param [in]     filename      File name backing this picture, or NULL.
 *  \param [in]     type          Must be OBJ_PICTURE.
 *  \param [in]     x1            Upper x coordinate.
 *  \param [in]     y1            Upper y coordinate.
 *  \param [in]     x2            Lower x coordinate.
 *  \param [in]     y2            Lower y coordinate.
 *  \param [in]     angle         Picture rotation angle.
 *  \param [in]     mirrored      Whether the image should be mirrored or not.
 *  \param [in]     embedded      Whether the embedded flag should be set or not.
 *  \return A pointer to a new picture #OBJECT.
 */
OBJECT *o_picture_new (TOPLEVEL *toplevel,
                       const gchar *file_content, gsize file_length,
                       const gchar *filename,
                       char type, int x1, int y1, int x2, int y2, int angle,
                       int mirrored, int embedded)
{
  OBJECT *new_node;
  PICTURE *picture;

  /* create the object */
  new_node = s_basic_new_object(type, "picture");

  picture = (PICTURE *) g_malloc0 (sizeof(PICTURE));
  new_node->picture = picture;

  /* describe the picture with its upper left and lower right corner */
  picture->upper_x = (x1 > x2) ? x2 : x1;
  picture->upper_y = (y1 > y2) ? y1 : y2;
  picture->lower_x = (x1 > x2) ? x1 : x2;
  picture->lower_y = (y1 > y2) ? y2 : y1;

  picture->pixbuf = NULL;
  picture->file_content = NULL;
  picture->file_length = 0;

  picture->ratio = fabs ((double) (x1 - x2) / (y1 - y2));
  picture->filename = g_strdup (filename);
  picture->angle = angle;
  picture->mirrored = mirrored;
  picture->embedded = embedded;

  if (file_content != NULL) {
    GError *error = NULL;
    if (!o_picture_set_from_buffer (toplevel, new_node, filename,
                                    file_content, file_length, &error)) {
      s_log_message (_("Failed to load buffer image [%s]: %s\n"),
                     filename, error->message);
      g_error_free (error);

      /* Force the data into the object anyway, so as to prevent data
       * loss of embedded images. */
      picture->file_content = g_memdup (file_content, file_length);
      picture->file_length = file_length;
    }
  }
  if (picture->pixbuf == NULL && filename != NULL) {
    GError *error = NULL;
    if (!o_picture_set_from_file (toplevel, new_node, filename, &error)) {
      s_log_message (_("Failed to load image from [%s]: %s\n"),
                     filename, error->message);
      g_error_free (error);
      /* picture not found; try to open a fall back pixbuf */
      picture->pixbuf = o_picture_get_fallback_pixbuf(toplevel);
    }
  }

  /* compute the bounding picture */
  new_node->w_bounds_valid_for = NULL;

  return new_node;
}

/*! \brief Get picture bounding rectangle in WORLD coordinates.
 *  \par Function Description
 *  This function sets the <B>left</B>, <B>top</B>, <B>right</B> and
 *  <B>bottom</B> parameters to the boundings of the picture object
 *  described in <B>*picture</B> in WORLD units.
 *
 *  \param [in]  toplevel  The TOPLEVEL object.
 *  \param [in]  object     Picture OBJECT to read coordinates from.
 *  \param [out] left       Left picture coordinate in WORLD units.
 *  \param [out] top        Top picture coordinate in WORLD units.
 *  \param [out] right      Right picture coordinate in WORLD units.
 *  \param [out] bottom     Bottom picture coordinate in WORLD units.
 */
void world_get_picture_bounds(TOPLEVEL *toplevel, OBJECT *object,
                              int *left, int *top, int *right, int *bottom)
{
  *left   = min(object->picture->upper_x, object->picture->lower_x);
  *top    = min(object->picture->upper_y, object->picture->lower_y);
  *right  = max(object->picture->upper_x, object->picture->lower_x);
  *bottom = max(object->picture->upper_y, object->picture->lower_y);

}

/*! \brief get the position of the left bottom point
 *  \par Function Description
 *  This function gets the position of the bottom left point of a picture object.
 *
 *  \param [in] object   The object to get the position.
 *  \param [out] x       pointer to the x-position
 *  \param [out] y       pointer to the y-position
 *  \return TRUE if successfully determined the position, FALSE otherwise
 */
gboolean o_picture_get_position (OBJECT *object, gint *x, gint *y)
{
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (object->type == OBJ_PICTURE, FALSE);
  g_return_val_if_fail (object->picture != NULL, FALSE);

  if (x != NULL) {
    *x = min (object->picture->lower_x, object->picture->upper_x);
  }

  if (y != NULL) {
    *y = min (object->picture->lower_y, object->picture->upper_y);
  }

  return TRUE;
}


/*! \brief Get the width/height ratio of an image.
 * \par Function Description

 * Returns the width/height ratio of picture \a object, taking the
 * image rotation into account.
 *
 * \param toplevel  The current #TOPLEVEL.
 * \param object    Picture #OBJECT to inspect.
 * \return width/height ratio for \a object.
 */
double
o_picture_get_ratio (TOPLEVEL *toplevel, OBJECT *object)
{
  g_return_val_if_fail (object != NULL, 1);
  g_return_val_if_fail (object->picture != NULL, 1);

  /* The effective ratio varies depending on the rotation of the
   * image. */
  switch (object->picture->angle) {
  case 0:
  case 180:
    return object->picture->ratio;
  case 90:
  case 270:
    return 1.0 / object->picture->ratio;
  default:
    g_critical (_("Picture %p has invalid angle %i\n"), object,
                object->picture->angle);
  }
  return 0;
}

/*! \brief Modify the description of a picture OBJECT.
 *  \par Function Description
 *  This function modifies the coordinates of one of the four corner of
 *  the picture. The new coordinates of the corner identified by
 *  <B>whichone</B> are given by <B>x</B> and <B>y</B> in world unit.
 *
 *  The coordinates of the corner is modified in the world coordinate system.
 *  Screen coordinates and boundings are then updated.
 *
 *  \param [in]     toplevel  The TOPLEVEL object.
 *  \param [in,out] object     Picture OBJECT to modify.
 *  \param [in]     x          New x coordinate.
 *  \param [in]     y          New y coordinate.
 *  \param [in]     whichone   Which picture parameter to modify.
 *
 *  <B>whichone</B> can have the following values:
 *  <DL>
 *    <DT>*</DT><DD>PICTURE_UPPER_LEFT
 *    <DT>*</DT><DD>PICTURE_LOWER_LEFT
 *    <DT>*</DT><DD>PICTURE_UPPER_RIGHT
 *    <DT>*</DT><DD>PICTURE_LOWER_RIGHT
 *  </DL>
 */
void o_picture_modify(TOPLEVEL *toplevel, OBJECT *object,
                      int x, int y, int whichone)
{
  int tmp;
  double ratio = o_picture_get_ratio (toplevel, object);

  o_emit_pre_change_notify (toplevel, object);

  /* change the position of the selected corner */
  switch(whichone) {
    case PICTURE_UPPER_LEFT:
      object->picture->upper_x = x;
      tmp = abs(object->picture->upper_x - object->picture->lower_x) / ratio;
      if (y < object->picture->lower_y) {
        tmp = -tmp;
      }
      object->picture->upper_y = object->picture->lower_y + tmp;
      break;

    case PICTURE_LOWER_LEFT:
      object->picture->upper_x = x;
      tmp = abs(object->picture->upper_x - object->picture->lower_x) / ratio;
      if (y > object->picture->upper_y) {
        tmp = -tmp;
      }
      object->picture->lower_y = object->picture->upper_y - tmp;
      break;

    case PICTURE_UPPER_RIGHT:
      object->picture->lower_x = x;
      tmp = abs(object->picture->upper_x - object->picture->lower_x) / ratio;
      if (y < object->picture->lower_y) {
        tmp = -tmp;
      }
      object->picture->upper_y = object->picture->lower_y + tmp;
      break;

    case PICTURE_LOWER_RIGHT:
      object->picture->lower_x = x;
      tmp = abs(object->picture->upper_x - object->picture->lower_x) / ratio;
      if (y > object->picture->upper_y) {
        tmp = -tmp;
      }
      object->picture->lower_y = object->picture->upper_y - tmp;
      break;

    default:
      return;
  }

  /* need to update the upper left and lower right corners */
  if(object->picture->upper_x > object->picture->lower_x) {
    tmp                      = object->picture->upper_x;
    object->picture->upper_x = object->picture->lower_x;
    object->picture->lower_x = tmp;
  }

  if(object->picture->upper_y < object->picture->lower_y) {
    tmp                      = object->picture->upper_y;
    object->picture->upper_y = object->picture->lower_y;
    object->picture->lower_y = tmp;
  }

  /* recalculate the screen coords and the boundings */
  object->w_bounds_valid_for = NULL;
  o_emit_change_notify (toplevel, object);
}

/*! \brief Modify a picture object's coordinates.
 * \par Function Description
 * Modifies the coordinates of all four corners of a picture \a
 * object.  The picture is adjusted to fit the rectangle enclosed by
 * the points (\a x1, \a y1) and (\a x2, \a y2), and scaled as large
 * as possible to still fit within that rectangle.
 *
 * \param [in]     toplevel current #TOPLEVEL.
 * \param [in,out] object   picture #OBJECT to be modified.
 * \param [in]     x1       x coordinate of first corner of box.
 * \param [in]     y1       y coordinate of first corner of box.
 * \param [in]     x2       x coordinate of second corner of box.
 * \param [in]     y2       y coordinate of second corner of box.
 */
void
o_picture_modify_all (TOPLEVEL *toplevel, OBJECT *object,
                      int x1, int y1, int x2, int y2)
{
  o_emit_pre_change_notify (toplevel, object);

  /* Normalise the requested rectangle. */
  object->picture->lower_x = (x1 > x2) ? x1 : x2;
  object->picture->lower_y = (y1 > y2) ? y2 : y1;
  object->picture->upper_x = (x1 > x2) ? x2 : x1;
  object->picture->upper_y = (y1 > y2) ? y1 : y2;

  /* recalculate the world coords and bounds */
  object->w_bounds_valid_for = NULL;
  o_emit_change_notify (toplevel, object);
}

/*! \brief Rotate picture OBJECT using WORLD coordinates.
 *  \par Function Description
 *  This function rotates the picture described by <B>*object</B> around
 *  the (<B>world_centerx</B>, <B>world_centery</B>) point by <B>angle</B>
 *  degrees.
 *  The center of rotation is in world units.
 *
 *  \param [in]      toplevel      The TOPLEVEL object.
 *  \param [in]      world_centerx  Rotation center x coordinate in
 *                                  WORLD units.
 *  \param [in]      world_centery  Rotation center y coordinate in
 *                                  WORLD units.
 *  \param [in]      angle          Rotation angle in degrees (See note below).
 *  \param [in,out]  object         Picture OBJECT to rotate.
 */
void o_picture_rotate_world(TOPLEVEL *toplevel,
                            int world_centerx, int world_centery, int angle,
                            OBJECT *object)
{
  int newx1, newy1;
  int newx2, newy2;

  g_return_if_fail (object != NULL);
  g_return_if_fail (object->picture != NULL);
  g_return_if_fail (object->type == OBJ_PICTURE);

  /* Only 90 degree multiple and positive angles are allowed. */
  /* angle must be positive */
  if(angle < 0) angle = -angle;
  /* angle must be a 90 multiple or no rotation performed */
  if((angle % 90) != 0) return;

  object->picture->angle = ( object->picture->angle + angle ) % 360;

  /* The center of rotation (<B>world_centerx</B>, <B>world_centery</B>) is
   * translated to the origin. The rotation of the upper left and lower
   * right corner are then performed. Finally, the rotated picture is
   * translated back to its previous location.
   */
  /* translate object to origin */
  object->picture->upper_x -= world_centerx;
  object->picture->upper_y -= world_centery;
  object->picture->lower_x -= world_centerx;
  object->picture->lower_y -= world_centery;

  /* rotate the upper left corner of the picture */
  rotate_point_90(object->picture->upper_x, object->picture->upper_y, angle,
                  &newx1, &newy1);

  /* rotate the lower left corner of the picture */
  rotate_point_90(object->picture->lower_x, object->picture->lower_y, angle,
                  &newx2, &newy2);

  /* reorder the corners after rotation */
  object->picture->upper_x = min(newx1,newx2);
  object->picture->upper_y = max(newy1,newy2);
  object->picture->lower_x = max(newx1,newx2);
  object->picture->lower_y = min(newy1,newy2);

  /* translate object back to normal position */
  object->picture->upper_x += world_centerx;
  object->picture->upper_y += world_centery;
  object->picture->lower_x += world_centerx;
  object->picture->lower_y += world_centery;

  /* recalc boundings and screen coords */
  object->w_bounds_valid_for = NULL;

}

/*! \brief Mirror a picture using WORLD coordinates.
 *  \par Function Description
 *  This function mirrors the picture from the point
 *  (<B>world_centerx</B>,<B>world_centery</B>) in world unit.
 *
 *  The picture is first translated to the origin, then mirrored and
 *  finally translated back at its previous position.
 *
 *  \param [in]     toplevel      The TOPLEVEL object.
 *  \param [in]     world_centerx  Origin x coordinate in WORLD units.
 *  \param [in]     world_centery  Origin y coordinate in WORLD units.
 *  \param [in,out] object         Picture OBJECT to mirror.
 */
void o_picture_mirror_world(TOPLEVEL *toplevel,
                            int world_centerx, int world_centery,
                            OBJECT *object)
{
  int newx1, newy1;
  int newx2, newy2;

  g_return_if_fail (object != NULL);
  g_return_if_fail (object->picture != NULL);
  g_return_if_fail (object->type == OBJ_PICTURE);

  /* Set info in object. Sometimes it's necessary to change the
   * rotation angle as well as the mirror flag. */
  object->picture->mirrored = !object->picture->mirrored;
  switch (object->picture->angle) {
  case 90:
    object->picture->angle = 270;
    break;
  case 270:
    object->picture->angle = 90;
    break;
  }

  /* translate object to origin */
  object->picture->upper_x -= world_centerx;
  object->picture->upper_y -= world_centery;
  object->picture->lower_x -= world_centerx;
  object->picture->lower_y -= world_centery;

  /* mirror the corners */
  newx1 = -object->picture->upper_x;
  newy1 = object->picture->upper_y;
  newx2 = -object->picture->lower_x;
  newy2 = object->picture->lower_y;

  /* reorder the corners */
  object->picture->upper_x = min(newx1,newx2);
  object->picture->upper_y = max(newy1,newy2);
  object->picture->lower_x = max(newx1,newx2);
  object->picture->lower_y = min(newy1,newy2);

  /* translate back in position */
  object->picture->upper_x += world_centerx;
  object->picture->upper_y += world_centery;
  object->picture->lower_x += world_centerx;
  object->picture->lower_y += world_centery;

  /* recalc boundings and screen coords */
  object->w_bounds_valid_for = NULL;

}

/*! \brief Translate a picture position in WORLD coordinates by a delta.
 *  \par Function Description
 *  This function applies a translation of (<B>x1</B>,<B>y1</B>) to the picture
 *  described by <B>*object</B>. <B>x1</B> and <B>y1</B> are in world units.
 *
 *  \param [in,out] object     Picture OBJECT to translate.
 *  \param [in]     dx         x distance to move.
 *  \param [in]     dy         y distance to move.
 */
void o_picture_translate_world(OBJECT *object, int dx, int dy)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->picture != NULL);
  g_return_if_fail (object->type == OBJ_PICTURE);

  /* Do world coords */
  object->picture->upper_x = object->picture->upper_x + dx;
  object->picture->upper_y = object->picture->upper_y + dy;
  object->picture->lower_x = object->picture->lower_x + dx;
  object->picture->lower_y = object->picture->lower_y + dy;

  /* recalc the screen coords and the bounding picture */
  object->w_bounds_valid_for = NULL;
}

/*! \brief Create a copy of a picture.
 *  \par Function Description
 *  This function creates a verbatim copy of the object pointed by
 *  <B>o_current</B> describing a picture.
 *
 *  \param [in]  toplevel   The TOPLEVEL object.
 *  \param [in]  object     Picture OBJECT to copy.
 *  \return The new OBJECT
 */
OBJECT *o_picture_copy(TOPLEVEL *toplevel, OBJECT *object)
{
  OBJECT *new_node;
  PICTURE *picture;

  /* create the object */
  new_node = s_basic_new_object(object->type, "picture");

  picture = g_malloc(sizeof(PICTURE));
  new_node->picture = picture;

  new_node->color = object->color;
  new_node->selectable = object->selectable;

  /* describe the picture with its upper left and lower right corner */
  picture->upper_x = object->picture->upper_x;
  picture->upper_y = object->picture->upper_y;
  picture->lower_x = object->picture->lower_x;
  picture->lower_y = object->picture->lower_y;

  if (object->picture->file_content != NULL) {
    picture->file_content = g_memdup (object->picture->file_content,
                                      object->picture->file_length);
  } else {
    picture->file_content = NULL;
  }

  picture->file_length = object->picture->file_length;
  picture->filename    = g_strdup (object->picture->filename);
  picture->ratio       = object->picture->ratio;
  picture->angle       = object->picture->angle;
  picture->mirrored    = object->picture->mirrored;
  picture->embedded    = object->picture->embedded;

  /* Get the picture data */
  picture->pixbuf = o_picture_get_pixbuf (toplevel, object);

  /* compute the bounding picture */
  new_node->w_bounds_valid_for = NULL;

  return new_node;
}

/*! \brief Embed the image file associated with a picture
 * \par Function Description
 * Verify that a picture has valid data associated with it, and if so,
 * mark it to be embedded.
 *
 *  \param [in]     toplevel     The TOPLEVEL object.
 *  \param [in]     object       The picture OBJECT to embed
 */
void o_picture_embed (TOPLEVEL *toplevel, OBJECT *object)
{
  const gchar *filename = o_picture_get_filename (object);
  gchar *basename;

  if (o_picture_is_embedded (object)) return;

  if (object->picture->file_content == NULL) {
    s_log_message (_("Picture [%s] has no image data.\n"), filename);
    s_log_message (_("Falling back to file loading. Picture is still unembedded.\n"));
    object->picture->embedded = 0;
    return;
  }

  object->picture->embedded = 1;

  basename = g_path_get_basename (filename);
  s_log_message (_("Picture [%s] has been embedded\n"), basename);
  g_free (basename);
}


/*! \brief Unembed a picture, reloading the image from disk
 * \par Function Description
 * Verify that the file associated with \a object exists on disk and
 * is usable, and if so, reload the picture and mark it as unembedded.
 *
 *  \param [in]     toplevel     The TOPLEVEL object.
 *  \param [in]     object       The picture OBJECT to unembed
 */
void o_picture_unembed (TOPLEVEL *toplevel, OBJECT *object)
{
  GError *err = NULL;
  const gchar *filename = o_picture_get_filename (object);
  gchar *basename;

  if (!o_picture_is_embedded (object)) return;

  o_picture_set_from_file (toplevel, object, filename, &err);

  if (err != NULL) {
    s_log_message (_("Failed to load image from file [%s]: %s\n"),
                   filename, err->message);
    s_log_message (_("Picture is still embedded.\n"));
    g_error_free (err);
    return;
  }

  object->picture->embedded = 0;

  basename = g_path_get_basename(filename);
  s_log_message (_("Picture [%s] has been unembedded\n"), basename);
  g_free(basename);
}

/*! \brief Calculates the distance between the given point and the closest
 * point in the picture.
 *
 *  Interrior points within the picture return a distance of zero.
 *
 *  \param [in] toplevel     The TOPLEVEL object.
 *  \param [in] object       The picture OBJECT.
 *  \param [in] x            The x coordinate of the given point.
 *  \param [in] y            The y coordinate of the given point.
 *  \param [in] force_solid  If true, force treating the object as solid.
 *  \return The shortest distance from the object to the point. With an
 *  invalid parameter, this function returns G_MAXDOUBLE.
 */
double o_picture_shortest_distance (TOPLEVEL *toplevel, OBJECT *object,
                                    int x, int y, int force_solid)
{
  double dx, dy;
  double x1, y1, x2, y2;

  g_return_val_if_fail (object->picture != NULL, G_MAXDOUBLE);

  x1 = (double)min (object->picture->upper_x, object->picture->lower_x);
  y1 = (double)min (object->picture->upper_y, object->picture->lower_y);
  x2 = (double)max (object->picture->upper_x, object->picture->lower_x);
  y2 = (double)max (object->picture->upper_y, object->picture->lower_y);

  dx = min (((double)x) - x1, x2 - ((double)x));
  dy = min (((double)y) - y1, y2 - ((double)y));

  dx = min (dx, 0);
  dy = min (dy, 0);

  return hypot (dx, dy);
}

/*! \brief Test whether a picture object is embedded.
 * \par Function Description
 * Returns TRUE if the picture \a object will have its data embedded
 * in a schematic or symbol file; returns FALSE if its data will be
 * obtained from a separate file.
 *
 * \param object    The picture #OBJECT to inspect.
 * \return TRUE if \a object is embedded.
 */
gboolean
o_picture_is_embedded (OBJECT *object)
{
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (object->picture != NULL, FALSE);

  return object->picture->embedded;
}

/*! \brief Get a pixel buffer for a picture object.
 * \par Function Description
 * Returns a #GdkPixbuf for the picture object \a object, or NULL if
 * the picture could not be loaded.
 *
 * The returned value should have its reference count decremented with
 * g_object_unref() when no longer needed.
 *
 * \param toplevel  The current #TOPLEVEL.
 * \param object    The picture #OBJECT to inspect.
 * \return A #GdkPixbuf for the picture.
 */
GdkPixbuf *
o_picture_get_pixbuf (TOPLEVEL *toplevel, OBJECT *object)
{
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (object->picture != NULL, NULL);

  if (object->picture->pixbuf != NULL) {
    return g_object_ref (object->picture->pixbuf);
  } else {
    return NULL;
  }
}

/*! \brief Get the raw image data from a picture object.
 * \par Function Description
 * Returns the raw image file data underlying the picture \a object,
 * or NULL if the picture could not be loaded.
 *
 * \param toplevel  The current #TOPLEVEL.
 * \param object    The picture #OBJECT to inspect.
 * \param len       Location to store buffer length.
 * \return A read-only buffer of raw image data.
 */
const char *
o_picture_get_data (TOPLEVEL *toplevel, OBJECT *object,
                    size_t *len)
{
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (object->picture != NULL, NULL);

  *len = object->picture->file_length;
  return object->picture->file_content;
}

/*! \brief Set a picture object's contents from a buffer.
 * \par Function Description
 * Sets the contents of the picture \a object by reading image data
 * from a buffer.  The buffer should be in on-disk format.
 *
 * \param toplevel The current #TOPLEVEL.
 * \param object   The picture #OBJECT to modify.
 * \param filename The new filename for the picture.
 * \param data     The new image data buffer.
 * \param len      The size of the data buffer.
 * \param error    Location to return error information.
 * \return TRUE on success, FALSE on failure.
 */
gboolean
o_picture_set_from_buffer (TOPLEVEL *toplevel, OBJECT *object,
                           const gchar *filename,
                           const gchar *data, size_t len,
                           GError **error)
{
  GdkPixbuf *pixbuf;
  GInputStream *stream;
  gchar *tmp;

  g_return_val_if_fail (toplevel != NULL, FALSE);
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (object->picture != NULL, FALSE);
  g_return_val_if_fail (data != NULL, FALSE);

  /* Check that we can actually load the data before making any
   * changes to the object. */
  stream = G_INPUT_STREAM (g_memory_input_stream_new_from_data (data, len, NULL));
  pixbuf = gdk_pixbuf_new_from_stream (stream, NULL, error);
  g_object_unref (stream);
  if (pixbuf == NULL) return FALSE;

  o_emit_pre_change_notify (toplevel, object);

  if (object->picture->pixbuf != NULL) {
    g_object_unref (object->picture->pixbuf);
  }
  object->picture->pixbuf = pixbuf;

  object->picture->ratio = ((double) gdk_pixbuf_get_width(pixbuf) /
                            gdk_pixbuf_get_height(pixbuf));

  tmp = g_strdup (filename);
  g_free (object->picture->filename);
  object->picture->filename = tmp;

  gchar *buf = g_realloc (object->picture->file_content,
                          len);
  /* It's possible that these buffers might overlap, because the
   * library user hates us. */
  memmove (buf, data, len);
  object->picture->file_content = buf;
  object->picture->file_length = len;

  o_emit_change_notify (toplevel, object);
  return TRUE;
}

/*! \brief Set a picture object's contents from a file.
 * \par Function Description
 * Sets the contents of the picture \a object by reading image data
 * from a file.
 *
 * \param toplevel The current #TOPLEVEL.
 * \param object   The picture #OBJECT to modify.
 * \param filename The filename to load image data from.
 * \param error    Location to return error information.
 * \return TRUE on success, FALSE on failure.
 */
gboolean
o_picture_set_from_file (TOPLEVEL *toplevel, OBJECT *object,
                         const gchar *filename,
                         GError **error)
{
  gchar *buf;
  size_t len;
  gboolean status;

  g_return_val_if_fail (filename != NULL, FALSE);

  if (!g_file_get_contents (filename, &buf, &len, error)) {
    return FALSE;
  }

  status = o_picture_set_from_buffer (toplevel, object, filename,
                                      buf, len, error);
  g_free (buf);
  return status;
}

/*! \brief Get a picture's corresponding filename.
 * \par Function Description
 * Returns the filename associated with the picture \a object.
 *
 * \param object   The picture #OBJECT to inspect.
 * \return the filename associated with \a object.
 */
const gchar *
o_picture_get_filename (OBJECT *object)
{
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (object->picture != NULL, NULL);

  return object->picture->filename;
}

/*! \brief Get fallback pixbuf for displaying pictures.
 * \par Function Description
 * Returns a pixbuf containing the fallback image to be used if a
 * picture object fails to load.  The returned pixbuf should be freed
 * with g_object_unref() when no longer needed.
 *
 * \return a #GdkPixbuf containing a warning image.
 */
GdkPixbuf *
o_picture_get_fallback_pixbuf (TOPLEVEL *toplevel)
{
  static GdkPixbuf *pixbuf = NULL;
  static gboolean failed = FALSE;

  if (pixbuf == NULL && !failed) {
    gchar *filename;
    GError *error = NULL;

    filename = g_build_filename (toplevel->bitmap_directory,
                                 "gschem-warning.png", NULL);
    pixbuf = gdk_pixbuf_new_from_file (filename, &error);

    if (pixbuf == NULL) {
      g_warning ( _("Failed to load fallback image %s: %s.\n"),
                  filename, error->message);
      g_error_free (error);
      failed = TRUE;
    }
    g_free (filename);
  }

  if (failed) return NULL;

  g_assert (GDK_IS_PIXBUF (pixbuf));
  return g_object_ref (pixbuf);
}
