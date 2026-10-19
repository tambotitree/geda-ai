"""Render the production viewport routine with translated GTK-like contexts."""
from pathlib import Path
import os
import shlex
import subprocess
import tempfile
import sys

ROOT = Path(__file__).resolve().parents[1]
BUILD_ROOT = Path(os.environ.get("GEDA_TEST_BUILD_ROOT", ROOT))
sys.path.insert(0, str(ROOT))
from gedai.refactor.oracle import _brace_body

source = (ROOT / 'gschem/src/o_basic.c').read_text()
start = source.index('void o_redraw_rect(')
_, end = _brace_body(source, source.index('{', start))
routine = source[start:end]
rubber = ['o_place', 'o_move', 'o_arc', 'o_box', 'o_circle', 'o_line',
          'o_path', 'o_picture', 'o_pin', 'o_bus', 'o_net', 'o_grips',
          'o_select_box', 'a_zoom_box', 'o_ognrst']
code = r'''
#include "gschem.h"
#include <stdint.h>
#include <math.h>
COLOR display_colors[MAX_COLORS];
COLOR display_outline_colors[MAX_COLORS];
static int expected_grid[4];
TOPLEVEL *gschem_toplevel_get_toplevel (GschemToplevel *window) {
  return window->toplevel;
}
GschemPageView *gschem_toplevel_get_current_page_view (GschemToplevel *window) {
  return (GschemPageView *)window->drawing_area;
}
COLOR *x_color_lookup (int index) { return &display_colors[index]; }
void x_grid_draw_region (GschemToplevel *window, cairo_t *cr,
                          int x, int y, int width, int height) {
  (void)window; (void)cr;
  g_assert_cmpint(x, ==, expected_grid[0]);
  g_assert_cmpint(y, ==, expected_grid[1]);
  g_assert_cmpint(width, ==, expected_grid[2]);
  g_assert_cmpint(height, ==, expected_grid[3]);
}
''' + ''.join(f'#define {name}_draw_rubber(...) ((void)0)\n' for name in rubber) + routine + r'''
/* Count visible green symbol pixels inside the widget's device rectangle. */
static guint render_coil (TOPLEVEL *top, PAGE *page, int offset_x, int offset_y,
                           int scale, const char *output) {
  const int width = 200, height = 140;
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
      offset_x + width * scale + 20, offset_y + height * scale + 20);
  cairo_t *cr = cairo_create(surface);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);
  cairo_translate(cr, offset_x, offset_y);
  cairo_scale(cr, scale, scale);
  cairo_matrix_t original, restored;
  cairo_get_matrix(cr, &original);
  double clip_before[4], clip_after[4];
  cairo_clip_extents(cr, &clip_before[0], &clip_before[1], &clip_before[2], &clip_before[3]);
  expected_grid[0] = offset_x; expected_grid[1] = offset_y;
  expected_grid[2] = width * scale; expected_grid[3] = height * scale;
  GschemToplevel window = {0};
  window.toplevel = top;
  window.renderer = eda_renderer_new(cr, NULL);
  GschemPageGeometry *geometry = gschem_page_geometry_new_with_values(
      width, height, 0, 0, 1200, 800, 0, 0, 1200, 800);
  gschem_page_geometry_zoom_extents(geometry, top, s_page_objects(page));
  GdkRectangle rectangle = {0, 0, width, height};
  o_redraw_rect(&window, cr, page, geometry, &rectangle);
  cairo_get_matrix(cr, &restored);
  cairo_clip_extents(cr, &clip_after[0], &clip_after[1], &clip_after[2], &clip_after[3]);
  for (guint i = 0; i < G_N_ELEMENTS(clip_before); i++)
    g_assert_cmpfloat(clip_before[i], ==, clip_after[i]);
  g_assert_cmpfloat(original.xx, ==, restored.xx);
  g_assert_cmpfloat(original.yy, ==, restored.yy);
  g_assert_cmpfloat(original.x0, ==, restored.x0);
  g_assert_cmpfloat(original.y0, ==, restored.y0);
  cairo_surface_flush(surface);
  const unsigned char *bytes = cairo_image_surface_get_data(surface);
  const int stride = cairo_image_surface_get_stride(surface);
  guint green = 0;
  for (int y = offset_y; y < offset_y + height * scale; y++) {
    const uint32_t *row = (const uint32_t *)(bytes + y * stride);
    for (int x = offset_x; x < offset_x + width * scale; x++) {
      uint32_t pixel = row[x];
      if (((pixel >> 8) & 255) > 64 && ((pixel >> 16) & 255) < 32)
        green++;
    }
  }
  g_assert_cmpint(cairo_surface_write_to_png(surface, output), ==, CAIRO_STATUS_SUCCESS);
  g_object_unref(window.renderer);
  gschem_page_geometry_free(geometry);
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  return green;
}
/* Use the real renderer to supply text extents without a GTK window. */
static int text_bounds (void *data, OBJECT *object,
                         int *left, int *top, int *right, int *bottom) {
  EdaRenderer *renderer = data;
  double l, t, r, b;
  int result = eda_renderer_get_user_bounds(renderer, object, &l, &t, &r, &b);
  if (result) {
    *left = (int)floor(fmin(l, r)); *right = (int)ceil(fmax(l, r));
    *top = (int)floor(fmin(t, b)); *bottom = (int)ceil(fmax(t, b));
  }
  return result;
}
int main (int argc, char **argv) {
  g_assert_cmpint(argc, ==, 2);
  scm_init_guile();
  libgeda_init();
  for (int i = 0; i < MAX_COLORS; i++) {
    display_colors[i] = (COLOR){0, 255, 0, 255, TRUE};
    display_outline_colors[i] = display_colors[i];
  }
  display_colors[BACKGROUND_COLOR] = (COLOR){0, 0, 0, 255, TRUE};
  TOPLEVEL *top = s_toplevel_new();
  cairo_surface_t *metrics_surface = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, NULL);
  cairo_t *metrics_cr = cairo_create(metrics_surface);
  EdaRenderer *metrics = eda_renderer_new(metrics_cr, NULL);
  o_text_set_rendered_bounds_func(top, text_bounds, metrics);
  top->init_right = 1200;
  top->init_bottom = 800;
  PAGE *page = s_page_new(top, "viewport-test");
  top->page_current = page;
  gchar *buffer = NULL;
  GError *error = NULL;
  g_assert_true(g_file_get_contents(argv[1], &buffer, NULL, &error));
  GList *objects = o_read_buffer(top, NULL, buffer, -1, "coil fixture", &error);
  g_assert_no_error(error);
  s_page_append_list(top, page, objects);
  g_free(buffer);
  guint identity = render_coil(top, page, 0, 0, 1, "/tmp/geda-coil-origin.png");
  guint translated = render_coil(top, page, 400, 250, 1, "/tmp/geda-coil-translated.png");
  guint scaled = render_coil(top, page, 400, 250, 2, "/tmp/geda-coil-scaled.png");
  g_print("Symbol pixels: origin=%u translated=%u scaled=%u\n", identity, translated, scaled);
  g_assert_cmpuint(identity, >, 50);
  g_assert_cmpuint(translated, ==, identity);
  g_assert_cmpuint(scaled, >, 50);
  s_toplevel_delete(top);
  g_object_unref(metrics);
  cairo_destroy(metrics_cr);
  cairo_surface_destroy(metrics_surface);
  return 0;
}
'''
flags = shlex.split(subprocess.check_output(
    ['pkg-config', '--cflags', '--libs', 'gtk+-3.0', 'guile-3.0', 'pangocairo'], text=True))
with tempfile.TemporaryDirectory(prefix='geda-viewport-') as directory:
    work = Path(directory)
    (work / 'check.c').write_text(code)
    subprocess.run(['cc', '-std=c11', '-g', '-O0', '-Wall', '-Wextra', '-Werror',
                    '-Wpedantic', '-Wstrict-prototypes', '-Wmissing-prototypes', '-Wformat=2',
                    '-I'+str(BUILD_ROOT), '-I'+str(ROOT), '-I'+str(ROOT/'gschem/include'),
                    '-I'+str(ROOT/'libgeda/include'), str(work/'check.c'),
                    str(BUILD_ROOT/'gschem/src/gschem-gschem_page_geometry.o'),
                    '-L'+str(BUILD_ROOT/'libgedacairo/.libs'), '-lgedacairo',
                    '-L'+str(BUILD_ROOT/'libgeda/src/.libs'), '-lgeda',
                    '-o', str(work/'check'), *flags], check=True)
    environment = dict(os.environ)
    libraries = ':'.join(str(BUILD_ROOT/p) for p in ['libgedacairo/.libs', 'libgeda/src/.libs'])
    environment['DYLD_LIBRARY_PATH'] = libraries
    environment['LD_LIBRARY_PATH'] = libraries
    for symbol in ['diode/diode-1.sym', 'diode/diode-bridge-1.sym',
                   'analog/coil-2.sym', 'analog/coil-1.sym']:
        print('Checking', symbol, flush=True)
        subprocess.run([str(work/'check'), str(ROOT/'symbols'/symbol)],
                       env=environment, check=True)
