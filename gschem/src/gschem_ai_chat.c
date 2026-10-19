/* gEDA AI Assist asynchronous chat; GPL-2.0-or-later.
 *
 * panel
 * +-- settings grid (provider, endpoint, model, credentials)
 * +-- scrolled transcript
 * +-- message entry + Send + Cancel + New chat
 * +-- status
 *
 * The helper process handles HTTP/JSON, not circuit edits. The GTK main loop
 * stays responsive. Each send captures the live page; history contains only
 * successful question/answer pairs, without accumulating schematic snapshots.
 */
#include <config.h>
#include "gschem_ai_chat_internal.h"
#include "gschem_ai_assist.h"

/*! \brief Append plain text to the transcript and scroll to the last turn.
 * \param chat Owner of the transcript.
 * \param speaker Visible speaker name.
 * \param text Plain text; never interpreted as markup or commands.
 */
static void
chat_append (GschemAiChat *chat, const gchar *speaker, const gchar *text)
{
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (chat->transcript));
  GtkTextIter end;
  gtk_text_buffer_get_end_iter (buffer, &end);
  gchar *line = g_strdup_printf ("%s:\n%s\n\n", speaker, text);
  gtk_text_buffer_insert (buffer, &end, line, -1);
  gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (chat->transcript),
                                     gtk_text_buffer_get_insert (buffer));
  g_free (line);
}

/*! \brief Set request sensitivity without blocking schematic editing.
 * \param chat Chat controls.
 * \param busy TRUE while its helper is running.
 */
static void
chat_busy (GschemAiChat *chat, gboolean busy)
{
  gtk_widget_set_sensitive (chat->settings, !busy);
  gtk_widget_set_sensitive (chat->send, !busy);
  gtk_widget_set_sensitive (chat->input, !busy);
  gtk_widget_set_sensitive (chat->clear, !busy);
  gtk_widget_set_sensitive (chat->cancel, busy);
}

/*! \brief Release chat state after its panel and async callbacks are finished.
 * \param data Owned GschemAiChat, attached to the panel.
 */
static void
chat_free (gpointer data)
{
  GschemAiChat *chat = data;
  g_clear_object (&chat->process);
  g_key_file_unref (chat->config);
  g_string_free (chat->history, TRUE);
  g_free (chat->pending);
  g_free (chat->provider_id);
  g_free (chat);
}

/*! \brief Stop a helper before the parent window's plain C state is freed.
 * \param panel Destroyed panel (unused).
 * \param chat State kept alive by any outstanding async callback.
 */
static void
chat_destroy (GtkWidget *panel, GschemAiChat *chat)
{
  (void) panel;
  chat->closed = TRUE;
  chat->window = NULL;
  if (chat->process != NULL)
    g_subprocess_force_exit (chat->process);
}

/*! \brief Cancel only this chat's outstanding request.
 * \param button Cancel button (unused).
 * \param chat State; controls remain busy until the process has exited.
 */
static void
chat_cancel (GtkButton *button, GschemAiChat *chat)
{
  (void) button;
  if (chat->process != NULL) {
    chat->cancelled = TRUE;
    g_subprocess_force_exit (chat->process);
    gtk_label_set_text (GTK_LABEL (chat->status), _("Cancelling…"));
  }
}

/*! \brief Clear the conversation; provider settings are retained.
 * \param button New chat button (unused).
 * \param chat Idle chat state.
 */
static void
chat_clear (GtkButton *button, GschemAiChat *chat)
{
  (void) button;
  if (chat->process != NULL)
    return;
  g_string_truncate (chat->history, 0);
  GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (chat->transcript));
  gtk_text_buffer_set_text (buffer, "", 0);
  gtk_label_set_text (GTK_LABEL (chat->status), _("New chat uses the current schematic."));
}

/*! \brief Populate model IDs returned by the selected provider.
 * \param chat Chat owning an editable model dropdown.
 * \param text Newline-separated IDs; an existing typed model is preserved.
 */
static void
chat_models_received (GschemAiChat *chat, const gchar *text)
{
  GtkWidget *entry = gtk_bin_get_child (GTK_BIN (chat->models));
  gchar *previous = g_strdup (gtk_entry_get_text (GTK_ENTRY (entry)));
  gtk_combo_box_text_remove_all (GTK_COMBO_BOX_TEXT (chat->models));
  gchar **models = g_strsplit (text, "\n", -1);
  for (guint i = 0; models[i] != NULL; i++) {
    if (*models[i])
      gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (chat->models), models[i]);
  }
  gtk_entry_set_text (GTK_ENTRY (entry), previous);
  gtk_label_set_text (GTK_LABEL (chat->status),
    *text ? _("Choose a model from the dropdown, or type its ID.")
          : _("No models returned. Install a model or check provider access."));
  g_strfreev (models);
  g_free (previous);
}

/*! \brief Complete a helper request on the main thread.
 * \param source Helper process.
 * \param result Async communication result.
 * \param data Referenced chat panel; its state survives dialog destruction.
 */
static void
chat_completed (GObject *source, GAsyncResult *result, gpointer data)
{
  GtkWidget *panel = data;
  GschemAiChat *chat = g_object_get_data (G_OBJECT (panel), "ai-chat");
  gchar *output = NULL, *diagnostic = NULL;
  GError *error = NULL;
  gboolean ok = g_subprocess_communicate_utf8_finish (G_SUBPROCESS (source), result,
                                                      &output, &diagnostic, &error);
  if (!chat->closed) {
    if (chat->cancelled) {
      gtk_label_set_text (GTK_LABEL (chat->status), _("Request cancelled."));
    } else if (ok && g_subprocess_get_successful (G_SUBPROCESS (source))) {
      if (chat->listing_models) {
        chat_models_received (chat, output);
      } else {
        chat_append (chat, _("Assistant"), output);
        gschem_ai_chat_append_message (chat->history, "user", chat->pending);
        gschem_ai_chat_append_message (chat->history, "assistant", output);
        gtk_entry_set_text (GTK_ENTRY (chat->input), "");
        gtk_label_set_text (GTK_LABEL (chat->status), _("Reply received."));
      }
    } else {
      const gchar *message = error != NULL ? error->message :
        (diagnostic != NULL && *diagnostic ? diagnostic : _("Model request failed."));
      gtk_label_set_text (GTK_LABEL (chat->status), message);
      chat_append (chat, _("Error"), message);
    }
    chat_busy (chat, FALSE);
  }
  g_clear_object (&chat->process);
  g_clear_pointer (&chat->pending, g_free);
  g_clear_error (&error);
  g_free (output);
  g_free (diagnostic);
  g_object_unref (panel);
}

/*! \brief Add a string-valued field to the helper's JSON request.
 * \param request Mutable object, already containing its first field.
 * \param name Field name (an internal identifier).
 * \param value UTF-8 value; encoded without shell interpolation.
 */
static void
chat_field (GString *request, const gchar *name, const gchar *value)
{
  gchar *quoted = gschem_ai_chat_quote (value);
  g_string_append_printf (request, ",\"%s\":%s", name, quoted);
  g_free (quoted);
}

/*! \brief Start a model-list or chat request through a private stdin pipe.
 * \param chat Idle state with a valid current page for chat requests.
 * \param list_models TRUE fetches IDs without sending schematic data.
 * Side effects: settings saved, helper launched, controls marked busy.
 */
static void
chat_request (GschemAiChat *chat, gboolean list_models)
{
  if (chat->process != NULL || chat->closed)
    return;
  GString *request = g_string_new (list_models ? "{\"operation\":\"models\""
                                               : "{\"operation\":\"chat\"");
  if (!list_models) {
    gchar *question = g_strdup (gtk_entry_get_text (GTK_ENTRY (chat->input)));
    g_strstrip (question);
    gchar *schematic = ai_assist_schematic_text (chat->window);
    if (*question == '\0' || schematic == NULL) {
      gtk_label_set_text (GTK_LABEL (chat->status), _("Open a schematic and enter a message."));
      g_free (schematic);
      g_free (question);
      g_string_free (request, TRUE);
      return;
    }
    PAGE *page = gschem_page_view_get_page (
      gschem_toplevel_get_current_page_view (chat->window));
    if (chat->page_identity != page) {
      chat_clear (NULL, chat);
      chat->page_identity = page;
    }
    gchar *report = ai_assist_describe_schematic (chat->window);
    chat_field (request, "question", question);
    chat_field (request, "schematic", schematic);
    chat_field (request, "report", report);
    g_string_append_printf (request, ",\"history\":[%s]", chat->history->str);
    chat->pending = question;
    chat_append (chat, _("You"), question);
    ai_assist_refresh (chat->window);
    g_free (schematic);
    g_free (report);
  }
  GtkWidget *model = gtk_bin_get_child (GTK_BIN (chat->models));
  const gchar *env = gtk_entry_get_text (GTK_ENTRY (chat->env_file));
  chat_field (request, "provider", chat->provider_id);
  chat_field (request, "endpoint", gtk_entry_get_text (GTK_ENTRY (chat->endpoint)));
  chat_field (request, "model", gtk_entry_get_text (GTK_ENTRY (model)));
  chat_field (request, "env_file", env);
  chat_field (request, "api_key", gtk_entry_get_text (GTK_ENTRY (chat->key)));
  g_string_append_c (request, '}');
  gschem_ai_chat_settings_save (chat);
  gchar *helper = g_build_filename (s_path_sys_data (), "ai", "chat_backend.py", NULL);
  GError *error = NULL;
  chat->process = g_subprocess_new (G_SUBPROCESS_FLAGS_STDIN_PIPE |
    G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_PIPE,
    &error, GSCHEM_AI_PYTHON, helper, NULL);
  g_free (helper);
  if (chat->process == NULL) {
    gtk_label_set_text (GTK_LABEL (chat->status), error->message);
    g_clear_error (&error);
    g_clear_pointer (&chat->pending, g_free);
  } else {
    chat->listing_models = list_models;
    chat->cancelled = FALSE;
    chat_busy (chat, TRUE);
    gtk_label_set_text (GTK_LABEL (chat->status),
      list_models ? _("Loading models…") : _("Waiting for the model…"));
    g_subprocess_communicate_utf8_async (chat->process, request->str, NULL,
                                        chat_completed, g_object_ref (chat->panel));
  }
  g_string_free (request, TRUE);
}

/*! \brief Load model IDs for the selected provider; sends no schematic.
 * \param button Load Models button (unused).
 * \param chat Owner state.
 */
void
gschem_ai_chat_list_models (GtkButton *button, GschemAiChat *chat)
{
  (void) button;
  chat_request (chat, TRUE);
}

/*! \brief Send a message from the Send button.
 * \param button Signal source (unused).
 * \param chat Owner state.
 */
static void
chat_send_clicked (GtkButton *button, GschemAiChat *chat)
{
  (void) button;
  chat_request (chat, FALSE);
}

/*! \brief Send a message when Enter is pressed in the message entry.
 * \param entry Signal source (unused).
 * \param chat Owner state.
 */
static void
chat_send_activated (GtkEntry *entry, GschemAiChat *chat)
{
  (void) entry;
  chat_request (chat, FALSE);
}

/*! \brief Create the native chat panel; parent ownership controls its lifetime.
 * \param w_current Editor owning the current live schematic.
 * \return Chat panel with settings, transcript, input/actions and status.
 */
GtkWidget *
gschem_ai_chat_new (GschemToplevel *w_current)
{
  GschemAiChat *chat = g_new0 (GschemAiChat, 1);
  chat->window = w_current;
  chat->history = g_string_new (NULL);
  chat->panel = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
  chat->status = gtk_label_new (NULL);
  gtk_label_set_xalign (GTK_LABEL (chat->status), 0);
  gtk_label_set_line_wrap (GTK_LABEL (chat->status), TRUE);
  chat->settings = gschem_ai_chat_settings_new (chat);
  gtk_box_pack_start (GTK_BOX (chat->panel), chat->settings, FALSE, FALSE, 0);
  GtkWidget *scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  chat->transcript = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (chat->transcript), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (chat->transcript), GTK_WRAP_WORD_CHAR);
  gtk_container_add (GTK_CONTAINER (scrolled), chat->transcript);
  gtk_box_pack_start (GTK_BOX (chat->panel), scrolled, TRUE, TRUE, 0);
  GtkWidget *row = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
  chat->input = gtk_entry_new ();
  gtk_entry_set_placeholder_text (GTK_ENTRY (chat->input), _("Ask about the current schematic…"));
  chat->send = gtk_button_new_with_label (_("Send"));
  chat->cancel = gtk_button_new_with_label (_("Cancel"));
  chat->clear = gtk_button_new_with_label (_("New chat"));
  gtk_box_pack_start (GTK_BOX (row), chat->input, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (row), chat->send, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (row), chat->cancel, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (row), chat->clear, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (chat->panel), row, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (chat->panel), chat->status, FALSE, FALSE, 0);
  g_object_set_data_full (G_OBJECT (chat->panel), "ai-chat", chat, chat_free);
  g_signal_connect (chat->panel, "destroy", G_CALLBACK (chat_destroy), chat);
  g_signal_connect (chat->send, "clicked", G_CALLBACK (chat_send_clicked), chat);
  g_signal_connect (chat->input, "activate", G_CALLBACK (chat_send_activated), chat);
  g_signal_connect (chat->cancel, "clicked", G_CALLBACK (chat_cancel), chat);
  g_signal_connect (chat->clear, "clicked", G_CALLBACK (chat_clear), chat);
  chat_busy (chat, FALSE);
  return chat->panel;
}
