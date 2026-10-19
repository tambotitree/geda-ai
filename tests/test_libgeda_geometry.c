/* Regression checks for array indices and reverse sweep iteration. */
#include <config.h>
#include <assert.h>
#include <math.h>
#include "libgeda_priv.h"

int
main (void)
{
  sPOINT corners[] = {{0, 0}, {1000, 0}, {1000, 1000}, {0, 1000}};
  GArray *points = g_array_new (FALSE, FALSE, sizeof (sPOINT));
  GArray *lines = g_array_new (FALSE, FALSE, sizeof (LINE));
  TRANSFORM transform;
  TRANSFORM inverse;

  m_transform_init (&transform);
  m_transform_points (&transform, points);
  assert (points->len == 0);

  g_array_append_vals (points, corners, G_N_ELEMENTS (corners));
  assert (m_polygon_interior_point (points, 500, 500));
  assert (!m_polygon_interior_point (points, 1500, 500));
  assert (fabs (m_polygon_shortest_distance (points, 1500, 500, TRUE)
                - 500.0) < 0.001);

  m_hatch_polygon (points, 0, 100, lines);
  assert (lines->len > 0);
  for (guint i = 0; i < lines->len; i++) {
    LINE *line = &g_array_index (lines, LINE, i);
    assert (line->y[0] == line->y[1]);
    assert (line->x[0] >= 0 && line->x[0] <= 1000);
    assert (line->x[1] >= 0 && line->x[1] <= 1000);
    assert (line->y[0] >= 0 && line->y[0] <= 1000);
  }

  m_transform_rotate (&transform, 90);
  m_transform_points (&transform, points);
  assert (g_array_index (points, sPOINT, 1).x == 0);
  assert (g_array_index (points, sPOINT, 1).y == 1000);
  m_transform_invert (&transform, &inverse);
  m_transform_points (&inverse, points);
  for (guint i = 0; i < points->len; i++) {
    sPOINT actual = g_array_index (points, sPOINT, i);
    assert (actual.x == corners[i].x);
    assert (actual.y == corners[i].y);
  }

  g_array_free (lines, TRUE);
  g_array_free (points, TRUE);
  g_print ("PASS: polygon containment, distance, hatching, and transform round trip\n");
  return 0;
}
