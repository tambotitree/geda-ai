"""Verify picker overrides survive placement copies and file round trips."""
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
source = (ROOT/'gschem/src/gschem_compselect_dockable.c').read_text()
start = source.index('static GList *\ncompselect_apply_attributes')
_, end = _brace_body(source, source.index('{', start))
routine = source[start:end]
code = r'''
#include "gschem.h"
enum { ATTRIBUTE_COLUMN_NAME, ATTRIBUTE_COLUMN_VALUE, ATTRIBUTE_COLUMN_EDITED };
''' + routine + r'''
static void check_attributes(OBJECT *component) {
  gchar *refdes = o_attrib_search_object_attribs_by_name(component, "refdes", 0);
  gchar *value = o_attrib_search_object_attribs_by_name(component, "value", 0);
  g_assert_cmpstr(refdes, ==, "L1");
  g_assert_cmpstr(value, ==, "XYZ mH");
  g_free(refdes); g_free(value);
}
int main(int argc, char **argv) {
  g_assert_cmpint(argc, ==, 2);
  scm_init_guile(); libgeda_init();
  TOPLEVEL *top = s_toplevel_new();
  s_clib_add_directory(argv[1], "fixture");
  const CLibSymbol *symbol = s_clib_get_symbol_by_name("coil-1.sym");
  g_assert_nonnull(symbol);
  OBJECT *component = o_complex_new(top, OBJ_COMPLEX, DEFAULT_COLOR,
                                    0, 0, 0, 0, symbol, "coil-1.sym", 1);
  GList *objects = o_complex_promote_attribs(top, component);
  objects = g_list_append(objects, component);
  GtkListStore *model = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
  GtkTreeIter row;
  gtk_list_store_append(model, &row);
  gtk_list_store_set(model, &row, 0, "refdes", 1, "L1", 2, TRUE, -1);
  gtk_list_store_append(model, &row);
  gtk_list_store_set(model, &row, 0, "value", 1, "XYZ mH", 2, TRUE, -1);
  objects = compselect_apply_attributes(GTK_TREE_MODEL(model), top, objects);
  check_attributes(component);
  guint count = g_list_length(objects);
  objects = compselect_apply_attributes(GTK_TREE_MODEL(model), top, objects);
  g_assert_cmpuint(g_list_length(objects), ==, count);
  GList *placed = o_glist_copy_all(top, objects, NULL);
  gchar *buffer = o_save_buffer(placed);
  GError *error = NULL;
  GList *loaded = o_read_buffer(top, NULL, buffer, -1, "round trip", &error);
  g_assert_no_error(error);
  gboolean found = FALSE;
  for (GList *item = loaded; item != NULL; item = item->next) {
    OBJECT *object = item->data;
    if (object->type == OBJ_COMPLEX) { check_attributes(object); found = TRUE; }
  }
  g_assert_true(found);
  gchar *original = s_clib_symbol_get_data(symbol);
  g_assert_nonnull(strstr(original, "refdes=L?"));
  g_assert_null(strstr(original, "XYZ mH"));
  g_free(original); g_free(buffer);
  s_delete_object_glist(top, loaded);
  s_delete_object_glist(top, placed);
  s_delete_object_glist(top, objects);
  g_object_unref(model);
  s_toplevel_delete(top);
  g_print("PASS: edited attributes, repeat placement, save/reload, unchanged library\n");
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
                    '-L'+str(BUILD_ROOT/'libgedacairo/.libs'), '-lgedacairo',
                    '-L'+str(BUILD_ROOT/'libgeda/src/.libs'), '-lgeda',
                    '-o', str(work/'check'), *flags], check=True)
    environment = dict(os.environ)
    libraries = ':'.join(str(BUILD_ROOT/p) for p in ['libgedacairo/.libs', 'libgeda/src/.libs'])
    environment['DYLD_LIBRARY_PATH'] = libraries
    environment['LD_LIBRARY_PATH'] = libraries
    subprocess.run([str(work/'check'), str(ROOT/'symbols/analog')],
                   env=environment, check=True)
