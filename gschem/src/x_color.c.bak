/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2020 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <config.h>

#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <math.h>

#include "gschem.h"

COLOR display_colors[MAX_COLORS];
COLOR display_outline_colors[MAX_COLORS];

// Previously GdkColor*, now we store actual RGBA structs
static GdkRGBA gdk_colors[MAX_COLORS];
static GdkRGBA gdk_outline_colors[MAX_COLORS];

// Default foreground/background colors
GdkRGBA black;
GdkRGBA white;

void
x_color_init (void)
{
  s_color_map_defaults (display_colors);
  s_color_map_defaults (display_outline_colors);
}

void
x_color_free (void)
{
  // Nothing to free: GdkRGBA is a value type (no allocation)
}

void
x_color_allocate (void)
{
  int i;
  COLOR c;

  // Parse static named colors
  gdk_rgba_parse(&black, "black");
  gdk_rgba_parse(&white, "white");

  for (i = 0; i < MAX_COLORS; i++) {
    if (display_colors[i].enabled) {
      c = display_colors[i];

      gdk_colors[i].red   = c.r / 255.0;
      gdk_colors[i].green = c.g / 255.0;
      gdk_colors[i].blue  = c.b / 255.0;
      gdk_colors[i].alpha = 1.0;
    } else {
      // alpha = 0 → disabled
      gdk_colors[i].red = gdk_colors[i].green = gdk_colors[i].blue = 0;
      gdk_colors[i].alpha = 0.0;
    }

    if (display_outline_colors[i].enabled) {
      c = display_outline_colors[i];

      gdk_outline_colors[i].red   = c.r / 255.0;
      gdk_outline_colors[i].green = c.g / 255.0;
      gdk_outline_colors[i].blue  = c.b / 255.0;
      gdk_outline_colors[i].alpha = 1.0;
    } else {
      gdk_outline_colors[i].red = gdk_outline_colors[i].green = gdk_outline_colors[i].blue = 0;
      gdk_outline_colors[i].alpha = 0.0;
    }
  }

  x_colorcb_update_store();
}

const GdkRGBA *
x_get_color(int color)
{
  if ((color < 0) || (color >= MAX_COLORS) || gdk_colors[color].alpha == 0.0) {
    g_warning (_("Tried to get an invalid color: %d\n"), color);
    return &white;
  } else {
    return &gdk_colors[color];
  }
}

COLOR *
x_color_lookup (int color)
{
  if (color < 0 || color >= MAX_COLORS || !display_colors[color].enabled) {
    fprintf(stderr, _("Tried to get an invalid color: %d\n"), color);
    return &display_colors[DEFAULT_COLOR];
  } else {
    return &display_colors[color];
  }
}

gboolean
x_color_display_enabled (int index)
{
  return (gdk_colors[index].alpha != 0.0);
}