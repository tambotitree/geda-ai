"""Drive real GTK chat controls and async pipes with a deterministic helper."""
from pathlib import Path
import os
import shlex
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
BUILD_ROOT = Path(os.environ.get('GEDA_TEST_BUILD_ROOT', ROOT))
sys.path.insert(0, str(ROOT))
from gedai.refactor.oracle import _brace_body
source = (ROOT/'gschem/src/gschem_ai_assist_dialog.c').read_text()
start = source.index('gchar *\nai_assist_schematic_text (')
_, end = _brace_body(source, source.index('{', start))
snapshot = source[start:end]
code = r'''
#include "gschem.h"
#include "gschem_ai_chat_internal.h"
#include "gschem_ai_assist.h"
static PAGE *page;
static const gchar *data_directory;
const char *s_path_sys_data(void) { return data_directory; }
GschemPageView *gschem_toplevel_get_current_page_view(GschemToplevel *w) {
  return (GschemPageView *)w;
}
PAGE *gschem_page_view_get_page(GschemPageView *view) { (void)view; return page; }
gchar *ai_assist_describe_schematic(GschemToplevel *w) {
  (void)w; return g_strdup("fixture page with unsaved changes");
}
void ai_assist_refresh(GschemToplevel *w) { (void)w; }
''' + snapshot + r'''
static void wait_for_reply(GschemAiChat *chat) {
  gint64 deadline=g_get_monotonic_time()+10*G_TIME_SPAN_SECOND;
  while(chat->process != NULL && g_get_monotonic_time()<deadline) {
    g_main_context_iteration(NULL,FALSE); g_usleep(1000);
  }
  g_assert_null(chat->process);
}
static gchar *transcript(GschemAiChat *chat) {
  GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat->transcript));
  GtkTextIter start,end;
  gtk_text_buffer_get_bounds(buffer,&start,&end);
  return gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
}
int main(int argc,char **argv) {
  g_assert_cmpint(argc,==,2); data_directory=argv[1];
  if(!gtk_init_check(NULL,NULL)) return 77;
  scm_init_guile(); libgeda_init();
  TOPLEVEL *top=s_toplevel_new(); top->init_right=10000; top->init_bottom=6000;
  page=s_page_new(top,"unsaved.sch"); top->page_current=page;
  OBJECT *attribute=o_text_new(top,ATTRIBUTE_COLOR,0,0,LOWER_LEFT,0,
                               "refdes=R1",10,VISIBLE,SHOW_VALUE);
  s_page_append(top,page,attribute); page->CHANGED=TRUE;
  GschemToplevel editor={0}; editor.toplevel=top;
  GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GtkWidget *panel=gschem_ai_chat_new(&editor);
  gtk_container_add(GTK_CONTAINER(window),panel);
  gtk_window_set_default_size(GTK_WINDOW(window),800,650);
  gtk_widget_show_all(window);
  GschemAiChat *chat=g_object_get_data(G_OBJECT(panel),"ai-chat");
  const gchar *providers[]={"openai","deepseek","openrouter","ollama"};
  for(guint i=0;i<G_N_ELEMENTS(providers);i++) {
    gtk_entry_set_text(GTK_ENTRY(chat->key),"old-provider-key");
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(chat->provider),providers[i]);
    g_assert_cmpstr(chat->provider_id,==,providers[i]);
    g_assert_cmpstr(gtk_entry_get_text(GTK_ENTRY(chat->key)),==,"");
  }
  g_assert_false(gtk_entry_get_visibility(GTK_ENTRY(chat->key)));
  gtk_entry_set_text(GTK_ENTRY(chat->key),"fixture-only-secret");
  gschem_ai_chat_list_models(NULL,chat); wait_for_reply(chat);
  GtkTreeModel *models=gtk_combo_box_get_model(GTK_COMBO_BOX(chat->models));
  g_assert_cmpint(gtk_tree_model_iter_n_children(models,NULL),==,2);
  gtk_combo_box_set_active(GTK_COMBO_BOX(chat->models),0);
  gtk_entry_set_text(GTK_ENTRY(chat->input),"first question");
  gtk_button_clicked(GTK_BUTTON(chat->send));
  g_assert_nonnull(chat->process); g_assert_false(gtk_widget_get_sensitive(chat->send));
  wait_for_reply(chat);
  gchar *text=transcript(chat); g_assert_nonnull(strstr(text,"refdes=R1")); g_free(text);
  g_assert_nonnull(strstr(chat->history->str,"first question"));
  o_text_set_string(top,attribute,"refdes=R2");
  gtk_entry_set_text(GTK_ENTRY(chat->input),"second question");
  gtk_button_clicked(GTK_BUTTON(chat->send)); wait_for_reply(chat);
  text=transcript(chat); g_assert_nonnull(strstr(text,"refdes=R2"));
  g_assert_null(strstr(text,"fixture-only-secret")); g_free(text);
  gsize history_length=chat->history->len;
  gtk_entry_set_text(GTK_ENTRY(chat->input),"fail");
  gtk_button_clicked(GTK_BUTTON(chat->send)); wait_for_reply(chat);
  g_assert_cmpuint(chat->history->len,==,history_length);
  g_assert_nonnull(strstr(gtk_label_get_text(GTK_LABEL(chat->status)),"API key"));
  gtk_entry_set_text(GTK_ENTRY(chat->input),"slow");
  gtk_button_clicked(GTK_BUTTON(chat->send));
  gtk_button_clicked(GTK_BUTTON(chat->cancel)); wait_for_reply(chat);
  g_assert_cmpuint(chat->history->len,==,history_length);
  g_assert_true(gtk_widget_get_sensitive(chat->send));
  gtk_entry_set_text(GTK_ENTRY(chat->input),"slow");
  gtk_button_clicked(GTK_BUTTON(chat->send));
  /* Retain the panel exactly as an outstanding async callback does. */
  g_object_ref(panel); gtk_widget_destroy(window); wait_for_reply(chat);
  g_assert_true(chat->closed); g_assert_null(chat->window); g_object_unref(panel);
  s_toplevel_delete(top);
  g_print("PASS: providers, model picker, live unsaved context, history, errors, cancel, close during request\n");
  return 0;
}
'''
helper = '''import json,sys,time
r=json.load(sys.stdin)
assert r['api_key']=='fixture-only-secret'
if r['operation']=='models':
    print('fixture-model\\nother-model',end='')
elif r['question']=='fail':
    print('No API key. Select a .env or enter an API key.',file=sys.stderr)
    sys.exit(1)
elif r['question']=='slow':
    time.sleep(4)
    print('slow reply',end='')
else:
    assert 'fixture' in r['model']
    if r['question']=='second question':
        assert len(r['history'])==2
        assert 'refdes=R1' not in r['schematic']
    print('Snapshot received: '+r['schematic'],end='')
'''
flags = shlex.split(subprocess.check_output(
    ['pkg-config','--cflags','--libs','gtk+-3.0','guile-3.0','pangocairo'], text=True))
with tempfile.TemporaryDirectory(prefix='geda-ai-ui-') as directory:
    work=Path(directory)
    (work/'check.c').write_text(code)
    (work/'ai').mkdir()
    (work/'ai/chat_backend.py').write_text(helper)
    subprocess.run(['cc','-std=c11','-g','-O0','-Wall','-Wextra','-Werror',
                    '-Wpedantic','-Wstrict-prototypes','-Wmissing-prototypes','-Wformat=2',
                    '-DGSCHEM_AI_PYTHON="'+sys.executable+'"',
                    '-I'+str(BUILD_ROOT),'-I'+str(ROOT/'gschem/include'),
                    '-I'+str(ROOT/'gschem/src'),'-I'+str(ROOT/'libgeda/include'),
                    str(work/'check.c'),
                    *(str(ROOT/'gschem/src'/p) for p in ['gschem_ai_chat.c',
                         'gschem_ai_chat_json.c','gschem_ai_chat_settings.c']),
                    '-L'+str(BUILD_ROOT/'libgeda/src/.libs'),'-lgeda',
                    '-o',str(work/'check'),*flags],check=True)
    environment=dict(os.environ)
    environment['XDG_CONFIG_HOME']=str(work/'config')
    environment['DYLD_LIBRARY_PATH']=str(BUILD_ROOT/'libgeda/src/.libs')
    environment['LD_LIBRARY_PATH']=str(BUILD_ROOT/'libgeda/src/.libs')
    result=subprocess.run([str(work/'check'),str(work)],env=environment,cwd=work)
    preferences=work/'config/geda-ai/ai-assist.ini'
    if preferences.exists():
        assert 'fixture-only-secret' not in preferences.read_text()
    if result.returncode==77:
        print('SKIP: GTK display is not available')
    elif result.returncode:
        raise SystemExit(result.returncode)
