/* Register the compiled icon resource before constructing menus and toolbars. */
#include "gschem_icons.h"
#include "gschem_icon_resources.h"

/*! \brief Register the process-lifetime resource and expose it to a GTK theme.
 * \param theme Theme used for gschem's icon name lookups.
 * No files, environment variables, or icon caches are needed. The resource
 * stays registered for the process lifetime; each supplied theme can use it.
 */
void
gschem_icons_init (GtkIconTheme *theme)
{
  static gsize resource_registered = 0;

  g_return_if_fail (GTK_IS_ICON_THEME (theme));
  if (g_once_init_enter (&resource_registered)) {
    gschem_icon_data_register_resource ();
    g_once_init_leave (&resource_registered, 1);
  }
  gtk_icon_theme_add_resource_path (theme, "/org/geda/gschem/icons");
}
