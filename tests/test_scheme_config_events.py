"""Exercise production Scheme configuration connections with Guile and GObject."""
from pathlib import Path
import subprocess,shlex,tempfile,atexit,shutil
work = Path(tempfile.mkdtemp(prefix="geda-config-events-"))
atexit.register(shutil.rmtree, work)
ROOT = Path(__file__).resolve().parents[1]
s=(ROOT / 'libgeda/src/scheme_config.c').read_text()
block=s[s.index('typedef struct {\n  SCM procedure;'):s.index('/*! \\brief Add a configuration change event handler.')]
def body(name):
 start=s.index('{',s.index('SCM_DEFINE ('+name)); depth=1; end=start+1
 while depth:
  if s[end]=='{': depth+=1
  elif s[end]=='}': depth-=1
  end+=1
 return s[start:end]
code='''#include <libguile.h>
#include <glib-object.h>
#include <assert.h>
typedef GObject EdaConfig;
static EdaConfig *edascm_to_config(SCM x) { return scm_to_pointer(x); }
static SCM edascm_from_config(EdaConfig *x) { return scm_from_pointer(x, NULL); }
static SCM g_scm_eval_protected(SCM x, SCM env) { return scm_eval(x, env); }
#define EDASCM_CONFIGP(x) SCM_POINTER_P(x)
static const char *s_add_config_event_x = "test-config-event";
'''+block+'\nstatic SCM add_config_event_x(SCM cfg_s, SCM proc_s)\n'+body('add_config_event_x')+'\nstatic SCM remove_config_event_x(SCM cfg_s, SCM proc_s)\n'+body('remove_config_event_x')+'''
int main(void) {
 scm_init_guile();
 guint signal = g_signal_new("config-changed", G_TYPE_OBJECT, G_SIGNAL_RUN_LAST,
 0, NULL, NULL, NULL, G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_STRING);
 GObject *cfg = g_object_new(G_TYPE_OBJECT, NULL);
 SCM wrapped = edascm_from_config(cfg);
 SCM proc = scm_c_eval_string("(begin (define calls 0) (lambda (cfg group key) (set! calls (+ calls 1))))");
 add_config_event_x(wrapped, proc);
 add_config_event_x(wrapped, proc);
 assert(g_hash_table_size(scheme_config_events(cfg)) == 1);
 g_signal_emit(cfg, signal, 0, "group", "key");
 assert(scm_to_int(scm_c_eval_string("calls")) == 1);
 remove_config_event_x(wrapped, proc);
 remove_config_event_x(wrapped, proc);
 assert(g_hash_table_size(scheme_config_events(cfg)) == 0);
 g_signal_emit(cfg, signal, 0, "group", "key");
 assert(scm_to_int(scm_c_eval_string("calls")) == 1);
 add_config_event_x(wrapped, proc);
 g_signal_emit(cfg, signal, 0, "group", "key");
 assert(scm_to_int(scm_c_eval_string("calls")) == 2);
 g_object_unref(cfg);
 scm_gc();
 return 0;
}
'''
(work / 'check.c').write_text(code)
flags=shlex.split(subprocess.check_output(['pkg-config','--cflags','--libs','guile-3.0','gobject-2.0'],text=True))
subprocess.run(['cc','-O2','-Wall','-Wextra','-Werror','-Wpedantic','-Wstrict-prototypes','-Wmissing-prototypes','-Wformat=2',str(work / 'check.c'),'-o',str(work / 'check'),*flags],check=True)
subprocess.run([str(work / 'check')],check=True)
print('Configuration event lifecycle checks passed.')
