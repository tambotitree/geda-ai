/* Fixed GTK appearance, independent of the desktop's theme selection. */
#include <gtk/gtk.h>
#include "gschem_appearance.h"

/*! \brief Select our built-in theme before GTK reads environment settings.
 * Overrides GTK_THEME for this process; no desktop configuration is changed.
 */
void
gschem_appearance_prepare (void)
{
  g_setenv ("GTK_THEME", GSCHEM_UI_THEME, TRUE);
}

/*! \brief Apply the compile-time appearance policy after GTK initialization.
 * Sets application-priority settings before widgets are constructed. Uses
 * GTK's built-in widget theme and font fallback, with no desktop icon paths.
 * Drawing-area colors and object rendering are configured independently.
 */
void
gschem_appearance_init (void)
{
  GtkSettings *settings = gtk_settings_get_default ();
  g_return_if_fail (GTK_IS_SETTINGS (settings));

  g_object_set (settings,
                "gtk-theme-name", GSCHEM_UI_THEME,
                "gtk-application-prefer-dark-theme", GSCHEM_UI_PREFER_DARK,
                "gtk-icon-theme-name", GSCHEM_UI_ICON_THEME,
                "gtk-fallback-icon-theme", "",
                "gtk-font-name", GSCHEM_UI_FONT,
                NULL);
  gtk_icon_theme_set_search_path (gtk_icon_theme_get_default (), NULL, 0);
}
