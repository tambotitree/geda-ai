/* Retain signal IDs without converting function pointers into data pointers. */
#include "gschem_signal.h"

typedef struct {
  GCallback callback;
  gpointer data;
  gulong id;
} SignalConnection;

/* Release bookkeeping when the emitting object is finalized. */
static void
connections_free (gpointer data)
{
  g_array_unref (data);
}

/* The emitter owns the IDs; neither callback nor user data is owned here. */
static GArray *
connections_get (gpointer instance)
{
  GArray *connections = g_object_get_data (instance, "gschem-signal-connections");
  if (connections == NULL) {
    connections = g_array_new (FALSE, FALSE, sizeof (SignalConnection));
    g_object_set_data_full (instance, "gschem-signal-connections", connections,
                            connections_free);
  }
  return connections;
}

/* Connect through GObject's callback API and retain its exact connection ID. */
static gulong
connect_with_flags (gpointer instance, const gchar *signal, GCallback callback,
                     gpointer data, GConnectFlags flags)
{
  SignalConnection connection = {callback, data, 0};
  connection.id = g_signal_connect_data (instance, signal, callback, data,
                                         NULL, flags);
  if (connection.id != 0) {
    g_array_append_val (connections_get (instance), connection);
  }
  return connection.id;
}

/* Register a normal signal callback. */
gulong
gschem_signal_connect (gpointer instance, const gchar *signal,
                       GCallback callback, gpointer data)
{
  return connect_with_flags (instance, signal, callback, data, 0);
}

/* Register a callback with the instance and user-data argument order swapped. */
gulong
gschem_signal_connect_swapped (gpointer instance, const gchar *signal,
                               GCallback callback, gpointer data)
{
  return connect_with_flags (instance, signal, callback, data, G_CONNECT_SWAPPED);
}

typedef enum { CONNECTION_BLOCK, CONNECTION_UNBLOCK, CONNECTION_REMOVE } ConnectionOperation;

/* Match typed callback addresses and user data, then operate on retained IDs. */
static guint
connections_apply (gpointer instance, GCallback callback, gpointer data,
                    ConnectionOperation operation)
{
  GArray *connections = connections_get (instance);
  guint count = 0;
  for (guint i = 0; i < connections->len;) {
    SignalConnection *connection = &g_array_index (connections, SignalConnection, i);
    if (!g_signal_handler_is_connected (instance, connection->id)) {
      g_array_remove_index (connections, i);
      continue;
    }
    if (connection->callback != callback || connection->data != data) {
      i++;
      continue;
    }
    count++;
    if (operation == CONNECTION_REMOVE) {
      g_signal_handler_disconnect (instance, connection->id);
      g_array_remove_index (connections, i);
    } else {
      if (operation == CONNECTION_BLOCK) {
        g_signal_handler_block (instance, connection->id);
      } else {
        g_signal_handler_unblock (instance, connection->id);
      }
      i++;
    }
  }
  return count;
}

/* Block all matching tracked connections, preserving GObject's nesting count. */
guint
gschem_signal_handlers_block (gpointer instance, GCallback callback, gpointer data)
{
  return connections_apply (instance, callback, data, CONNECTION_BLOCK);
}

/* Unblock the corresponding set of tracked connections. */
guint
gschem_signal_handlers_unblock (gpointer instance, GCallback callback, gpointer data)
{
  return connections_apply (instance, callback, data, CONNECTION_UNBLOCK);
}

/* Disconnect matching IDs and discard their bookkeeping. */
guint
gschem_signal_handlers_disconnect (gpointer instance, GCallback callback, gpointer data)
{
  return connections_apply (instance, callback, data, CONNECTION_REMOVE);
}
