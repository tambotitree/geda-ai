/* gEDA AI Assist chat; GPL-2.0-or-later. */
#ifndef GSCHEM_AI_CHAT_H
#define GSCHEM_AI_CHAT_H

#include <gtk/gtk.h>
typedef struct st_gschem_toplevel GschemToplevel;

GtkWidget *gschem_ai_chat_new (GschemToplevel *w_current);
gchar *gschem_ai_chat_quote (const gchar *text);
void gschem_ai_chat_append_message (GString *history, const gchar *role,
                                    const gchar *text);

#endif
