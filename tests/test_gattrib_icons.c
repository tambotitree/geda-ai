/* Exercise the production appearance and icon initialization without themes. */
#include <gtk/gtk.h>
#include "gattrib_icons.h"

/*! \brief Load application and dialog icons with no filesystem theme paths. */
int
main (int argc, char **argv)
{
  GError *error = NULL;
  gchar **paths = NULL;
  gint count = -1;
  const gchar *names[] = {"geda-gattrib", "dialog-error", "dialog-warning"};
  g_setenv ("GTK_THEME", "MissingDesktopTheme", TRUE);
  gattrib_icons_prepare ();
  if (!gtk_init_check (&argc, &argv))
    return 77;
  gattrib_icons_init ();
  GtkIconTheme *theme = gtk_icon_theme_get_default ();
  gtk_icon_theme_get_search_path (theme, &paths, &count);
  g_assert_cmpint (count, ==, 0);
  g_strfreev (paths);
  for (guint i = 0; i < G_N_ELEMENTS (names); i++) {
    GdkPixbuf *icon = gtk_icon_theme_load_icon (theme, names[i], 48,
                                                GTK_ICON_LOOKUP_FORCE_SIZE, &error);
    g_assert_no_error (error);
    g_assert_nonnull (icon);
    g_object_unref (icon);
  }
  g_print ("PASS: gattrib application/dialog icons without desktop themes\n");
  return 0;
}
