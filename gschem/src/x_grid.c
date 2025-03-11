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

#include <math.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"

#define DOTS_VARIABLE_MODE_SPACING   30

#define MESH_COARSE_GRID_MULTIPLIER  5


/*! \brief Query the spacing in world coordinates at which the dots grid is drawn.
 *
 *  \par Function Description
 *  Returns the world spacing of the rendered grid, taking into account where
 *  the grid drawing code may drop elements which are too densely packed for a
 *  given zoom level.
 *
 *  \param [in] w_current  The GschemToplevel.
 *  \returns The grid spacing in world units of the grid as rendered, or -1
 *           if there are no items drawn.
 */
static int query_dots_grid_spacing (GschemToplevel *w_current)
{
  int incr, screen_incr;

  g_assert (w_current != NULL);

  int snap_size = gschem_options_get_snap_size (w_current->options);

  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_val_if_fail (page_view != NULL, -1);

  GschemPageGeometry *geometry = gschem_page_view_get_page_geometry (page_view);

  /* geometry may be NULL if page_view has no underlying page */
  if (geometry == NULL) {
    return -1;
  }

  if (w_current->dots_grid_mode == DOTS_GRID_VARIABLE_MODE) {
    /* In the variable mode around every (DOTS_VARIABLE_MODE_SPACING)'th
     * screenpixel will be grid-point. */
    /* adding 0.1 for correct cast*/
    incr = round_5_2_1 (geometry->to_world_x_constant * DOTS_VARIABLE_MODE_SPACING) + 0.1;

    /* limit minimum grid spacing to grid to snap_size */
    if (incr < snap_size) {
      incr = snap_size;
    }
  } else {
    /* Fixed size grid in world coorinates */
    incr = snap_size;
    screen_incr = gschem_page_view_SCREENabs (page_view, incr);
    if (screen_incr < w_current->dots_grid_fixed_threshold) {
      /* No grid drawn if the on-screen spacing is less than the threshold */
      incr = -1;
    }
  }
  return incr;
}

/*! \brief Draw an area of the screen with a dotted grid pattern
 *
 *  \par Function Description
 *  Draws the dotted grid pattern over a given region of the screen.
 *
 *  \param [in] w_current  The GschemToplevel.
 *  \param [in] x          The left screen coordinate for the drawing.
 *  \param [in] y          The top screen coordinate for the drawing.
 *  \param [in] width      The width of the region to draw.
 *  \param [in] height     The height of the region to draw.
 */
static void
draw_dots_grid_region (GschemToplevel *w_current, cairo_t *cr, int x, int y, int width, int height)
{
  int incr = query_dots_grid_spacing (w_current);

  if (incr == -1)
    return;

  int dot_size = min (w_current->dots_grid_dot_size, 5);

  COLOR *color = x_color_lookup (DOTS_GRID_COLOR);
  cairo_set_source_rgba (cr,
                         color->r / 255.0,
                         color->g / 255.0,
                         color->b / 255.0,
                         color->a / 255.0);

  cairo_matrix_t user_to_device_matrix;
  double x_start = x - 1;
  double y_start = y + height + 1;
  double x_end = x + width + 1;
  double y_end = y - 1;
  int xs, ys, xe, ye;
  int i, j;
  double x1, y1;

  cairo_device_to_user (cr, &x_start, &y_start);
  cairo_device_to_user (cr, &x_end, &y_end);

  cairo_get_matrix (cr, &user_to_device_matrix);
  cairo_save (cr);
  cairo_identity_matrix (cr);

  /* figure starting grid coordinates, work by taking the start
   * and end coordinates and rounding down to the nearest increment */
  xs = (floor (x_start/incr) - 1) * incr;
  ys = (floor (y_start/incr) - 1) * incr;
  xe = ceil (x_end);
  ye = ceil (y_end);

  for (j = ys; j < ye; j = j + incr) {
    for (i = xs; i < xe; i = i + incr) {
      x1 = i;
      y1 = j;

      cairo_matrix_transform_point (&user_to_device_matrix, &x1, &y1);

      if (w_current->dots_grid_dot_size == 1) {
        cairo_rectangle (cr, round (x1), round (y1), 1, 1);
      } else {
        cairo_move_to (cr, round (x1), round (y1));
        cairo_arc (cr, round (x1), round (y1),
                   dot_size/2,
                   0,
                   2*M_PI);
      }
    }
  }

  cairo_fill (cr);
  cairo_restore (cr);
}


/*! \brief Helper function for draw_mesh_grid_region
 */
static void draw_mesh (GschemToplevel *w_current,
                       cairo_t *cr,
                       cairo_matrix_t *user_to_device_matrix,
                       int color,
                       int x_start, int y_start, int x_end, int y_end,
                       int incr, int coarse_mult,
                       gboolean leave_out_origin)
{
  int i, j;
  double x1, y1, x2, y2;
  int next_coarse_x, next_coarse_y;
  int coarse_incr = incr * coarse_mult;
  COLOR *c;

  /* figure starting grid coordinates, work by taking the start
   * and end coordinates and rounding down to the nearest increment */
  x_start -= (x_start % incr) + (x_start < 0 ? incr : 0);
  y_start -= (y_start % incr) + (y_start < 0 ? incr : 0);

  if (coarse_incr == 0) {
    next_coarse_x = x_start - 1; /* Ensures we never hit this when looping */
    next_coarse_y = y_start - 1; /* Ensures we never hit this when looping */
  } else {
    next_coarse_x = x_start - (x_start % coarse_incr);
    next_coarse_y = y_start - (y_start % coarse_incr);
    if (next_coarse_x < x_start) next_coarse_x += coarse_incr;
    if (next_coarse_y < y_start) next_coarse_y += coarse_incr;
  }

  c = x_color_lookup (color);
  cairo_set_source_rgba (cr,
                         (double)c->r / 255.0,
                         (double)c->g / 255.0,
                         (double)c->b / 255.0,
                         (double)c->a / 255.0);

  cairo_set_line_width (cr, 1.);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);

  for (j = y_start; j < y_end; j = j + incr) {

    /* Skip lines which will be drawn in the coarser grid */
    if (j == next_coarse_y) {
      next_coarse_y += coarse_incr;
      continue;
    }
    if (j == 0 && leave_out_origin)
      continue;

    x1 = x_start;
    y1 = j;
    x2 = x_end;
    y2 = j;

    cairo_matrix_transform_point (user_to_device_matrix, &x1, &y1);
    cairo_matrix_transform_point (user_to_device_matrix, &x2, &y2);

    cairo_move_to (cr, (int)(x1+0.5), (int)(y1+0.5));
    cairo_line_to (cr, (int)(x2+0.5), (int)(y2+0.5));

    cairo_stroke (cr);
  }

  for (i = x_start; i < x_end; i = i + incr) {

    /* Skip lines which will be drawn in the coarser grid */
    if (i == next_coarse_x) {
      next_coarse_x += coarse_incr;
      continue;
    }
    if (i == 0 && leave_out_origin)
      continue;

    x1 = i;
    y1 = y_start;
    x2 = i;
    y2 = y_end;

    cairo_matrix_transform_point (user_to_device_matrix, &x1, &y1);
    cairo_matrix_transform_point (user_to_device_matrix, &x2, &y2);

    cairo_move_to (cr, (int)(x1+0.5), (int)(y1+0.5));
    cairo_line_to (cr, (int)(x2+0.5), (int)(y2+0.5));

    cairo_stroke (cr);
  }
}


/*! \brief Helper function for draw_mesh_grid_region
 */
static void draw_origin (GschemToplevel *w_current,
                         cairo_t *cr,
                         cairo_matrix_t *user_to_device_matrix,
                         int x_start, int y_start, int x_end, int y_end)
{
  COLOR *c = x_color_lookup (ORIGIN_COLOR);
  cairo_set_source_rgba (cr, (double)c->r / 255.,
                             (double)c->g / 255.,
                             (double)c->b / 255.,
                             (double)c->a / 255.);
  cairo_set_line_width (cr, 1.);
  cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE);

  double x1, y1, x2, y2;

  x1 = x_start;  y1 = 0;
  x2 = x_end;    y2 = 0;
  cairo_matrix_transform_point (user_to_device_matrix, &x1, &y1);
  cairo_matrix_transform_point (user_to_device_matrix, &x2, &y2);

  cairo_move_to (cr, (int)(x1 + .5), (int)(y1 + .5));
  cairo_line_to (cr, (int)(x2 + .5), (int)(y2 + .5));
  cairo_stroke (cr);

  x1 = 0;  y1 = y_start;
  x2 = 0;  y2 = y_end;
  cairo_matrix_transform_point (user_to_device_matrix, &x1, &y1);
  cairo_matrix_transform_point (user_to_device_matrix, &x2, &y2);

  cairo_move_to (cr, (int)(x1 + .5), (int)(y1 + .5));
  cairo_line_to (cr, (int)(x2 + .5), (int)(y2 + .5));
  cairo_stroke (cr);
}


/*! \brief Query the spacing in world coordinates at which the mesh grid is drawn.
 *
 *  \par Function Description
 *  Returns the world spacing of the rendered grid, taking into account where
 *  the grid drawing code may drop elements which are too densely packed for a
 *  given zoom level.
 *
 *  \param [in] w_current  The GschemToplevel.
 *  \returns The grid spacing in world units of the grid as rendered, or -1
 *           if there are no items drawn.
 */
static int query_mesh_grid_spacing (GschemToplevel *w_current)
{
  int incr, screen_incr;

  GschemPageView *page_view = gschem_toplevel_get_current_page_view (w_current);
  g_return_val_if_fail (page_view != NULL, -1);

  incr = gschem_options_get_snap_size (w_current->options);
  screen_incr = gschem_page_view_SCREENabs (page_view, incr);

  /* We draw a fine grid if its on-screen spacing is large enough */
  if (screen_incr >= w_current->mesh_grid_display_threshold) {
    return incr;
  }

  incr *= MESH_COARSE_GRID_MULTIPLIER;
  screen_incr = gschem_page_view_SCREENabs (page_view, incr);

  /* We draw a coarse grid if its on-screen spacing is large enough */
  if (screen_incr >= w_current->mesh_grid_display_threshold)
    return incr;

  return -1;
}

/*! \brief Draw an area of the screen with a mesh grid pattern
 *
 *  \par Function Description
 *  Draws the mesh grid pattern over a given region of the screen.
 *
 *  \param [in] w_current  The GschemToplevel.
 *  \param [in] cr         The cairo context.
 *  \param [in] x          The left screen coordinate for the drawing.
 *  \param [in] y          The top screen coordinate for the drawing.
 *  \param [in] width      The width of the region to draw.
 *  \param [in] height     The height of the region to draw.
 */
static void
draw_mesh_grid_region (GschemToplevel *w_current, cairo_t *cr, int x, int y, int width, int height)
{
  int snap_size = gschem_options_get_snap_size (w_current->options);
  int coarse_increment = MESH_COARSE_GRID_MULTIPLIER * snap_size;
  double dummy = 0.0;
  double threshold = w_current->mesh_grid_display_threshold;
  gboolean show_origin = gschem_options_get_show_origin (w_current->options);

  cairo_device_to_user_distance (cr, &threshold, &dummy);

  if (coarse_increment >= threshold || show_origin) {
    cairo_matrix_t user_to_device_matrix;
    double x_start = x - 1;
    double y_start = y + height + 1;
    double x_end = x + width + 1;
    double y_end = y - 1;

    cairo_device_to_user (cr, &x_start, &y_start);
    cairo_device_to_user (cr, &x_end, &y_end);

    cairo_get_matrix (cr, &user_to_device_matrix);
    cairo_save (cr);
    cairo_identity_matrix (cr);
    cairo_translate (cr, 0.5, 0.5);

    /* Draw the fine grid if its on-screen spacing is large enough */
    if (snap_size >= threshold) {
      draw_mesh (w_current,
                 cr,
                 &user_to_device_matrix,
                 MESH_GRID_MINOR_COLOR,
                 floor (x_start),
                 floor (y_start),
                 ceil (x_end),
                 ceil (y_end),
                 snap_size,
                 MESH_COARSE_GRID_MULTIPLIER,
                 show_origin);
    }

    if (coarse_increment >= threshold)
      draw_mesh (w_current,
                 cr,
                 &user_to_device_matrix,
                 MESH_GRID_MAJOR_COLOR,
                 floor (x_start),
                 floor (y_start),
                 ceil (x_end),
                 ceil (y_end),
                 coarse_increment,
                 0,
                 show_origin);

    if (show_origin)
      draw_origin (w_current,
                   cr,
                   &user_to_device_matrix,
                   floor (x_start),
                   floor (y_start),
                   ceil (x_end),
                   ceil (y_end));

    cairo_restore (cr);
  }
}


/*! \brief Draw an area of the screen with the current grid pattern.
 *
 *  \par Function Description
 *  Draws the desired grid pattern over a given region of the screen.
 *
 *  \param [in] w_current  The GschemToplevel.
 *  \param [in] cr         The cairo context.
 *  \param [in] x          The left screen coordinate for the drawing.
 *  \param [in] y          The top screen coordinate for the drawing.
 *  \param [in] width      The width of the region to draw.
 *  \param [in] height     The height of the region to draw.
 */
void x_grid_draw_region (GschemToplevel *w_current,
                         cairo_t *cr,
                         int x,
                         int y,
                         int width,
                         int height)
{
  GRID_MODE grid_mode;

  g_return_if_fail (w_current != NULL);

  grid_mode = gschem_options_get_grid_mode (w_current->options);

  switch (grid_mode) {
    case GRID_MODE_NONE:
      return;

    case GRID_MODE_DOTS:
      draw_dots_grid_region (w_current, cr, x, y, width, height);
      break;

    case GRID_MODE_MESH:
      draw_mesh_grid_region (w_current, cr, x, y, width, height);
      break;
    default: g_assert_not_reached ();
  }
}


/*! \brief Query the spacing in world coordinates at which the grid is drawn.
 *
 *  \par Function Description
 *  Returns the world spacing of the rendered grid, taking into account where
 *  the grid drawing code may drop elements which are too densely packed for a
 *  given zoom level.
 *
 *  \param [in] w_current  The GschemToplevel.
 *  \returns The grid spacing in world units of the grid as rendered, or -1
 *           if there are no items drawn.
 */
int x_grid_query_drawn_spacing (GschemToplevel *w_current)
{
  GRID_MODE grid_mode;

  g_return_val_if_fail (w_current != NULL, -1);

  grid_mode = gschem_options_get_grid_mode (w_current->options);

  switch (grid_mode) {
    default:
    case GRID_MODE_NONE: return -1;
    case GRID_MODE_DOTS: return query_dots_grid_spacing (w_current);
    case GRID_MODE_MESH: return query_mesh_grid_spacing (w_current);
  }
}
