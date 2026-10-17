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
#include <math.h>

#include "gschem.h"
/*!
 * \brief Warp the mouse pointer to a widget-relative coordinate.
 *
 * \param widget A pointer to the GtkWidget to warp within.
 * \param x Horizontal offset from the widget origin.
 * \param y Vertical offset from the widget origin.
 *
 * This function moves the pointer to a given (x, y) coordinate within
 * the toplevel window containing `widget`. It updates the device pointer
 * using GDK APIs.
 */
void x_basic_warp_cursor(GtkWidget* widget, gint x, gint y)
{
  GdkDisplay *display;
  GdkScreen *screen;
  GdkSeat *seat;
  GdkDevice *pointer;
  gint window_x, window_y;

  gdk_window_get_origin(gtk_widget_get_window(widget), &window_x, &window_y);
  screen = gtk_widget_get_screen(widget);
  display = gdk_screen_get_display(screen);

  seat = gdk_display_get_default_seat(display);
  pointer = gdk_seat_get_pointer(seat);

  gdk_device_warp(pointer, screen, window_x + x, window_y + y);
}