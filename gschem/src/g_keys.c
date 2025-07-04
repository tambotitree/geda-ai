/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2020 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <config.h>

#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "gschem.h"

#include <gdk/gdkkeysyms-compat.h>


/*! Contains the smob tag for key smobs */
static scm_t_bits g_key_smob_tag;
#define G_SCM_IS_KEY(x) SCM_SMOB_PREDICATE (g_key_smob_tag, (x))

/*! Type for keybindings. Used internally by gschem key smobs. */
typedef struct {
  guint keyval;
  GdkModifierType modifiers;
  gchar *str; /* UTF-8. Free with g_free(). */
  gchar *disp_str; /* UTF-8. Free with g_free(). */
} GschemKey;

/*! \brief Test if a key is valid.
 * \par Function Description
 * Test if the key combination defined by \a keyval and \a modifiers
 * is valid for key binding.  This is a less restrictive version of
 * gtk_accelerator_valid() from GTK 2.
 *
 * \param keyval     The key that was pressed.
 * \param modifiers  The active modifiers when the key was pressed.
 *
 * \return TRUE if the key combination is valid for keybinding.
 */
static gboolean
g_key_is_valid (guint keyval, GdkModifierType modifiers)
{
  static const guint invalid_keyvals[] = {
    GDK_KEY_Shift_L, GDK_KEY_Shift_R, GDK_KEY_Shift_Lock, GDK_KEY_Caps_Lock, GDK_KEY_ISO_Lock,
    GDK_KEY_Control_L, GDK_KEY_Control_R, GDK_KEY_Meta_L, GDK_KEY_Meta_R,
    GDK_KEY_Alt_L, GDK_KEY_Alt_R, GDK_KEY_Super_L, GDK_KEY_Super_R, GDK_KEY_Hyper_L, GDK_KEY_Hyper_R,
    GDK_KEY_ISO_Level3_Shift, GDK_KEY_ISO_Next_Group, GDK_KEY_ISO_Prev_Group,
    GDK_KEY_ISO_First_Group, GDK_KEY_ISO_Last_Group,
    GDK_KEY_Mode_switch, GDK_KEY_Num_Lock, GDK_KEY_Multi_key,
    GDK_KEY_Scroll_Lock, GDK_KEY_Sys_Req,
    GDK_KEY_Tab, GDK_KEY_ISO_Left_Tab, GDK_KEY_KP_Tab,
    GDK_KEY_First_Virtual_Screen, GDK_KEY_Prev_Virtual_Screen,
    GDK_KEY_Next_Virtual_Screen, GDK_KEY_Last_Virtual_Screen,
    GDK_KEY_Terminate_Server, GDK_KEY_AudibleBell_Enable,
    0
  };
  const guint *val;

  /* Exclude a bunch of control chars */
  if (keyval <= 0xFF) return keyval >= 0x20;

  /* Exclude special & modifier keys */
  val = invalid_keyvals;
  while (*val) {
    if (keyval == *val++) return FALSE;
  }

  return TRUE;
}

/*! \brief Create a new bindable key object.
 * \par Function Description
 * Create and return a new gschem key object from a \a keyval and a
 * set of \a modifiers.  If the key combination is invalid, return
 * SCM_BOOL_F.
 *
 * \param keyval     the pressed key.
 * \param modifiers  the active modifiers for the key.
 *
 * \return a new bindable key object, or SCM_BOOL_F.
 */
static SCM
g_make_key (guint keyval, GdkModifierType modifiers)
{
  SCM result = SCM_BOOL_F;
  if (g_key_is_valid (keyval, modifiers)) {
    GschemKey *k = g_new0 (GschemKey, 1);
    k->keyval = keyval;
    k->modifiers = modifiers & GDK_MODIFIER_MASK;
    SCM_NEWSMOB (result, g_key_smob_tag, k);
  }
  return result;
}

/*! \brief Test if a Scheme value is a bindable key object.
 * \par Function Description
 * Returns SCM_BOOL_T if \a key_s is a gschem key object.  Otherwise,
 * returns SCM_BOOL_F.
 *
 * \note Scheme API: Implements the %key? procedure in the
 * (gschem core keymap) module.
 *
 * \param key_s          value to test
 * \return SCM_BOOL_T iff value is a key, otherwise SCM_BOOL_F.
 */
SCM_DEFINE (g_keyp, "%key?", 1, 0, 0, (SCM key_s),
            "Test if value is a gschem key.")
{
  if (G_SCM_IS_KEY (key_s)) {
    return SCM_BOOL_T;
  } else {
    return SCM_BOOL_F;
  }
}

/*! \brief Create a bindable key object from a string.
 * \par Function Description
 * Parse the string key description \a str_s to create and return a
 * new gschem key object.  If \a str_s contains syntax errors, or does
 * not represent a valid bindable key combination, returns SCM_BOOL_F.
 *
 * \note Scheme API: Implements the %string-key procedure in the
 * (gschem core keymap) module.
 *
 * \param str_s  string to parse.
 * \return a new gschem key object, or SCM_BOOL_F.
 */
SCM_DEFINE (g_string_to_key, "%string->key", 1, 0, 0, (SCM str_s),
            "Create a gschem key by parsing a string.")
{
  SCM_ASSERT (scm_is_string (str_s), str_s, SCM_ARG1, s_g_string_to_key);

  guint keyval;
  GdkModifierType modifiers;
  char *str = scm_to_utf8_string (str_s);
  gtk_accelerator_parse (str, &keyval, &modifiers);
  if ((keyval == 0) && (modifiers == 0)) return SCM_BOOL_F;
  return g_make_key (keyval, modifiers);
}

/*! \brief Convert a bindable key object to a string.
 * \par Function Description
 * Returns a string representation of the gschem key object \a key_s,
 * in a format suitable for parsing with %string->key.
 *
 * \note Scheme API: Implements the %key->string procedure in the
 * (gschem core keymap) module.
 *
 * \param key_s  Bindable key object to convert to string.
 * \return a string representation of the key combination.
 */
SCM_DEFINE (g_key_to_string, "%key->string", 1, 0, 0, (SCM key_s),
            "Create a string from a gschem key.")
{
  SCM_ASSERT (G_SCM_IS_KEY (key_s), key_s, SCM_ARG1, s_g_key_to_string);

  GschemKey *key = (GschemKey *) SCM_SMOB_DATA (key_s);
  if (key->str != NULL) return scm_from_utf8_string (key->str);

  key->str = gtk_accelerator_name (key->keyval, key->modifiers);
  return scm_from_utf8_string (key->str);
}

/*! \brief Convert a bindable key object to a displayable string.
 * \par Function Description
 * Returns a string representation of the gschem key object \a key_s,
 * in a format suitable for display to the user (e.g. as accelerator
 * text in a menu).
 *
 * \note Scheme API: Implements the %key->display-string procedure in
 * the (gschem core keymap) module.
 *
 * \param key_s  Bindable key object to convert to string.
 * \return a string representation of the key combination.
 */
SCM_DEFINE (g_key_to_display_string, "%key->display-string", 1, 0, 0,
            (SCM key_s), "Create a display string from a gschem key.")
{
  SCM_ASSERT (G_SCM_IS_KEY (key_s), key_s, SCM_ARG1,
              s_g_key_to_display_string);

  GschemKey *key = (GschemKey *) SCM_SMOB_DATA (key_s);
  if (key->disp_str != NULL) return scm_from_utf8_string (key->disp_str);

  key->disp_str = gtk_accelerator_get_label (key->keyval, key->modifiers);
  return scm_from_utf8_string (key->disp_str);
}

/*! \brief Print a representation of a key smob
 * \par Function Description
 * Outputs a string representing the \a smob to a Scheme output \a
 * port.  The format used is "#<gschem-key \"Ctrl+A\">".
 *
 * Used internally to Guile.
 */
static int
g_key_print (SCM smob, SCM port, scm_print_state *pstate)
{
  scm_puts ("#<gschem-key ", port);
  scm_write (g_key_to_display_string (smob), port);
  scm_puts (">", port);

  /* Non-zero means success */
  return 1;
}

/* \brief Test if two key combinations are equivalent.
 * \par Function Description
 * Tests if the two gschem key objects \a a and \a b represent the
 * same key event.
 *
 * Used internally to Guile.
 */
static SCM
g_key_equalp (SCM a, SCM b)
{
  GschemKey *akey = (GschemKey *) SCM_SMOB_DATA (a);
  GschemKey *bkey = (GschemKey *) SCM_SMOB_DATA (b);
  if (akey->keyval != bkey->keyval) return SCM_BOOL_F;
  if (akey->modifiers != bkey->modifiers) return SCM_BOOL_F;
  return SCM_BOOL_T;
}

/* \brief Destroy a bindable key object
 * \par Function Description
 * Destroys the contents of a gschem key object on garbage collection.
 *
 * Used internally to Guile.
 */
static size_t
g_key_free (SCM key) {
  GschemKey *k = (GschemKey *) SCM_SMOB_DATA (key);
  g_free (k->str);
  g_free (k->disp_str);
  g_free (k);
  return 0;
}

SCM_SYMBOL (reset_keys_sym, "reset-keys");
SCM_SYMBOL (press_key_sym, "press-key");
SCM_SYMBOL (prefix_sym, "prefix");

/*! \brief Clear the current key accelerator string.
 * \par Function Description
 * This function clears the current keyboard accelerator string in
 * the status bar of the relevant toplevel.  Called some time after a
 * keystroke is pressed.  If the current key sequence was a prefix,
 * let it persist.
 *
 * \param [in] data a pointer to the GschemToplevel to update.
 * \return FALSE (this is a one-shot timer).
 */
static gboolean clear_keyaccel_string(gpointer data)
{
  GschemToplevel *w_current = data;

  /* If the window context has disappeared, do nothing. */
  if (g_list_find(global_window_list, w_current) == NULL) {
    return FALSE;
  }

  g_free(w_current->keyaccel_string);
  w_current->keyaccel_string = NULL;
  w_current->keyaccel_string_source_id = 0;
  i_show_state(w_current, NULL);
  return FALSE;
}

/*! \brief Reset the current key sequence.
 * \par Function Description
 * If any prefix keys are stored in the current key sequence, clears
 * them.
 *
 * \param w_current  The active #GschemToplevel context.
 */
void
g_keys_reset (GschemToplevel *w_current)
{
  SCM s_expr = scm_list_1 (reset_keys_sym);

  /* Reset the status bar */
  g_free (w_current->keyaccel_string);
  w_current->keyaccel_string = NULL;
  i_show_state(w_current, NULL);

  /* Reset the Scheme keybinding state */
  scm_dynwind_begin (0);
  g_dynwind_window (w_current);
  g_scm_eval_protected (s_expr, scm_interaction_environment ());
  scm_dynwind_end ();
}

/*! \brief Evaluate a user keystroke.
 * \par Function Description
 * Evaluates the key combination specified by \a event using the
 * current keymap.  Updates the gschem status bar with the current key
 * sequence.
 *
 * \param w_current  The active #GschemToplevel context.
 * \param event      A GdkEventKey structure.
 *
 * \return 1 if a binding was found for the keystroke, 0 otherwise.
 */
int
g_keys_execute(GschemToplevel *w_current, GdkEventKey *event)
{
  SCM s_retval, s_key, s_expr;
  guint key, mods, upper, lower, caps;
  GdkDisplay *display;
  GdkKeymap *keymap;
  GdkModifierType consumed_modifiers;

  g_return_val_if_fail (w_current != NULL, 0);
  g_return_val_if_fail (event != NULL, 0);

  display = gtk_widget_get_display (w_current->main_window);
  keymap = gdk_keymap_get_for_display (display);

  /* Figure out what modifiers went into determining the key symbol */
  gdk_keymap_translate_keyboard_state (keymap,
                                       event->hardware_keycode,
                                       event->state, event->group,
                                       NULL, NULL, NULL, &consumed_modifiers);

  key = event->keyval;
  gdk_keyval_convert_case (event->keyval, &lower, &upper);
  mods = (event->state & gtk_accelerator_get_default_mod_mask ()
                & ~consumed_modifiers);

  /* Handle Caps Lock. The idea is to obtain the same keybindings
   * whether Caps Lock is enabled or not. */
  if (upper != lower) {
    caps = gdk_keymap_get_caps_lock_state (keymap);
    if ((caps && (key == lower)) || (!caps && (key == upper))) {
      mods |= GDK_SHIFT_MASK;
    }
  }

  /* Always process key as lower case */
  key = lower;

  /* Validate the key -- there are some keystrokes we mask out. */
  if (!g_key_is_valid (key, mods)) {
    return FALSE;
  }

  /* Create Scheme key value */
  s_key = g_make_key (key, mods);

  /* Update key hint string for status bar. */
  gchar *keystr = gtk_accelerator_get_label (key, mods);

  /* If no current hint string, or the hint string is going to be
   * cleared anyway, use key string directly */
  if ((w_current->keyaccel_string == NULL) ||
      w_current->keyaccel_string_source_id) {
    g_free (w_current->keyaccel_string);
    w_current->keyaccel_string = keystr;

  } else {
    gchar *p = w_current->keyaccel_string;
    w_current->keyaccel_string = g_strconcat (p, " ", keystr, NULL);
    g_free (p);
    g_free (keystr);
  }

  /* Update status bar */
  i_show_state(w_current, NULL);

  /* Build and evaluate Scheme expression. */
  scm_dynwind_begin (0);
  g_dynwind_window (w_current);
  s_expr = scm_list_2 (press_key_sym, s_key);
  s_retval = g_scm_eval_protected (s_expr, scm_interaction_environment ());
  scm_dynwind_end ();

  /* only start timer if window wasn't destroyed during the action */
  if (g_list_find (global_window_list, w_current) != NULL) {
    /* If the keystroke was not part of a prefix, start a timer to clear
     * the status bar display. */
    if (w_current->keyaccel_string_source_id) {
      /* Cancel any existing timers that haven't fired yet. */
      GSource *timer =
        g_main_context_find_source_by_id (NULL,
                                          w_current->keyaccel_string_source_id);
      g_source_destroy (timer);
      w_current->keyaccel_string_source_id = 0;
    }
    if (!scm_is_eq (s_retval, prefix_sym)) {
      w_current->keyaccel_string_source_id =
        g_timeout_add(400, clear_keyaccel_string, w_current);
    }
  }

  return !scm_is_false (s_retval);
}

/*! \brief Create the (gschem core keymap) Scheme module
 * \par Function Description
 * Defines procedures in the (gschem core keymap) module.  The module
 * can be accessed using (use-modules (gschem core keymap)).
 */
static void
init_module_gschem_core_keymap ()
{
  /* Register the functions */
  #include "g_keys.x"

  /* Add them to the module's public definitions */
  scm_c_export (s_g_keyp, s_g_string_to_key, s_g_key_to_string,
                s_g_key_to_display_string, NULL);
}

/*! \brief Initialise the key combination procedures
 * \par Function Description
 * Registers some Scheme procedures for working with key combinations.
 * Should only be called by main_prog().
 */
void
g_init_keys ()
{
  /* Register key smob type */
  g_key_smob_tag = scm_make_smob_type ("gschem-key", 0);
  scm_set_smob_print (g_key_smob_tag, g_key_print);
  scm_set_smob_equalp (g_key_smob_tag, g_key_equalp);
  scm_set_smob_free (g_key_smob_tag, g_key_free);

  scm_c_define_module ("gschem core keymap",
                       init_module_gschem_core_keymap,
                       NULL);
}

