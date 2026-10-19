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
/*! \todo STILL NEED to clean up line lengths in aa and tr */
#include <config.h>

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"

#define GLADE_HOOKUP_OBJECT(component, widget, name) \
  g_object_set_data_full(G_OBJECT(component), name,  \
                         gtk_widget_ref(widget), (GDestroyNotify)g_object_unref)

/***************** Start of Arc dialog box ***************************/

/*! \brief response function for the arc angle dialog
 *  \par Function Description
 *  The response function of th arc angle dialog takes the content of
 *  the dialog and applies it on the current arc.
 *  If the dialog is closed or canceled the function destroys the dialog.
 */
void arc_angle_dialog_response(GtkDialog *dialog, gint response_id, GschemToplevel *w_current)
{
  GtkWidget *spinentry;
  gint radius, start_angle, sweep_angle;
  OBJECT *arc_object = NULL;

  switch (response_id)
  {
    case GTK_RESPONSE_REJECT:
    case GTK_RESPONSE_DELETE_EVENT:
      // No action needed
      break;

    case GTK_RESPONSE_ACCEPT:
      spinentry = g_object_get_data(G_OBJECT(w_current->aawindow), "radius");
      radius = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinentry));

      spinentry = g_object_get_data(G_OBJECT(w_current->aawindow), "spin_start");
      start_angle = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinentry));

      spinentry = g_object_get_data(G_OBJECT(w_current->aawindow), "spin_sweep");
      sweep_angle = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinentry));

      arc_object = g_object_get_data(G_OBJECT(w_current->aawindow), "arc_object");

      if (arc_object != NULL)
      {
        o_arc_modify(w_current->toplevel, arc_object, radius, 0, ARC_RADIUS);
        o_arc_modify(w_current->toplevel, arc_object, start_angle, 0, ARC_START_ANGLE);
        o_arc_modify(w_current->toplevel, arc_object, sweep_angle, 0, ARC_SWEEP_ANGLE);
      }
      else
      {
        o_arc_end4(w_current, radius, start_angle, sweep_angle);
      }
      break;

    default:
      g_warning("arc_angle_dialog_response(): unexpected response %d", response_id);
      break;
  }

  gtk_widget_destroy(GTK_WIDGET(w_current->aawindow));
  w_current->aawindow = NULL;
}

/**
 * \brief Display and initialize the Arc Parameters dialog box.
 *
 * This function creates (if needed) and presents a modal dialog for editing
 * or initializing arc parameters: radius, start angle, and sweep angle.
 *
 * If the dialog has not yet been created for the given GschemToplevel, it is
 * initialized with labeled spin buttons for user input. These widgets are stored
 * as `GObject` data keys on the dialog window so they can be retrieved during
 * the response callback. If the dialog already exists, it is simply brought
 * to the foreground and its input fields are updated based on `arc_object`.
 *
 * If `arc_object` is non-NULL, its arc parameters are used to populate the dialog.
 * Otherwise, default values are provided based on the current working context
 * (`w_current->distance` for the radius, 0° start angle, and 90° sweep).
 *
 * \param w_current Pointer to the current GschemToplevel context.
 * \param arc_object The arc OBJECT being edited, or NULL to define a new arc.
 */
void arc_angle_dialog(GschemToplevel *w_current, OBJECT *arc_object)
{
  GtkWidget *label[3];
  GtkWidget *box, *vbox, *table;
  GtkWidget *widget[3];

  if (!w_current->aawindow)
  {
    w_current->aawindow = gschem_dialog_new_with_buttons(
        _("Arc Params"), GTK_WINDOW(w_current->main_window), GTK_DIALOG_MODAL,
        "arc-angle", w_current, _("Cancel"), GTK_RESPONSE_REJECT, _("OK"), GTK_RESPONSE_ACCEPT, NULL);

    gtk_window_set_position(GTK_WINDOW(w_current->aawindow), GTK_WIN_POS_MOUSE);

    g_signal_connect(w_current->aawindow, "response",
                     G_CALLBACK(arc_angle_dialog_response), w_current);

    gtk_dialog_set_default_response(GTK_DIALOG(w_current->aawindow), GTK_RESPONSE_ACCEPT);
    gtk_container_set_border_width(GTK_CONTAINER(w_current->aawindow), DIALOG_BORDER_SPACING);

    box = gtk_dialog_get_content_area(GTK_DIALOG(w_current->aawindow));
    gtk_box_set_spacing(GTK_BOX(box), DIALOG_V_SPACING);

    // Replace deprecated GtkAlignment with a GtkBox
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(box), vbox, FALSE, FALSE, 0);

    label[0] = gschem_dialog_misc_create_property_label(_("Arc Radius:"));
    label[1] = gschem_dialog_misc_create_property_label(_("Start Angle:"));
    label[2] = gschem_dialog_misc_create_property_label(_("Degrees of Sweep:"));

    widget[0] = gtk_spin_button_new_with_range(1, 100000, 100);
    widget[1] = gtk_spin_button_new_with_range(-360, 360, 1);
    widget[2] = gtk_spin_button_new_with_range(-360, 360, 1);

    for (int i = 0; i < 3; ++i)
      gtk_entry_set_activates_default(GTK_ENTRY(widget[i]), TRUE);

    table = gschem_dialog_misc_create_property_table(label, widget, 3);
    gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 0);

    // Replace GLADE_HOOKUP_OBJECT with simple g_object_set_data
    g_object_set_data(G_OBJECT(w_current->aawindow), "radius", widget[0]);
    g_object_set_data(G_OBJECT(w_current->aawindow), "spin_start", widget[1]);
    g_object_set_data(G_OBJECT(w_current->aawindow), "spin_sweep", widget[2]);
    g_object_set_data(G_OBJECT(w_current->aawindow), "arc_object", arc_object);

    gtk_widget_show_all(w_current->aawindow);
  }
  else
  {
    gtk_window_present(GTK_WINDOW(w_current->aawindow));
    widget[0] = g_object_get_data(G_OBJECT(w_current->aawindow), "radius");
    widget[1] = g_object_get_data(G_OBJECT(w_current->aawindow), "spin_start");
    widget[2] = g_object_get_data(G_OBJECT(w_current->aawindow), "spin_sweep");
  }

  if (arc_object == NULL)
  {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget[0]), w_current->distance);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget[1]), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget[2]), 90);
  }
  else
  {
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget[0]), arc_object->arc->radius);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget[1]), arc_object->arc->start_angle);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget[2]), arc_object->arc->sweep_angle);
  }

  gtk_widget_grab_focus(widget[0]);
}

/***************** End of Arc dialog box *****************************/
