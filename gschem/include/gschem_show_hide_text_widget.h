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
 * \file gschem_show_hide_text_widget.h
 *
 * \brief A widget for showing or hiding text
 */

#define GSCHEM_TYPE_SHOW_HIDE_TEXT_WIDGET           (gschem_show_hide_text_widget_get_type())
#define GSCHEM_SHOW_HIDE_TEXT_WIDGET(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSCHEM_TYPE_SHOW_HIDE_TEXT_WIDGET, GschemShowHideTextWidget))
#define GSCHEM_SHOW_HIDE_TEXT_WIDGET_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass),  GSCHEM_TYPE_SHOW_HIDE_TEXT_WIDGET, GschemShowHideTextWidgetClass))
#define GSCHEM_IS_SHOW_HIDE_TEXT_WIDGET(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSCHEM_TYPE_SHOW_HIDE_TEXT_WIDGET))
#define GSCHEM_SHOW_HIDE_TEXT_WIDGET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSCHEM_TYPE_SHOW_HIDE_TEXT_WIDGET, GschemShowHideTextWidgetClass))

typedef struct _GschemShowHideTextWidgetClass GschemShowHideTextWidgetClass;
typedef struct _GschemShowHideTextWidget GschemShowHideTextWidget;

struct _GschemShowHideTextWidgetClass
{
  GtkInfoBarClass parent_class;
};

struct _GschemShowHideTextWidget
{
  GtkInfoBar parent;

  GtkWidget *entry;
  GtkWidget *label;
  GtkWidget *ok_button;
};



const char*
gschem_show_hide_text_widget_get_button_text (GschemShowHideTextWidget *widget);

GtkWidget*
gschem_show_hide_text_widget_get_entry (GschemShowHideTextWidget *widget);

const char*
gschem_show_hide_text_widget_get_label_text (GschemShowHideTextWidget *widget);

const char*
gschem_show_hide_text_widget_get_text_string (GschemShowHideTextWidget *widget);

GType
gschem_show_hide_text_widget_get_type ();

void
gschem_show_hide_text_widget_set_button_text (GschemShowHideTextWidget *widget, const char *text);

void
gschem_show_hide_text_widget_set_label_text (GschemShowHideTextWidget *widget, const char *text);

void
gschem_show_hide_text_widget_set_text_string (GschemShowHideTextWidget *widget, const char *str);
