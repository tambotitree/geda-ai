/* Opt-in isolated macOS GUI resize probe; GPL-2.0-or-later. */
#include <gtk/gtk.h>
#include <stdlib.h>
static unsigned int cycle;
static unsigned int waits;
/* Exercise the main window only; every resize returns to the GTK event loop. */
static gboolean resize_probe(gpointer unused)
{
  GList *windows = gtk_window_list_toplevels();
  GtkWindow *main_window = NULL;
  (void) unused;
  for (GList *node = windows; node; node = node->next) {
    const char *title = gtk_window_get_title(GTK_WINDOW(node->data));
    if (title && g_strrstr(title, "gschem") && gtk_widget_get_visible(node->data)) {
      main_window = GTK_WINDOW(node->data);
      break;
    }
  }
  g_list_free(windows);
  if (!main_window) {
    if (++waits > 40) exit(2);
    return G_SOURCE_CONTINUE;
  }
  if (cycle == 0) {
    GdkDisplay *display = gtk_widget_get_display(GTK_WIDGET(main_window));
    GdkMonitor *monitor = gdk_display_get_monitor_at_window(display,
      gtk_widget_get_window(GTK_WIDGET(main_window)));
    GdkRectangle work;
    gint width, height;
    g_assert_nonnull(monitor);
    gdk_monitor_get_workarea(monitor, &work);
    gtk_window_get_size(main_window, &width, &height);
    g_assert_cmpint(width, >=, MIN(1280, work.width));
    g_assert_cmpint(height, >=, MIN(720, work.height));
    g_print("PASS: usable startup size %dx%d.\n", width, height);
  }
  if (cycle >= 16) {
    g_print("PASS: startup and 16 resize/maximize cycles without Cairo/geometry failure.\n");
    exit(0);
  }
  if (cycle % 4 == 0) gtk_window_maximize(main_window);
  else {
    gtk_window_unmaximize(main_window);
    gtk_window_resize(main_window, cycle % 2 ? 1250 : 800, cycle % 2 ? 850 : 600);
  }
  cycle++;
  return G_SOURCE_CONTINUE;
}
/* Schedule after process load; do not touch GTK widgets before initialization. */
__attribute__((constructor)) static void start_probe(void)
{
  g_timeout_add(350, resize_probe, NULL);
}
