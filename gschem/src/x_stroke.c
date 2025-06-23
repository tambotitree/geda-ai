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


/*! \brief Initialize the stroke recognition system.
 *
 * This must be called once during application startup. It initializes:
 * - The libstroke gesture recognition engine (`stroke_init()`), and
 * - An internal array (`stroke_points`) for recording mouse strokes.
 *
 * \note This function must only be called once. Subsequent calls are ignored or rejected.
 * \warning Not thread-safe.
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

/*! \brief Finalize the stroke recognition system.
 *
 * This function frees the internal `stroke_points` array used to track mouse gestures.
 * It should be called during application shutdown to release memory.
 *
 * \note After this call, stroke input must not be recorded until `x_stroke_init()` is called again.
 * \warning Not idempotent. Will fail if stroke system was not initialized.
 */
void
x_stroke_free (void)
{
  g_return_if_fail (stroke_points != NULL);

  g_array_free (stroke_points, TRUE);
  stroke_points = NULL;
}

/*! \brief Record and draw a stroke gesture point.
 *
 * Appends a new (x, y) point to the stroke trail, sends it to libstroke,
 * and draws a line to the previous point for visual feedback.
 *
 * \param [in] w_current The active GschemToplevel.
 * \param [in] x         The current X coordinate (device pixels).
 * \param [in] y         The current Y coordinate (device pixels).
 *
 * \note Drawing uses the world-to-screen matrix from page geometry.
 *       Function is a no-op for the first point (no prior segment to draw).
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

    GtkWidget *widget = GTK_WIDGET(view);
    if (gtk_widget_get_window(widget) != NULL) {
      cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
      g_return_if_fail(cr != NULL);

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
  } // <-- closes STROKE_MAX_POINTS guard
}

/*! \brief Translate and evaluate the current stroke sequence.
 *
 * Converts the list of recorded stroke points into a gesture string,
 * evaluates it using the embedded Guile interpreter, and erases the
 * temporary on-screen stroke trace.
 *
 * \param [in] w_current The active GschemToplevel.
 * \return 1 if stroke was successfully translated and evaluated, 0 otherwise.
 */
gint
x_stroke_translate_and_execute (GschemToplevel *w_current)
{
  gchar sequence[STROKE_MAX_SEQUENCE];
  StrokePoint *point;
  int min_x, min_y, max_x, max_y;
  gint i;

  g_assert (stroke_points != NULL);

  // No points = no stroke
  if (stroke_points->len == 0)
    return 0;

  // Initialize bounding box
  point = &g_array_index (stroke_points, StrokePoint, 0);
  min_x = max_x = point->x;
  min_y = max_y = point->y;

  // Expand bounding box over all points
  for (i = 1; i < stroke_points->len; i++) {
    point = &g_array_index (stroke_points, StrokePoint, i);
    min_x = MIN (min_x, point->x);
    min_y = MIN (min_y, point->y);
    max_x = MAX (max_x, point->x);
    max_y = MAX (max_y, point->y);
  }

  // Erase stroke trail by invalidating the region
  o_invalidate_rect (w_current, min_x, min_y, max_x + 1, max_y + 1);

  // Reset the stroke buffer
  stroke_points->len = 0;

  // Ask libstroke to translate the recorded gesture into a sequence
  if (stroke_trans ((char *)sequence)) {
    gchar *guile_string = g_strdup_printf("(eval-stroke \"%s\")", sequence);
    SCM ret;

    scm_dynwind_begin (0);
    scm_dynwind_unwind_handler (g_free, guile_string, SCM_F_WIND_EXPLICITLY);

    // Execute Scheme expression and capture result
    ret = g_scm_c_eval_string_protected (guile_string);

    scm_dynwind_end ();

    // Return true if expression was not false
    return (SCM_NFALSEP (ret));
  }

  // Stroke could not be interpreted
  return 0;
}