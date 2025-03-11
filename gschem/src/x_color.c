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

static GdkColor* gdk_colors[MAX_COLORS];
static GdkColor* gdk_outline_colors[MAX_COLORS];

static GdkColormap *colormap = NULL;

/*! \brief Initializes the color system for the application.
 *  \par Function Documentation
 *
 *  Initialises the color maps to defaults.
 */
void
x_color_init (void)
{
  colormap = gdk_colormap_get_system ();

  /* Initialise default color maps */
  s_color_map_defaults (display_colors);
  s_color_map_defaults (display_outline_colors);
}

/*! \brief Frees memory used by the color system.
 *  \par Function Documentation
 *  This function frees the colors from colormap along with
 *  \b black and \b white.
 */
void
x_color_free (void)
{
  int i;

  gdk_colormap_free_colors (colormap, &black, 1);
  gdk_colormap_free_colors (colormap, &white, 1);

  for (i = 0; i < MAX_COLORS; i++) {
    if (display_colors[i].enabled)
      gdk_colormap_free_colors (colormap, gdk_colors[i], 1);
    if (display_outline_colors[i].enabled)
      gdk_colormap_free_colors (colormap, gdk_outline_colors[i], 1);
  }
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Documentation
 *
 */
void x_color_allocate (void)
{
  int error;
  int i;		
  COLOR c;

  gdk_color_parse ("black", &black);
  if (!gdk_colormap_alloc_color (colormap,
                                 &black,
                                 FALSE,
                                 TRUE)) {
    fprintf (stderr, _("Could not allocate the color %s!\n"), _("black"));
    exit (-1);
  }

  gdk_color_parse ("white", &white);
  if (!gdk_colormap_alloc_color (colormap,
                                 &white,
                                 FALSE,
                                 TRUE)) {
    fprintf (stderr, _("Could not allocate the color %s!\n"), _("white"));
    exit (-1);
  }

  for (i = 0; i < MAX_COLORS; i++) {

    if (display_colors[i].enabled) {
      gdk_colors[i] = (GdkColor *)
        g_malloc(sizeof(GdkColor));

      c = display_colors[i];

      /* Interpolate 8-bpp colours into 16-bpp GDK color
       * space. N.b. ignore transparency because GDK doesn't
       * understand it. */
      gdk_colors[i]->red = c.r + (c.r<<8);
      gdk_colors[i]->green = c.g + (c.g<<8);
      gdk_colors[i]->blue = c.b + (c.b<<8);

      error = gdk_color_alloc(colormap, gdk_colors[i]);

      if (error == FALSE) {
        g_error (_("Could not allocate display color %i!\n"), i);
      }
    } else {
      gdk_colors[i] = NULL;
    }

    if (display_outline_colors[i].enabled) {
      gdk_outline_colors[i] = (GdkColor *)
        g_malloc(sizeof(GdkColor));

      c = display_outline_colors[i];

      /* Interpolate 8-bpp colours into 16-bpp GDK color
       * space. N.b. ignore transparency because GDK doesn't
       * understand it. */
      gdk_outline_colors[i]->red = c.r + (c.r<<8);
      gdk_outline_colors[i]->green = c.g + (c.g<<8);
      gdk_outline_colors[i]->blue = c.b + (c.b<<8);

      error = gdk_color_alloc(colormap, gdk_outline_colors[i]);

      if (error == FALSE) {
        g_error (_("Could not allocate outline color %i!\n"), i);
      }
    } else {
      gdk_outline_colors[i] = NULL;
    }
  }

  x_colorcb_update_store ();
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Documentation
 *
 */
GdkColor *x_get_color(int color)
{
  if ((color < 0) || (color >= MAX_COLORS)
      || (gdk_colors[color] == NULL)) {
    g_warning (_("Tried to get an invalid color: %d\n"), color);
    return(&white);
  } else {
    return(gdk_colors[color]);
  }
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Documentation
 *
 */
COLOR *x_color_lookup (int color)
{
  if (color < 0 || color >= MAX_COLORS ||
      !display_colors[color].enabled) {
    fprintf(stderr, _("Tried to get an invalid color: %d\n"), color);
    return &display_colors[DEFAULT_COLOR];
  } else {
    return &display_colors[color];
  }
}

gboolean
x_color_display_enabled (int index)
{
  return (gdk_colors[index] != NULL);
}
