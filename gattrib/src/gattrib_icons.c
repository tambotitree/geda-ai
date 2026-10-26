/* Bundled gattrib artwork; GPL-2.0-or-later. */
#include <gtk/gtk.h>
#include "gattrib_icons.h"
#include "gattrib_icon_resources.h"
#include "gschem_appearance.h"

/*! \brief Select the shared fixed widget theme before GTK initializes. */
void
gattrib_icons_prepare (void)
{
  gschem_appearance_prepare ();
}

/*! \brief Expose compiled artwork with no desktop icon search directories. */
void
gattrib_icons_init (void)
{
  static gsize registered = 0;
  gschem_appearance_init ();
  if (g_once_init_enter (&registered)) {
    gattrib_icon_data_register_resource ();
    g_once_init_leave (&registered, 1);
  }
  gtk_icon_theme_add_resource_path (gtk_icon_theme_get_default (),
                                    "/org/geda/gattrib/icons");
}
