/* One compile-time UI policy; schematic rendering has its own color map. */
#ifndef GSCHEM_APPEARANCE_H
#define GSCHEM_APPEARANCE_H

#define GSCHEM_UI_THEME "Adwaita"
#define GSCHEM_UI_ICON_THEME "hicolor"
#define GSCHEM_UI_PREFER_DARK 0
#define GSCHEM_UI_FONT "Sans 10"

void gschem_appearance_prepare (void);
void gschem_appearance_init (void);

#endif
