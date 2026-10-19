/* Headless checks for rendering, color bounds, and Cairo context ownership. */
#include <config.h>
#include <stdint.h>
#include <libgedacairo/libgedacairo.h>

typedef struct {
  cairo_surface_t *surface;
  cairo_t *cr;
  EdaRenderer *renderer;
  GArray *colors;
} RenderFixture;

/* Create a transparent image with one enabled, opaque black drawing color. */
static RenderFixture
fixture_new (void)
{
  RenderFixture f;
  COLOR black = {0, 0, 0, 255, TRUE};
  f.surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 128, 128);
  f.cr = cairo_create (f.surface);
  cairo_scale (f.cr, 0.1, 0.1);
  f.renderer = eda_renderer_new (f.cr, NULL);
  f.colors = g_array_new (FALSE, FALSE, sizeof (COLOR));
  g_array_append_val (f.colors, black);
  eda_renderer_set_color_map (f.renderer, f.colors);
  return f;
}

/* Release the renderer before the caller's context, surface, and color map. */
static void
fixture_free (RenderFixture *f)
{
  eda_renderer_destroy (f->renderer);
  g_assert_cmpuint (cairo_get_reference_count (f->cr), ==, 1);
  cairo_destroy (f->cr);
  cairo_surface_destroy (f->surface);
  g_array_free (f->colors, TRUE);
}

/* Count actual painted pixels rather than accepting a successful API return. */
static guint
painted_pixels_in_region (cairo_surface_t *surface, int start, int end)
{
  guint count = 0;
  cairo_surface_flush (surface);
  const unsigned char *data = cairo_image_surface_get_data (surface);
  int stride = cairo_image_surface_get_stride (surface);
  for (int y = start; y < end; y++) {
    const uint32_t *row = (const uint32_t *) (data + y * stride);
    for (int x = start; x < end; x++) {
      if ((row[x] >> 24) != 0) count++;
    }
  }
  return count;
}

/* Count painted pixels across the complete test surface. */
static guint
painted_pixels (cairo_surface_t *surface)
{
  return painted_pixels_in_region (surface, 0, 128);
}

/* Draw a net and verify that it produces pixels without Cairo errors. */
static void
test_net (void)
{
  RenderFixture f = fixture_new ();
  LINE line = {{100, 900}, {500, 500}};
  OBJECT net = {.type = OBJ_NET, .color = 0, .line = &line};
  eda_renderer_draw (f.renderer, &net);
  g_assert_cmpint (cairo_status (f.cr), ==, CAIRO_STATUS_SUCCESS);
  g_assert_cmpuint (painted_pixels (f.surface), >, 0);
  fixture_free (&f);
}

/* Exercise Pango glyph rendering and the resulting font cache teardown. */
static void
test_text (void)
{
  RenderFixture f = fixture_new ();
  TEXT text = {.x = 200, .y = 600, .size = 10, .alignment = LOWER_LEFT,
               .string = "R1", .disp_string = "R1"};
  OBJECT label = {.type = OBJ_TEXT, .color = 0, .text = &text,
                  .visibility = VISIBLE};
  eda_renderer_draw (f.renderer, &label);
  g_assert_cmpint (cairo_status (f.cr), ==, CAIRO_STATUS_SUCCESS);
  g_assert_cmpuint (painted_pixels (f.surface), >, 0);
  fixture_free (&f);
}

/* Exercise each typed hatch dispatcher with both hatch and mesh fills. */
static void
test_hatching (void)
{
  BOX box = {100, 900, 900, 100};
  CIRCLE circle = {500, 500, 300};
  PATH_SECTION sections[] = {
    {.code = PATH_MOVETO, .x3 = 100, .y3 = 100},
    {.code = PATH_LINETO, .x3 = 900, .y3 = 100},
    {.code = PATH_LINETO, .x3 = 500, .y3 = 900},
    {.code = PATH_END}
  };
  PATH path = {sections, G_N_ELEMENTS (sections), G_N_ELEMENTS (sections)};
  const int types[] = {OBJ_BOX, OBJ_CIRCLE, OBJ_PATH};
  const int fills[] = {FILLING_HATCH, FILLING_MESH};
  for (guint t = 0; t < G_N_ELEMENTS (types); t++) {
    for (guint fill = 0; fill < G_N_ELEMENTS (fills); fill++) {
      RenderFixture f = fixture_new ();
      OBJECT shape = {.type = types[t], .color = 0, .box = &box,
        .circle = &circle, .path = &path, .line_width = 10,
        .line_type = TYPE_SOLID, .line_end = END_NONE,
        .fill_type = fills[fill], .fill_width = 10,
        .fill_angle1 = 0, .fill_pitch1 = 100,
        .fill_angle2 = 90, .fill_pitch2 = 100};
      eda_renderer_draw (f.renderer, &shape);
      g_assert_cmpint (cairo_status (f.cr), ==, CAIRO_STATUS_SUCCESS);
      g_assert_cmpuint (painted_pixels (f.surface), >, 100);
      /* This region is strictly inside all three shapes, away from outlines. */
      g_assert_cmpuint (painted_pixels_in_region (f.surface, 42, 58), >, 0);
      fixture_free (&f);
    }
  }
}

/* Invalid color indices must be rejected before indexing the color array. */
static void
test_color_bounds (void)
{
  RenderFixture f = fixture_new ();
  LINE line = {{100, 900}, {500, 500}};
  OBJECT net = {.type = OBJ_NET, .color = -1, .line = &line};
  g_test_expect_message ("libgedacairo", G_LOG_LEVEL_CRITICAL, "*assertion*failed*");
  eda_renderer_draw (f.renderer, &net);
  g_test_assert_expected_messages ();
  net.color = 1;
  g_test_expect_message ("libgedacairo", G_LOG_LEVEL_CRITICAL, "*assertion*failed*");
  eda_renderer_draw (f.renderer, &net);
  g_test_assert_expected_messages ();
  g_assert_cmpuint (painted_pixels (f.surface), ==, 0);
  fixture_free (&f);
}

/* A caller may replace the context with the renderer's own borrowed context. */
static void
test_context_ownership (void)
{
  cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 32, 32);
  cairo_t *cr = cairo_create (surface);
  EdaRenderer *renderer = eda_renderer_new (cr, NULL);
  cairo_destroy (cr);
  cr = eda_renderer_get_cairo_context (renderer);
  g_object_set (renderer, "cairo-context", cr, NULL);
  g_assert_cmpint (cairo_status (eda_renderer_get_cairo_context (renderer)),
                   ==, CAIRO_STATUS_SUCCESS);
  PangoContext *pc = NULL;
  g_object_get (renderer, "pango-context", &pc, NULL);
  g_assert_nonnull (pc);
  g_object_set (renderer, "pango-context", pc, NULL);
  eda_renderer_destroy (renderer);
  g_assert_cmpint (cairo_surface_status (surface), ==, CAIRO_STATUS_SUCCESS);
  cairo_surface_destroy (surface);
  renderer = eda_renderer_new (NULL, NULL);
  eda_renderer_destroy (renderer);
}

int
main (int argc, char **argv)
{
  g_test_init (&argc, &argv, NULL);
  g_test_message ("Cairo headers: %s; runtime: %s",
                  CAIRO_VERSION_STRING, cairo_version_string ());
  g_test_add_func ("/renderer/net", test_net);
  g_test_add_func ("/renderer/text", test_text);
  g_test_add_func ("/renderer/hatching", test_hatching);
  g_test_add_func ("/renderer/color-bounds", test_color_bounds);
  g_test_add_func ("/renderer/context-ownership", test_context_ownership);
  return g_test_run ();
}
