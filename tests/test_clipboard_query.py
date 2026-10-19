"""Exercise the production async clipboard query with controlled GTK replies."""
from pathlib import Path
import shlex
import subprocess
import tempfile

root = Path(__file__).resolve().parents[1]
source = (root / 'gschem/src/x_clipboard.c').read_text()
start = source.index('typedef struct {\n  GschemToplevel *toplevel;')
end = source.index('/* \\brief Set the contents of the system clipboard.', start)
production = source[start:end]
harness = r'''
#include <glib-object.h>
#include <string.h>
typedef GObject GtkWidget;
typedef guint GdkAtom;
typedef struct { int unused; } GtkClipboard;
typedef struct { gint length; GdkAtom type; } GtkSelectionData;
typedef struct { GtkWidget *main_window; } GschemToplevel;
typedef struct { int unused; } GschemAction;
static GschemAction paste_action;
static GschemAction *action_clipboard_paste = &paste_action;
#define GDK_SELECTION_CLIPBOARD 1
#define MIME_TYPE_SCHEMATIC "application/x-geda-schematic"
static GtkClipboard clipboard;
static guint updates;
static gboolean last_usable;
static void (*pending_reply)(GtkClipboard *, GtkSelectionData *, gpointer);
static gpointer pending_data;
static GtkClipboard *gtk_clipboard_get (GdkAtom atom) {
  g_assert_cmpuint(atom, ==, GDK_SELECTION_CLIPBOARD);
  return &clipboard;
}
static GdkAtom gdk_atom_intern_static_string (const gchar *name) {
  return g_quark_from_static_string(name);
}
static gint gtk_selection_data_get_length (GtkSelectionData *data) {
  return data->length;
}
static GdkAtom gtk_selection_data_get_data_type (GtkSelectionData *data) {
  return data->type;
}
static void gschem_action_set_sensitive (GschemAction *action, gboolean usable,
                                        GschemToplevel *toplevel) {
  g_assert_true(action == action_clipboard_paste);
  g_assert_true(G_IS_OBJECT(toplevel->main_window));
  updates++;
  last_usable = usable;
}
static void gtk_clipboard_request_contents (GtkClipboard *clip, GdkAtom type,
    void (*reply)(GtkClipboard *, GtkSelectionData *, gpointer), gpointer data) {
  g_assert_true(clip == &clipboard);
  g_assert_cmpuint(type, ==, g_quark_from_static_string(MIME_TYPE_SCHEMATIC));
  pending_reply = reply;
  pending_data = data;
}
void x_clipboard_update_menus (GschemToplevel *toplevel);
''' + production + r'''
static void complete_reply (gint length, const gchar *type) {
  GtkSelectionData selection = {length, g_quark_from_static_string(type)};
  pending_reply(&clipboard, &selection, pending_data);
  pending_reply = NULL;
  pending_data = NULL;
}
int main (void) {
  guint destroy = g_signal_new("destroy", G_TYPE_OBJECT, G_SIGNAL_RUN_LAST,
      0, NULL, NULL, NULL, G_TYPE_NONE, 0);
  GschemToplevel *toplevel = g_new0(GschemToplevel, 1);
  GtkWidget *window = g_object_new(G_TYPE_OBJECT, NULL);
  toplevel->main_window = window;
  x_clipboard_update_menus(toplevel);
  complete_reply(-1, MIME_TYPE_SCHEMATIC);
  g_assert_false(last_usable);
  x_clipboard_update_menus(toplevel);
  complete_reply(32, MIME_TYPE_SCHEMATIC);
  g_assert_true(last_usable);
  x_clipboard_update_menus(toplevel);
  complete_reply(32, "text/plain");
  g_assert_false(last_usable);
  x_clipboard_update_menus(toplevel);
  complete_reply(0, MIME_TYPE_SCHEMATIC);
  g_assert_false(last_usable);
  g_assert_cmpuint(updates, ==, 4);
  x_clipboard_update_menus(toplevel);
  g_signal_emit(window, destroy, 0);
  g_free(toplevel);
  complete_reply(32, MIME_TYPE_SCHEMATIC);
  g_assert_cmpuint(updates, ==, 4);
  g_object_unref(window);
  g_print("PASS: missing, schematic, unrelated, empty, and late clipboard replies\n");
  return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='geda-clipboard-query-') as directory:
    work = Path(directory)
    (work / 'check.c').write_text(harness)
    flags = shlex.split(subprocess.check_output(
        ['pkg-config', '--cflags', '--libs', 'gobject-2.0'], text=True))
    subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-Wpedantic',
                    '-Wstrict-prototypes', '-Wmissing-prototypes', '-Wformat=2',
                    str(work / 'check.c'), '-o', str(work / 'check'), *flags], check=True)
    subprocess.run([str(work / 'check')], check=True)
