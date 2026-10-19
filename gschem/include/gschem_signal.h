#ifndef GSCHEM_SIGNAL_H
#define GSCHEM_SIGNAL_H

#include <glib-object.h>

gulong gschem_signal_connect (gpointer instance, const gchar *signal,
                              GCallback callback, gpointer data);
gulong gschem_signal_connect_swapped (gpointer instance, const gchar *signal,
                                      GCallback callback, gpointer data);
guint gschem_signal_handlers_block (gpointer instance, GCallback callback,
                                    gpointer data);
guint gschem_signal_handlers_unblock (gpointer instance, GCallback callback,
                                      gpointer data);
guint gschem_signal_handlers_disconnect (gpointer instance, GCallback callback,
                                         gpointer data);
#endif
