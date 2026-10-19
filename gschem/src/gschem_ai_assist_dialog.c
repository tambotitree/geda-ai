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

/*! \file gschem_ai_assist_dialog.c
 *
 * \brief "AI Assist" dialog.
 *
 * gEDA schematics are plain text, which is what makes them tractable for a
 * language model.  This dialog is the counterpart in the GUI: it gathers the
 * schematic as text and reports whether the design is ready to be netlisted
 * for SPICE, then hands the text to whatever assistant the user has.
 *
 * The chat tab sends the live page to a configurable model provider. The
 * schematic shown is rendered from the edit buffer, so unsaved work is
 *       included and opening the dialog never writes to disk.
 */

#include <config.h>

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"
#include "gschem_ai_assist.h"
#include "gschem_ai_chat.h"

/*! \brief Response id for the dialog's Refresh button.
 *
 *  Custom responses must be positive and must not collide with GTK's own,
 *  which are all negative.
 */
#define AI_ASSIST_RESPONSE_REFRESH 1

/*! \brief Look up one of the dialog's two text views.
 *
 *  \param [in] dialog The dialog widget.
 *  \param [in] key    "textview-schematic" or "textview-report".
 *  \return The text view, or NULL if the dialog has not been built yet.
 */
static GtkWidget *
ai_assist_get_view (GtkWidget *dialog, const gchar *key)
{
  return g_object_get_data (G_OBJECT (dialog), key);
}

/*! \brief Replace the whole contents of one of the dialog's text views.
 *
 *  \param [in] dialog The dialog widget.
 *  \param [in] key    Which view to write; see ai_assist_get_view().
 *  \param [in] text   The text to show; NULL is treated as empty.
 */
static void
ai_assist_set_text (GtkWidget *dialog, const gchar *key, const gchar *text)
{
  GtkWidget *view = ai_assist_get_view (dialog, key);

  if (view == NULL) {
    return;
  }

  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)),
                            text != NULL ? text : "", -1);
}

/*! \brief Render the current page as gEDA text.
 *
 *  This is the single place that turns the live edit buffer into text, so
 *  that the dialog and the headless prompt writer cannot drift apart.
 *
 *  It renders the edit buffer rather than the file on disk: the user may have
 *  unsaved changes, and this avoids writing anything.
 *
 *  \param [in] w_current The GschemToplevel whose schematic is rendered.
 *  \return A newly allocated string; free with g_free().
 */
gchar *
ai_assist_schematic_text (GschemToplevel *w_current)
{
  GschemPageView *page_view =
    gschem_toplevel_get_current_page_view (w_current);
  PAGE *page = gschem_page_view_get_page (page_view);

  if (page == NULL) {
    return NULL;
  }

  return o_save_buffer (s_page_objects (page));
}

/*! \brief Fill the text view with the current page as gEDA text.
 *
 *  \param [in] dialog    The dialog widget.
 *  \param [in] w_current The GschemToplevel whose schematic is rendered.
 */
static void
ai_assist_show_schematic (GtkWidget *dialog, GschemToplevel *w_current)
{
  gchar *text = ai_assist_schematic_text (w_current);

  ai_assist_set_text (dialog, "textview-schematic",
                      text != NULL
                        ? text
                        : _("No schematic is currently open."));
  g_free (text);
}

/*! \brief Describe the current schematic and its SPICE readiness.
 *
 *  The report is descriptive rather than prescriptive: it states what is in
 *  the design and what a SPICE netlist would need, and leaves the decision
 *  to the user.
 *
 *  \param [in] w_current The GschemToplevel whose schematic is described.
 *  \return A newly allocated string; free with g_free().
 */
gchar *
ai_assist_describe_schematic (GschemToplevel *w_current)
{
  GschemPageView *page_view =
    gschem_toplevel_get_current_page_view (w_current);
  PAGE *page = gschem_page_view_get_page (page_view);
  GString *out = g_string_new (NULL);
  const GList *iter;
  gint objects = 0;
  gint components = 0;
  gint with_model = 0;
  gint with_device = 0;

  g_string_append (out, _("Schematic"));
  g_string_append (out, "\n");
  g_string_append_printf (out, "  %-16s %s\n", _("Page:"),
                          (page != NULL && page->page_filename != NULL)
                            ? page->page_filename
                            : _("(untitled)"));
  g_string_append_printf (out, "  %-16s %s\n", _("Modified:"),
                          (page != NULL && page->CHANGED)
                            ? _("yes (text below includes unsaved changes)")
                            : _("no"));

  if (page != NULL) {
    for (iter = s_page_objects (page); iter != NULL; iter = g_list_next (iter)) {
      OBJECT *object = (OBJECT *) iter->data;
      gchar *value;

      objects++;
      if (object->type != OBJ_COMPLEX) {
        continue;
      }

      components++;

      value = o_attrib_search_object_attribs_by_name (object, "model-name", 0);
      if (value != NULL) {
        with_model++;
        g_free (value);
      }

      value = o_attrib_search_object_attribs_by_name (object, "device", 0);
      if (value != NULL) {
        with_device++;
        g_free (value);
      }
    }
  }

  g_string_append_printf (out, "  %-16s %d\n", _("Objects:"), objects);
  g_string_append_printf (out, "  %-16s %d\n", _("Components:"), components);

  g_string_append (out, "\n");
  g_string_append (out, _("SPICE readiness"));
  g_string_append (out, "\n");
  g_string_append_printf (out, "  %-16s %d\n", _("model-name:"), with_model);
  g_string_append_printf (out, "  %-16s %d\n", _("device:"), with_device);

  if (components == 0) {
    g_string_append_printf (out, "  %s\n",
                            _("The page has no components to simulate."));
  } else if (with_model == 0) {
    g_string_append_printf (out, "  %s\n",
      _("No component carries a model-name attribute.  Semiconductors need\n"
        "  one for SPICE; passives can be netlisted without it."));
  } else {
    g_string_append_printf (out, "  %s\n",
      _("Semiconductor models are attached; the design can be netlisted."));
  }

  g_string_append (out, "\n");
  g_string_append (out, _("Netlist"));
  g_string_append (out, "\n");
  g_string_append_printf (out, "  %s\n",
    _("gschem does not extract netlists itself.  Save the schematic, then:"));
  g_string_append (out, "\n");
  g_string_append (out, "    gnetlist -g spice-sdb -o design.cir design.sch\n");

  return g_string_free (out, FALSE);
}

/*! \brief Bring the dialog's contents up to date with the current page.
 *
 *  Called when the dialog is opened and whenever the user presses Refresh,
 *  so that the text always describes the page named in its own report.
 *
 *  \param [in] w_current The GschemToplevel the dialog belongs to.
 */
void
ai_assist_refresh (GschemToplevel *w_current)
{
  GtkWidget *dialog = w_current->ai_assist_window;
  gchar *report;

  if (dialog == NULL) {
    return;
  }

  ai_assist_show_schematic (dialog, w_current);

  report = ai_assist_describe_schematic (w_current);
  ai_assist_set_text (dialog, "textview-report", report);
  g_free (report);
}

/*! \brief Handle a signal from the AI Assist dialog.
 *
 *  \param [in] widget    The dialog.
 *  \param [in] response  The response id.
 *  \param [in] w_current The GschemToplevel the dialog belongs to.
 */
static void
ai_assist_dialog_response (GtkWidget *widget, gint response,
                           GschemToplevel *w_current)
{
  switch (response) {
  case AI_ASSIST_RESPONSE_REFRESH:
    /* Re-read the page in place; the dialog stays open. */
    ai_assist_refresh (w_current);
    return;

  case GTK_RESPONSE_DELETE_EVENT:
  case GTK_RESPONSE_CLOSE:
    break;

  default:
    /* Unknown responses leave the dialog open so the text available. */
    return;
  }

  gtk_widget_destroy (widget);
}

/*! \brief Clear the editor's dialog pointer on every destruction path.
 * \param widget Destroyed dialog (unused).
 * \param w_current Editor, still alive while its dialog is destroyed.
 */
static void
ai_assist_dialog_destroyed (GtkWidget *widget, GschemToplevel *w_current)
{
  (void) widget;
  w_current->ai_assist_window = NULL;
}

/*! \brief Create (or re-present) the AI Assist dialog.
 *
 *  The dialog is modeless, so the user can keep editing the schematic while
 *  it is open.  Its contents are a snapshot taken when it is opened; the
 *  Refresh button re-reads the current page.
 *
 *  \param [in] w_current The GschemToplevel to attach the dialog to.
 */
void
ai_assist_dialog (GschemToplevel *w_current)
{
  GtkWidget *vbox;
  GtkWidget *notebook;
  GtkWidget *scrolled;
  GtkWidget *textview;
  GtkWidget *frame;
  GtkWidget *label;

  if (w_current->ai_assist_window != NULL) {
    gtk_window_present (GTK_WINDOW (w_current->ai_assist_window));
    return;
  }

  w_current->ai_assist_window =
    gschem_dialog_new_with_buttons (_("AI Assist"),
                                    GTK_WINDOW (w_current->main_window),
                                    0,
                                    "ai-assist", w_current,
                                    "gtk-close", GTK_RESPONSE_CLOSE,
                                    NULL);

  g_signal_connect (G_OBJECT (w_current->ai_assist_window), "response",
                    G_CALLBACK (ai_assist_dialog_response), w_current);

  g_signal_connect (w_current->ai_assist_window, "destroy",
                    G_CALLBACK (ai_assist_dialog_destroyed), w_current);

  gtk_container_set_border_width (
    GTK_CONTAINER (w_current->ai_assist_window), DIALOG_BORDER_SPACING);

  vbox = gtk_dialog_get_content_area (
    GTK_DIALOG (w_current->ai_assist_window));
  gtk_box_set_spacing (GTK_BOX (vbox), DIALOG_V_SPACING);

  /* A short explanation, so the dialog makes sense on first use. */
  label = gtk_label_new (
    _("Each message includes the current schematic, including unsaved edits. "
      "Choose a provider and model below."));
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  gtk_label_set_xalign (GTK_LABEL (label), 0.0);
  gtk_label_set_max_width_chars (GTK_LABEL (label), 60);
  gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

  notebook = gtk_notebook_new ();
  gtk_widget_set_size_request (notebook, 640, 420);
  gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);

  gtk_notebook_append_page (GTK_NOTEBOOK (notebook),
                            gschem_ai_chat_new (w_current), gtk_label_new (_("Chat")));
  gtk_window_set_default_size (GTK_WINDOW (w_current->ai_assist_window), 800, 700);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (w_current->ai_assist_window), TRUE);

  /* Tab 1: the schematic as gEDA text. */
  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  textview = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_monospace (GTK_TEXT_VIEW (textview), TRUE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textview), 4);
  gtk_container_add (GTK_CONTAINER (scrolled), textview);

  label = gtk_label_new (_("Schematic text"));
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), scrolled, label);

  g_object_set_data (G_OBJECT (w_current->ai_assist_window),
                     "textview-schematic", textview);

  /* Tab 2: what the design contains and whether SPICE can use it. */
  frame = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (frame),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  textview = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (textview), FALSE);
  gtk_text_view_set_monospace (GTK_TEXT_VIEW (textview), TRUE);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textview), 4);
  gtk_container_add (GTK_CONTAINER (frame), textview);

  label = gtk_label_new (_("Design and SPICE"));
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), frame, label);

  g_object_set_data (G_OBJECT (w_current->ai_assist_window),
                     "textview-report", textview);

  /* The refresh button re-reads the page; without it the dialog would go
   * stale as soon as the user edited anything. */
  gtk_dialog_add_button (GTK_DIALOG (w_current->ai_assist_window),
                         _("gtk-refresh"), AI_ASSIST_RESPONSE_REFRESH);

  /* Populate before showing, so the first thing the user sees is real. */
  ai_assist_refresh (w_current);

  gtk_widget_show_all (w_current->ai_assist_window);
}


/*! \brief Build the text handed to an assistant for one prompt.
 *
 *  The format is fixed and deliberately not translated: it is read back by a
 *  program, and the section markers must mean the same thing on every
 *  machine regardless of locale.  Only the report body inside the
 *  "REPORT" section is localised, and it is informational.
 *
 *  \param [in] w_current   The GschemToplevel being described.
 *  \param [in] prompt      The user's instruction, or NULL if none.
 *  \return A newly allocated string; free with g_free().
 */
static gchar *
ai_assist_build_prompt (GschemToplevel *w_current, const gchar *prompt)
{
  GString *out = g_string_new (NULL);
  gchar *schematic = ai_assist_schematic_text (w_current);
  gchar *report = ai_assist_describe_schematic (w_current);

  g_string_append (out, "=== PROMPT ===\n");
  g_string_append (out, (prompt != NULL && *prompt != '\0')
                        ? prompt
                        : "(no prompt supplied)\n");
  if (out->len == 0 || out->str[out->len - 1] != '\n') {
    g_string_append_c (out, '\n');
  }

  g_string_append (out, "\n=== REPORT ===\n");
  g_string_append (out, report);
  if (out->len == 0 || out->str[out->len - 1] != '\n') {
    g_string_append_c (out, '\n');
  }

  g_string_append (out, "\n=== SCHEMATIC ===\n");
  g_string_append (out, schematic != NULL ? schematic : "");

  g_free (report);
  g_free (schematic);

  return g_string_free (out, FALSE);
}

/*! \brief Write a prompt bundle for the headless AI Assist command.
 *
 *  Reads \a prompt_file, bundles it with the schematic text and the SPICE
 *  report, and writes the result alongside it.  No model is called: this
 *  produces the input an assistant needs and reports the file name so the
 *  caller can pick it up.
 *
 *  \param [in]  w_current   The GschemToplevel to describe.
 *  \param [in]  prompt_file The prompt to read; must be a readable file.
 *  \param [out] output_file Where the bundle was written.  Set to NULL on
 *                           failure.  Free with g_free().
 *  \param [out] error       Populated on failure.
 *  \return TRUE on success.
 */
gboolean
ai_assist_write_prompt (GschemToplevel *w_current,
                        const gchar *prompt_file,
                        gchar **output_file,
                        GError **error)
{
  gchar *prompt = NULL;
  gchar *bundle = NULL;
  gchar *out_path = NULL;
  gboolean ok;

  g_return_val_if_fail (output_file != NULL, FALSE);
  *output_file = NULL;

  if (!g_file_get_contents (prompt_file, &prompt, NULL, error)) {
    return FALSE;
  }

  if (ai_assist_schematic_text (w_current) == NULL) {
    g_free (prompt);
    g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
                 "%s", "No schematic is currently open.");
    return FALSE;
  }

  bundle = ai_assist_build_prompt (w_current, prompt);

  /* Write next to the prompt rather than choosing a name in the schematic's
   * directory: the caller has to find the output again, and <prompt>.out is
   * predictable from the argument it already passed. */
  out_path = g_strdup_printf ("%s.out", prompt_file);

  ok = g_file_set_contents (out_path, bundle, -1, error);

  g_free (prompt);
  g_free (bundle);

  if (!ok) {
    g_free (out_path);
    return FALSE;
  }

  *output_file = out_path;
  return TRUE;
}
