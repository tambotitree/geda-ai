"""Exercise production alternative-layout widgets through parent/unparent teardown."""
from pathlib import Path
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT/'gschem/src/gschem_compselect_dockable.c').read_text()
a = source.index('  compselect->preview_expander = ', source.index('compselect_create_widget ('))
b = source.index('  /* Keep Attributes', a)
creation = source[a:b]
code = r'''
#include <gtk/gtk.h>
typedef struct {
  GtkWidget *preview_expander, *attribs_expander;
  GtkWidget *preview_box, *attribs_box;
  GtkWidget *preview_paned, *attribs_paned;
} Layout;
/*! Count actual object finalization to detect dangling pointers and leaks. */
static void gone(gpointer data, GObject *object) {
  guint *count = data;
  (void)object;
  (*count)++;
}
/*! Exercise production references both with and without a GTK container. */
static void check_lifetime(gboolean parented) {
  Layout layout = {0};
  Layout *compselect = &layout;
''' + creation + r'''
  GtkWidget **slots[] = {&layout.preview_expander, &layout.attribs_expander,
    &layout.preview_box, &layout.attribs_box, &layout.preview_paned, &layout.attribs_paned};
  GtkWidget *parent = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  g_object_ref_sink(parent);
  guint finalized = 0;
  for (guint i=0; i<G_N_ELEMENTS(slots); i++) {
    g_object_weak_ref(G_OBJECT(*slots[i]), gone, &finalized);
    if (parented) gtk_container_add(GTK_CONTAINER(parent), *slots[i]);
  }
  gtk_widget_destroy(parent);
  g_object_unref(parent);
  if (!parented) g_assert_cmpuint(finalized, ==, 0);
  for (guint i=0; i<G_N_ELEMENTS(slots); i++)
    g_assert_true(G_IS_OBJECT(*slots[i]));
  for (guint i=0; i<G_N_ELEMENTS(slots); i++) g_clear_object(slots[i]);
  g_assert_cmpuint(finalized, ==, 6);
}
/*! Run teardown under fatal GTK warnings after display initialization. */
int main(void) {
  if (!gtk_init_check(NULL,NULL)) return 77;
  g_log_set_always_fatal(G_LOG_FATAL_MASK|G_LOG_LEVEL_WARNING|G_LOG_LEVEL_CRITICAL);
  check_lifetime(FALSE);
  check_lifetime(TRUE);
  g_print("PASS: parented/unparented picker layouts finalize without GTK warnings\n");
  return 0;
}
'''
flags = shlex.split(subprocess.check_output(
    ['pkg-config', '--cflags', '--libs', 'gtk+-3.0'], text=True))
with tempfile.TemporaryDirectory(prefix='geda-picker-lifetime-') as directory:
    work = Path(directory)
    (work/'check.c').write_text(code)
    subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-Wpedantic',
                    '-Wstrict-prototypes', '-Wmissing-prototypes', '-Wformat=2',
                    str(work/'check.c'), '-o', str(work/'check'), *flags], check=True)
    result = subprocess.run([str(work/'check')])
    if result.returncode == 77:
        print('SKIP: GTK display unavailable')
    elif result.returncode:
        raise SystemExit(result.returncode)
