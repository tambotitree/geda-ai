"""Ask DeepSeek for a design that removes Guile from gschem's menu builder.

This is a design question, not a transformation, so there is no oracle to
diff against. The reply is therefore treated as a proposal: it is printed
for a human to read, never written into the tree by this script. Any claim
it makes about the codebase is meant to be checked against the source.

Usage:
    python3 -m gedai.refactor.de_menu
"""

import json
import sys

from . import config, deepseek

SYSTEM = """You are a senior C/GTK3 developer working on gschem, part of gEDA.
You are given a concrete design problem with hard constraints. Propose a
design that satisfies every constraint, or say clearly which constraint
cannot be satisfied and why. Do not hand-wave: name the functions, structs
and files you would change, and give the key data structure in C.
Prefer the smallest change that meets the goal. If the goal is impossible as
stated, say so plainly and propose the nearest achievable goal instead."""

CONTEXT = """Goal: remove Guile (libguile) from the code path that builds gschem's
menu bar, context menu and toolbar, so that opening a window does not walk
Scheme data structures in order to create GTK widgets.

Current implementation (gschem/src/x_menus.c, 284 lines):

  static GtkWidget *build_menu (SCM s_menu, gboolean is_main_menu,
                                GschemToplevel *w_current)
  {
    GtkWidget *menu = gtk_menu_new ();
    GtkWidget *placeholder = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), placeholder);
    gtk_widget_show(placeholder);

    for (SCM l0 = s_menu; scm_is_pair (l0); l0 = scm_cdr (l0)) {
      SCM s_section = scm_car (l0);
      SCM_ASSERT (scm_is_true (scm_list_p (s_section)), ...);
      for (SCM l1 = s_section; scm_is_pair (l1); l1 = scm_cdr (l1)) {
        SCM s_item = scm_car (l1);
        SCM_ASSERT (scm_is_action (s_item), ...);
        scm_permanent_object (s_item);
        GschemAction *action = scm_to_action (s_item);
        GtkWidget *menu_item =
          gschem_action_create_menu_item (action, is_main_menu, w_current);
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
        gtk_widget_show (menu_item);
      }
      if (scm_is_pair (scm_cdr (l0))) {
        GtkWidget *menu_item = gtk_menu_item_new ();   /* separator */
        gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
        gtk_widget_show (menu_item);
      }
    }
    return menu;
  }

  x_menus_create_main_menu()   reads Scheme variable `menubar`, iterates
                               ((title . sections) ...), and for each menu
                               builds a GtkMenuItem with the title.
  x_menus_create_main_popup()  reads Scheme variable `context-menu` and calls
                               build_menu() on it.
  x_menus_create_toolbar()     reads Scheme variable `toolbar`, walks it the
                               same way, and calls
                               gschem_action_create_tool_button() instead.

The three variables are defined in gschem/lib/system-gschemrc.scm and are
loaded before the first window is created.

The C registry that already exists: gschem/src/actions.c holds
`static const GschemActionSpec action_specs[]` with 135 rows of
{id, icon_name, name, label, menu_label, tooltip, type, action, activate},
plus one global `GschemAction *action_<id>` per action. Registering them
creates, for every action, a Scheme binding named "&<id>".

HARD CONSTRAINTS:

1. The default Help menu contains four actions defined in Scheme with
   define-action-public in gschem/scheme/gschem/builtins.scm, whose bodies
   are Scheme (they call show-wiki): &help-manual, &help-guide, &help-faq,
   &help-wiki. They are NOT in action_specs and cannot be moved there
   without changing what they are. The stock Help menu must still contain
   all four.

2. gschem/lib/system-gschemrc.scm documents that users may redefine the menu
   structure from their own .gschemrc, e.g.
       (set! tools-menu (append! tools-menu `((,&do-something))))
       (assoc-set! menubar (_ "_Help") my-help-menu)
       (set! context-menu `((,&edit-select)))
       (set! toolbar ...)
   using arbitrary user-defined actions created with define-action.
   These overrides must continue to work after the change.

3. The default menu data is currently 112 action references across 11 lists
   (file-menu, edit-menu, add-menu, object-menu, view-menu, pages-menu,
   tools-menu, options-menu, help-menu, context-menu, toolbar) in
   sections. 108 of those 112 are in action_specs; the other 4 are the
   Tier B actions from constraint 1.

4. No new runtime dependencies. No dlopen. No JSON at runtime.

5. Menus are built once per window in gschem/src/x_window.c via
   x_menus_create_submenus(), x_menus_create_main_menu(),
   x_menus_create_toolbar() and x_menus_create_main_popup().

QUESTION: Design the change. Address specifically:
  a) What happens to constraints 1 and 2? If they conflict with the goal,
     say so and propose the nearest achievable goal.
  b) The concrete C data structure to represent a menu tree, and how a
     Scheme override to one of the 11 lists is detected and applied.
  c) How the default data is kept in sync with system-gschemrc.scm, or why
     it no longer needs to be.
  d) Which files and functions change, and roughly how each changes.
  e) What could regress, and how we would detect that.

Answer as a JSON object with keys: "feasible" (yes/no/partly), "verdict"
(one paragraph), "blocked_constraints" (list), "design" (object with
"data_structure" (C code), "override_detection" (paragraph),
"sync_strategy" (paragraph)), "files_changed" (list of {file, change}),
"risks" (list of {risk, detection}), and "nearest_achievable_goal"
(paragraph)."""


def main():
    try:
        config.api_key()
    except config.ConfigError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2

    messages = [
        {"role": "system", "content": SYSTEM},
        {"role": "user", "content": CONTEXT},
    ]
    try:
        reply, usage = deepseek.complete_json(messages, max_tokens=4000,
                                              timeout=180)
    except deepseek.DeepSeekError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    print(f"tokens: {usage.get('total_tokens', '?')}", file=sys.stderr)
    print(json.dumps(reply, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(main())
