/* Private AI Assist state; GPL-2.0-or-later. */
#ifndef GSCHEM_AI_CHAT_INTERNAL_H
#define GSCHEM_AI_CHAT_INTERNAL_H
#include "gschem.h"
#include "gschem_ai_chat.h"

typedef struct {
  GschemToplevel *window;
  GtkWidget *panel, *settings, *provider, *endpoint, *models, *key, *env_file;
  GtkWidget *transcript, *input, *send, *cancel, *clear, *status;
  GSubprocess *process;
  GKeyFile *config;
  GString *history;
  gchar *pending, *provider_id;
  PAGE *page_identity;
  gboolean closed, cancelled, listing_models;
} GschemAiChat;

GtkWidget *gschem_ai_chat_settings_new (GschemAiChat *chat);
void gschem_ai_chat_settings_save (GschemAiChat *chat);
void gschem_ai_chat_list_models (GtkButton *button, GschemAiChat *chat);
#endif
