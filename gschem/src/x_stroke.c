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

#include "gschem.h"

#include <stroke.h>

/*
 * <B>stroke_points</B> is an array of points for the stroke
 * footprints. The points of the stroke are displayed over the display
 * area of the main window. They have to be erased when the stroke is
 * translated and the sequence evaluated.
 *
 * Its size will never exceed <B>STROKE_MAX_POINTS</B> (the limit in
 * number of points of a stroke provided by libstroke).
 */
typedef struct {
  gint x, y;
} StrokePoint;

static GArray *stroke_points = NULL;


/*! \brief Initializes the stroke interface.
 *  \par Function Description
 *  This is the initialization function for the stroke interface. It
 *  initializes the libstroke library and prepare an array of points
 *  for the mouse footprints.
 *
 *  This function has to be called only once at application
 *  initialization before any use of the stroke interface.
 */
void
x_stroke_init (void)
{
  g_return_if_fail (stroke_points == NULL);

  stroke_init ();

  stroke_points = g_array_new (FALSE,
                               FALSE,
                               sizeof (StrokePoint));
}

/*! \brief Frees memory of the stroke interface.
 *  \par Function Description
 *  This function frees the memory used for the mouse footprint
 *  points. It terminates the use of the stroke interface.
 */
void
x_stroke_free (void)
{
  g_return_if_fail (stroke_points != NULL);

  g_array_free (stroke_points, TRUE);
  stroke_points = NULL;
}

/*! \brief Records a new point for the stroke.
 *  \par Function Description
 *  This function adds the point (<B>x</B>,<B>y</B>) as a new point in
 *  the stroke.
 *
 * The footprint is updated and the new point is drawn on the drawing area.
 *
 *  \param [in] w_current The GschemToplevel object.
 *  \param [in] x        The X coord of the new point.
 *  \param [in] Y        The X coord of the new point.
 */
void
x_stroke_record (GschemToplevel *w_current, gint x, gint y)
{
  cairo_matrix_t user_to_device_matrix;
  double x0, y0, x1, y1;
  GschemPageView *view = gschem_toplevel_get_current_page_view (w_current);
  g_return_if_fail (view != NULL);
  GschemPageGeometry *geometry = gschem_page_view_get_page_geometry (view);
  g_return_if_fail (geometry != NULL);

  g_assert (stroke_points != NULL);

  stroke_record (x, y);

  if (stroke_points->len < STROKE_MAX_POINTS) {
    StrokePoint point = { x, y };

    g_array_append_val (stroke_points, point);

    if (stroke_points->len == 1)
      return;

    StrokePoint *last_point = &g_array_index (stroke_points, StrokePoint,
                                              stroke_points->len - 2);

    cairo_t *cr = gdk_cairo_create (gtk_widget_get_window (GTK_WIDGET(view)));
    COLOR *color = x_color_lookup (STROKE_COLOR);
    cairo_set_source_rgba (cr,
                           color->r / 255.0,
                           color->g / 255.0,
                           color->b / 255.0,
                           color->a / 255.0);

    cairo_set_matrix (cr, gschem_page_geometry_get_world_to_screen_matrix (geometry));
    x0 = last_point->x;
    y0 = last_point->y;
    x1 = x;
    y1 = y;
    cairo_device_to_user (cr, &x0, &y0);
    cairo_device_to_user (cr, &x1, &y1);
    cairo_get_matrix (cr, &user_to_device_matrix);
    cairo_save (cr);
    cairo_identity_matrix (cr);

    cairo_matrix_transform_point (&user_to_device_matrix, &x0, &y0);
    cairo_matrix_transform_point (&user_to_device_matrix, &x1, &y1);

    cairo_move_to (cr, x0, y0);
    cairo_line_to (cr, x1, y1);
    cairo_stroke (cr);
    cairo_restore (cr);
    cairo_destroy (cr);
  }

}

/*! \brief Evaluates the stroke.
 *  \par Function Description
 *  This function transforms the stroke input so far in an action.
 *
 *  It makes use of the guile procedure <B>eval-stroke</B> to evaluate
 *  the stroke sequence into a possible action. The mouse footprint is
 *  erased in this function.
 *
 *  It returns 1 if the stroke has been successfully evaluated as an
 *  action. It returns 0 if libstroke failed to transform the stroke
 *  or there is no action attached to the stroke.
 *
 *  \param [in] w_current The GschemToplevel object.
 *  \returns 1 on success, 0 otherwise.
 */
gint
x_stroke_translate_and_execute (GschemToplevel *w_current)
{
  gchar sequence[STROKE_MAX_SEQUENCE];
  StrokePoint *point;
  int min_x, min_y, max_x, max_y;
  gint i;

  g_assert (stroke_points != NULL);

  if (stroke_points->len == 0)
    return 0;

  point = &g_array_index (stroke_points, StrokePoint, 0);
  min_x = max_x = point->x;
  min_y = max_y = point->y;

  for (i = 1; i < stroke_points->len; i++) {
    point = &g_array_index (stroke_points, StrokePoint, i);
    min_x = min (min_x, point->x);
    min_y = min (min_y, point->y);
    max_x = max (max_x, point->x);
    max_y = max (max_y, point->y);
  }

  o_invalidate_rect (w_current, min_x, min_y, max_x + 1, max_y + 1);

  /* resets length of array */
  stroke_points->len = 0;

  /* try evaluating stroke */
  if (stroke_trans ((char*)&sequence)) {
    gchar *guile_string =
      g_strdup_printf("(eval-stroke \"%s\")", sequence);
    SCM ret;

    scm_dynwind_begin (0);
    scm_dynwind_unwind_handler (g_free, guile_string, SCM_F_WIND_EXPLICITLY);
    ret = g_scm_c_eval_string_protected (guile_string);
    scm_dynwind_end ();

    return (SCM_NFALSEP (ret));
  }

  return 0;
}
