/* Verify embedded images and GTK lookups with no filesystem theme paths. */
#include "gschem_icons.h"
#include "gschem_appearance.h"
#include <string.h>

/*! \brief Decode one resource directory and look up every icon at both sizes.
 * \param theme Isolated GTK theme with only the embedded resource available.
 * \param size_directory Resource directory for one native image size.
 * \param native_size Expected unscaled image dimension, in pixels.
 * \return Number of decoded images. Assertion failure reports missing artwork.
 */
static guint
check_directory (GtkIconTheme *theme, const gchar *size_directory, gint native_size)
{
  GError *error = NULL;
  gchar *directory = g_strconcat ("/org/geda/gschem/icons/", size_directory,
                                  "/actions", NULL);
  gchar **files = g_resources_enumerate_children (directory, 0, &error);
  g_assert_no_error (error);
  g_assert_nonnull (files);
  guint count = 0;
  for (gchar **file = files; *file != NULL; file++) {
    gchar *path = g_strconcat (directory, "/", *file, NULL);
    GdkPixbuf *image = gdk_pixbuf_new_from_resource (path, &error);
    g_assert_no_error (error);
    g_assert_nonnull (image);
    g_assert_cmpint (gdk_pixbuf_get_width (image), ==, native_size);
    g_assert_cmpint (gdk_pixbuf_get_height (image), ==, native_size);
    g_object_unref (image);

    const gchar *extension = strrchr (*file, '.');
    g_assert_nonnull (extension);
    gchar *name = g_strndup (*file, extension - *file);
    const gint sizes[] = {16, 24};
    for (guint i = 0; i < G_N_ELEMENTS (sizes); i++) {
      image = gtk_icon_theme_load_icon (theme, name, sizes[i],
                                        GTK_ICON_LOOKUP_FORCE_SIZE, &error);
      g_assert_no_error (error);
      g_assert_nonnull (image);
      g_assert_cmpint (gdk_pixbuf_get_width (image), ==, sizes[i]);
      g_assert_cmpint (gdk_pixbuf_get_height (image), ==, sizes[i]);
      g_object_unref (image);
    }
    g_free (name);
    g_free (path);
    count++;
  }
  g_strfreev (files);
  g_free (directory);
  return count;
}

/*! \brief Check the actual default theme used by editor widgets and dialogs.
 * Clears filesystem paths through production initialization, then decodes
 * application and GTK built-in fallback icons to check their availability.
 */
static void
check_default_theme (void)
{
  GError *error = NULL;
  GtkIconTheme *theme = gtk_icon_theme_get_default ();
  const gchar *names[] = {"geda-gschem", "dialog-warning", "dialog-error",
                          "dialog-information", "dialog-question",
                          "image-missing", "folder", "text-x-generic",
                          "applications-science", "applications-science-ltr",
                          "applications-science-rtl"};
  gschem_icons_init (theme);
  for (guint i = 0; i < G_N_ELEMENTS (names); i++) {
    GdkPixbuf *image = gtk_icon_theme_load_icon (theme, names[i], 24,
                                                GTK_ICON_LOOKUP_FORCE_SIZE,
                                                &error);
    g_assert_no_error (error);
    g_assert_nonnull (image);
    g_object_unref (image);
  }
}

/*! \brief Check resource registration and all menu/toolbar icon lookups.
 * Uses an isolated icon theme, so a desktop theme cannot hide missing images.
 * Returns 77 if GTK cannot connect to a display, otherwise zero on success.
 */
int
main (int argc, char **argv)
{
  g_setenv ("GTK_THEME", "NonexistentDesktopTheme:dark", TRUE);
  gschem_appearance_prepare ();
  if (!gtk_init_check (&argc, &argv)) {
    g_print ("SKIP: GTK display unavailable\n");
    return 77;
  }

  gschem_appearance_init ();
  GtkSettings *settings = gtk_settings_get_default ();
  gchar *theme_name = NULL, *icon_theme_name = NULL;
  gboolean prefer_dark = TRUE;
  g_object_get (settings, "gtk-theme-name", &theme_name,
                "gtk-icon-theme-name", &icon_theme_name,
                "gtk-application-prefer-dark-theme", &prefer_dark, NULL);
  g_assert_cmpstr (theme_name, ==, GSCHEM_UI_THEME);
  g_assert_cmpstr (icon_theme_name, ==, GSCHEM_UI_ICON_THEME);
  g_assert_cmpint (prefer_dark, ==, GSCHEM_UI_PREFER_DARK);
  g_free (theme_name);
  g_free (icon_theme_name);
  gchar **search_paths = NULL;
  gint path_count = -1;
  gtk_icon_theme_get_search_path (gtk_icon_theme_get_default (),
                                 &search_paths, &path_count);
  g_assert_cmpint (path_count, ==, 0);
  g_strfreev (search_paths);

  check_default_theme ();

  GtkIconTheme *theme = gtk_icon_theme_new ();
  gtk_icon_theme_set_custom_theme (theme, "hicolor");
  gtk_icon_theme_set_search_path (theme, NULL, 0);
  g_assert_false (gtk_icon_theme_has_icon (theme, "insert-net"));
  gschem_icons_init (theme);

  guint count = check_directory (theme, "16x16", 16);
  count += check_directory (theme, "24x24", 24);
  g_assert_cmpuint (count, >=, 66);
  g_object_unref (theme);
  g_print ("PASS: %u embedded icons decoded and resolved at 16 and 24 pixels\n", count);
  return 0;
}
