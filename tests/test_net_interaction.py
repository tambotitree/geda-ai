"""Exercise grid, resistor pin snapping, net completion and redraw bounds."""
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

def routine(file, signature):
    source = (ROOT/'gschem/src'/file).read_text()
    start = source.index(signature)
    _, end = _brace_body(source, source.index('{', start))
    return source[start:end]

source = (ROOT/'gschem/src/o_net.c').read_text()
constants = source[source.index('#define MAGNETIC_HALFSIZE'):source.index('/*! \\brief Reset')]
code = r'''
#include "gschem.h"
#include <math.h>
static PAGE *test_page;
static GschemPageGeometry *geometry;
static gboolean magnet = TRUE;
static SNAP_STATE snap_mode = SNAP_GRID;
static int damage[4];
static guint damaged;
GschemPageView *gschem_toplevel_get_current_page_view(GschemToplevel *w) {
  return (GschemPageView *)w->drawing_area;
}
PAGE *gschem_page_view_get_page(GschemPageView *view) {
  (void)view; return test_page;
}
int gschem_page_view_WORLDabs(GschemPageView *view, int pixels) {
  (void)view; return (int)round(pixels * geometry->to_world_x_constant);
}
int visible(GschemToplevel *w, int l, int t, int r, int b) {
  (void)w;
  return r >= geometry->viewport_left && l <= geometry->viewport_right &&
         b >= geometry->viewport_top && t <= geometry->viewport_bottom;
}
gboolean gschem_options_get_magnetic_net_mode(GschemOptions *o) {
  (void)o; return magnet;
}
SNAP_STATE gschem_options_get_snap_mode(GschemOptions *o) {
  (void)o; return snap_mode;
}
int gschem_options_get_snap_size(GschemOptions *o) { (void)o; return 100; }
void i_action_start(GschemToplevel *w) { w->inside_action = TRUE; }
void i_action_stop(GschemToplevel *w) { w->inside_action = FALSE; }
void o_net_invalidate_rubber(GschemToplevel *w) { (void)w; damaged++; }
void g_run_hook_object_list(GschemToplevel *w, const char *name, GList *objects) {
  (void)w; (void)name; (void)objects;
}
void gschem_toplevel_page_content_changed(GschemToplevel *w, PAGE *page) {
  (void)w; (void)page;
}
void o_undo_savestate_old(GschemToplevel *w, int flag, const gchar *desc) {
  (void)w; (void)flag; (void)desc;
}
int o_net_add_busrippers(GschemToplevel *w, OBJECT *net, GList *others) {
  (void)w; (void)net;
  for (GList *item=others;item;item=item->next)
    g_assert_cmpint(((OBJECT *)item->data)->type, !=, OBJ_BUS);
  return FALSE;
}
#undef GTK_WIDGET
#define GTK_WIDGET(view) ((GtkWidget *)(view))
#define gtk_widget_get_realized(widget) ((void)(widget), TRUE)
void gschem_page_view_invalidate_screen_rect(GschemPageView *view,
                                            int l, int t, int r, int b) {
  (void)view; damage[0]=l; damage[1]=t; damage[2]=r; damage[3]=b;
}
''' + constants
code += routine('m_basic.c', 'int snap_grid(')
for name in ['reset', 'guess_direction', 'find_magnetic', 'finishmagnetic',
             'start_magnetic', 'start', 'end', 'motion']:
    code += routine('o_net.c', 'void o_net_'+name+'(' if name != 'motion' else 'void o_net_motion (')
code += routine('o_basic.c', 'void o_invalidate_rect(')
code += r'''
static OBJECT *pin_at(OBJECT *component, int x) {
  for (GList *item=component->complex->prim_objs; item; item=item->next) {
    OBJECT *pin=item->data;
    if (pin->type==OBJ_PIN && pin->line->x[pin->whichend]==x) return pin;
  }
  g_assert_not_reached();
}
int main(int argc, char **argv) {
  g_assert_cmpint(argc, ==, 2);
  scm_init_guile(); libgeda_init();
  TOPLEVEL *top=s_toplevel_new();
  top->init_right=10000; top->init_bottom=6000;
  test_page=s_page_new(top, "two-resistors.sch"); top->page_current=test_page;
  s_clib_add_directory(argv[1], "fixture");
  const CLibSymbol *symbol=s_clib_get_symbol_by_name("resistor-1.sym");
  OBJECT *r1=o_complex_new(top, OBJ_COMPLEX, DEFAULT_COLOR, 1000,1000,0,0,
                           symbol,"resistor-1.sym",1);
  OBJECT *r2=o_complex_new(top, OBJ_COMPLEX, DEFAULT_COLOR, 4000,1000,0,0,
                           symbol,"resistor-1.sym",1);
  r1->complex_embedded=TRUE; r2->complex_embedded=TRUE;
  s_page_append(top,test_page,r1); s_page_append(top,test_page,r2);
  OBJECT *p1=pin_at(r1,1900), *p2=pin_at(r2,4000);
  GschemToplevel w={0}; w.toplevel=top; w.drawing_area=(GtkWidget *)&w;
  geometry=gschem_page_geometry_new_with_values(1000,600,0,0,10000,6000,
                                                0,0,10000,6000);
  g_assert_cmpint(snap_grid(&w,1949),==,1900);
  g_assert_cmpint(snap_grid(&w,1951),==,2000);
  g_assert_cmpint(snap_grid(&w,-1949),==,-1900);
  snap_mode=SNAP_OFF; g_assert_cmpint(snap_grid(&w,1949),==,1949);
  snap_mode=SNAP_GRID;
  for (int zoom=0;zoom<2;zoom++) {
    if (zoom) gschem_page_geometry_set_values(geometry,5,1000,600,
                                               500,500,5500,3500);
    int sx=gschem_page_geometry_pix_x(geometry,1900);
    int sy=gschem_page_geometry_pix_y(geometry,1100);
    g_assert_cmpint(snap_grid(&w,gschem_page_geometry_mil_x(geometry,sx)),==,1900);
    g_assert_cmpint(snap_grid(&w,gschem_page_geometry_mil_y(geometry,sy)),==,1100);
    o_net_find_magnetic(&w,1920,1120);
    g_assert_cmpint(w.magnetic_wx,==,1900);
    g_assert_cmpint(w.magnetic_wy,==,1100);
  }
  /* Start click must supersede an obsolete magnetic marker. */
  w.magnetic_wx=4000; w.magnetic_wy=1100;
  o_net_start(&w,1900,1100);
  g_assert_cmpint(w.first_wx,==,1900);
  o_net_motion(&w,3000,1600);
  /* The final click arrives without a matching motion event. */
  o_net_end(&w,4000,1100);
  OBJECT *net=NULL;
  for(const GList *item=s_page_objects(test_page);item;item=item->next) {
    OBJECT *object=item->data;
    if(object->type==OBJ_NET) { g_assert_null(net); net=object; }
  }
  g_assert_nonnull(net);
  g_assert_cmpint(net->line->x[0],==,1900);
  g_assert_cmpint(net->line->x[1],==,4000);
  g_assert_cmpint(net->line->y[0],==,1100);
  g_assert_cmpint(net->line->y[1],==,1100);
  g_assert_nonnull(g_list_find(test_page->connectible_list,p1));
  g_assert_nonnull(p1->conn_list); g_assert_nonnull(p2->conn_list);
  g_assert_cmpuint(g_list_length(net->conn_list),==,2);
  g_assert_cmpuint(damaged,>,1);
  o_invalidate_rect(&w,800,600,900,650);
  g_assert_cmpint(damage[0],<=,800); g_assert_cmpint(damage[1],<=,600);
  g_assert_cmpint(damage[2],>=,900); g_assert_cmpint(damage[3],>=,650);
  /* The electrical connection must survive serialization and page reload. */
  gchar *saved=o_save_buffer(s_page_objects(test_page));
  GError *error=NULL;
  GList *loaded=o_read_buffer(top,NULL,saved,-1,"saved schematic",&error);
  g_assert_no_error(error);
  PAGE *reopened=s_page_new(top,"reopened.sch");
  s_page_append_list(top,reopened,loaded);
  /* s_page_append_list transfers ownership of the list to the page. */
  g_free(saved);
  guint resistors=0, nets=0;
  for(const GList *item=s_page_objects(reopened);item;item=item->next) {
    OBJECT *object=item->data;
    if(object->type==OBJ_COMPLEX) resistors++;
    if(object->type==OBJ_NET) {
      nets++;
      g_assert_cmpuint(g_list_length(object->conn_list),==,2);
    }
  }
  g_assert_cmpuint(resistors,==,2); g_assert_cmpuint(nets,==,1);
  magnet=FALSE;
  w.magnetic_wx=1900; w.magnetic_wy=1100;
  o_net_start(&w,3000,2000);
  g_assert_cmpint(w.first_wx,==,3000);
  g_assert_cmpint(w.first_wy,==,2000);
  gschem_page_geometry_free(geometry);
  s_toplevel_delete(top);
  g_print("PASS: grid round trips, pin snapping, final click, two-resistor connectivity/save/reload, damage bounds\n");
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
    subprocess.run([str(work/'check'), str(ROOT/'symbols/analog')],
                   env=environment, check=True)
