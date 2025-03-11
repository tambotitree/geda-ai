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


#ifndef __GSCHEM_DIALOG_H__
#define __GSCHEM_DIALOG_H__


#define GSCHEM_TYPE_DIALOG           (gschem_dialog_get_type())
#define GSCHEM_DIALOG(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSCHEM_TYPE_DIALOG, GschemDialog))
#define GSCHEM_DIALOG_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass),  GSCHEM_TYPE_DIALOG, GschemDialogClass))
#define GSCHEM_IS_DIALOG(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSCHEM_TYPE_DIALOG))
#define GSCHEM_DIALOG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),  GSCHEM_TYPE_DIALOG, GschemDialogClass))

typedef struct _GschemDialogClass GschemDialogClass;
typedef struct _GschemDialog      GschemDialog;


struct _GschemDialogClass {
  GtkDialogClass parent_class;

  void (*geometry_save)    (GschemDialog *dialog,
                            EdaConfig *cfg,
                            gchar *group_name);
  void (*geometry_restore) (GschemDialog *dialog,
                            EdaConfig *cfg,
                            gchar *group_name);
};

struct _GschemDialog {
  GtkDialog parent_instance;

  gchar *settings_name;
  GschemToplevel *w_current;
};


GType gschem_dialog_get_type (void);

GtkWidget* gschem_dialog_new_with_buttons (const gchar *title, GtkWindow *parent, GtkDialogFlags flags,
                                           const gchar *settings_name, GschemToplevel *w_current,
                                           const gchar *first_button_text, ...);


#endif /* __GSCHEM_DIALOG_H__ */
