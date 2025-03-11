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
/*!
 * \file gschem_bottom_widget.h
 *
 * \brief A widget for the "status bar" at the bottom of the window
 */

#define GSCHEM_TYPE_BOTTOM_WIDGET           (gschem_bottom_widget_get_type())
#define GSCHEM_BOTTOM_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSCHEM_TYPE_BOTTOM_WIDGET, GschemBottomWidget))
#define GSCHEM_BOTTOM_WIDGET_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass),  GSCHEM_TYPE_BOTTOM_WIDGET, GschemBottomWidgetClass))
#define GSCHEM_IS_BOTTOM_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSCHEM_TYPE_BOTTOM_WIDGET))
#define GSCHEM_BOTTOM_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSCHEM_TYPE_BOTTOM_WIDGET, GschemBottomWidgetClass))

typedef struct _GschemBottomWidgetClass GschemBottomWidgetClass;
typedef struct _GschemBottomWidget GschemBottomWidget;

struct _GschemBottomWidgetClass
{
  GtkHBoxClass parent_class;
};

struct _GschemBottomWidget
{
  GtkHBox parent;

  GtkWidget *snap_label;
  GtkWidget *grid_label;
  int       grid_mode;
  int       grid_size;
  GtkWidget *left_button_label;
  GtkWidget *middle_button_label;
  GtkWidget *right_button_label;
  int       snap_mode;
  int       snap_size;
  GtkWidget *coord_label;
  GtkWidget *coord_separator;
  GtkWidget *status_label;
};



int
gschem_bottom_widget_get_grid_mode (GschemBottomWidget *widget);

int
gschem_bottom_widget_get_grid_size (GschemBottomWidget *widget);

const char*
gschem_bottom_widget_get_left_button_text (GschemBottomWidget *widget);

const char*
gschem_bottom_widget_get_middle_button_text (GschemBottomWidget *widget);

const char*
gschem_bottom_widget_get_right_button_text (GschemBottomWidget *widget);

int
gschem_bottom_widget_get_snap_mode (GschemBottomWidget *widget);

int
gschem_bottom_widget_get_snap_size (GschemBottomWidget *widget);

const char*
gschem_bottom_widget_get_status_text (GschemBottomWidget *widget);

GType
gschem_bottom_widget_get_type ();

void
gschem_bottom_widget_set_grid_mode (GschemBottomWidget *widget, int mode);

void
gschem_bottom_widget_set_grid_size (GschemBottomWidget *widget, int size);

void
gschem_bottom_widget_set_left_button_text (GschemBottomWidget *widget, const char *text);

void
gschem_bottom_widget_set_middle_button_text (GschemBottomWidget *widget, const char *text);

void
gschem_bottom_widget_set_right_button_text (GschemBottomWidget *widget, const char *text);

void
gschem_bottom_widget_set_snap_mode (GschemBottomWidget *widget, int mode);

void
gschem_bottom_widget_set_snap_size (GschemBottomWidget *widget, int size);

void
gschem_bottom_widget_set_status_text (GschemBottomWidget *widget, const char *text);

void
gschem_bottom_widget_set_status_text_color (GschemBottomWidget *widget, gboolean active);

void
gschem_bottom_widget_set_coordinates (GschemBottomWidget *bottom_widget,
                                      int world_x, int world_y);
