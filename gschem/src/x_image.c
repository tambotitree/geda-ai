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
#include <unistd.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <glib.h>

#include "gschem.h"

#define X_IMAGE_DEFAULT_SIZE "800x600"

#define X_IMAGE_SIZE_MENU_NAME "image_size_menu"
#define X_IMAGE_TYPE_MENU_NAME "image_type_menu"

#define X_IMAGE_DEFAULT_TYPE "PNG"

static char *x_image_sizes[] = {"320x240", "640x480", "800x600", "1200x768",
  "1280x960", "1600x1200", "3200x2400", NULL};

/*! \brief Create the options of the image size combobox
 *  \par This function adds the options of the image size to the given combobox.
 *  \param combo [in] the combobox to add the options to.
 *  \return nothing
 *  \note
 *  This function is only used in this file, there are other create_menus...
 */
static void create_size_menu (GtkComboBox *combo)
{
  char *buf;
  char *default_size;
  int i, default_index = 0;

  default_size = g_strdup (X_IMAGE_DEFAULT_SIZE);
  for (i=0; x_image_sizes[i] != NULL;i++) {
    /* Create a new string and add it as an option*/
    buf = g_strdup (x_image_sizes[i]);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), buf);

    /* Compare with the default size, to get the default index */
    if (strcasecmp(buf, default_size ) == 0) {
      default_index = i;
    }
    g_free(buf);
  }
  g_free(default_size);

  /* Set the default menu */
  gtk_combo_box_set_active(GTK_COMBO_BOX (combo), default_index);

  return;
}

/*! \brief Create the options of the image type combobox
 *  \par This function adds the options of the image type to the given combobox.
 *  \param combo [in] the combobox to add the options to.
 *  \return nothing
 *  \note
 *  This function is only used in this file, there are other create_menus...
 */
static void create_type_menu(GtkComboBox *combo)
{
  GSList *formats = gdk_pixbuf_get_formats ();
  GSList *ptr;
  char *buf;
  int i=0, default_index=0;

  ptr = formats;
  while (ptr) {
    if (gdk_pixbuf_format_is_writable (ptr->data)) {
      /* Get the format description and add it to the menu */
      buf = g_strdup (gdk_pixbuf_format_get_description(ptr->data));
      gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), buf);

      /* Compare the name with "png" and store the index */
      buf = g_strdup (gdk_pixbuf_format_get_name(ptr->data));
      if (strcasecmp(buf, X_IMAGE_DEFAULT_TYPE) == 0) {
        default_index = i;
      }
      i++;  /* this is the count of items added to the combo box */
      /* not the total number of pixbuf formats */
      g_free(buf);
    }
    ptr = ptr->next;
  }
  g_slist_free (formats);
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Portable Document Format");

  /* Set the default menu */
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo), default_index);
  return;
}

/*! \brief Given a gdk-pixbuf image type description, it returns the type,
 *  or extension of the image.
 *  \par Return the gdk-pixbuf image type, or extension, which has the
 *  given gdk-pixbuf description.
 *  \param description The gdk-pixbuf image type description.
 *  \return The gdk-pixbuf type, or extension, of the image.
 *  \note This function is only used in this file.
 */
static const char *
x_image_get_type_from_description (const char *description)
{
  GSList *ptr;

  if (strcmp (description, "Portable Document Format") == 0) {
    return "pdf";
  }

  ptr = gdk_pixbuf_get_formats ();

  while (ptr != NULL) {
    gchar *ptr_descr = gdk_pixbuf_format_get_description (ptr->data);

    if (ptr_descr && (strcasecmp (ptr_descr, description) == 0)) {
      return gdk_pixbuf_format_get_name (ptr->data);
    }

    ptr = g_slist_next (ptr);
  }

  return NULL;
}

/*! \brief Update the filename of a file dialog, when the image type has changed.
 *  \par Given a combobox inside a file chooser dialog, this function updates
 *  the filename displayed by the dialog, removing the current extension, and
 *  adding the extension of the image type selected.
 *  \param combo     [in] A combobox inside a file chooser dialog, with gdk-pixbuf image type descriptions.
 *  \param w_current [in] the GschemToplevel structure.
 *  \return nothing.
 *
 */
static void x_image_update_dialog_filename(GtkComboBox *combo,
    GschemToplevel *w_current) {
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  char* image_type_descr = NULL;
  char *image_type = NULL;
  char *old_image_filename = NULL;
  char *file_basename = NULL;
  char *file_name = NULL ;
  char *new_image_filename = NULL;
  GtkWidget *file_chooser;

  /* Get the current image type */
  // image_type_descr = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
  image_type_descr = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
  if (image_type_descr != NULL) {
    image_type = x_image_get_type_from_description(image_type_descr);
    g_free(image_type_descr);  // free ONLY what GTK allocated
  }
  image_type = x_image_get_type_from_description(image_type_descr);

  /* Get the parent dialog */
  file_chooser = gtk_widget_get_ancestor(GTK_WIDGET(combo),
      GTK_TYPE_FILE_CHOOSER);

  /* Get the previous file name. If none, revert to the page filename */
  old_image_filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
  if (!old_image_filename) {
    old_image_filename = toplevel->page_current->page_filename;
  }

  /* Get the file name, without extension */
  if (old_image_filename) {
    file_basename = g_path_get_basename(old_image_filename);

    if (g_strrstr(file_basename, ".") != NULL) {
      file_name = g_strndup(file_basename,
          g_strrstr(file_basename, ".") - file_basename);
    }
  }

  /* Add the extension */
  if (file_name) {
    new_image_filename = g_strdup_printf("%s.%s", file_name,
        image_type);
  } else {
    new_image_filename = g_strdup_printf("%s.%s", file_basename,
        image_type);
  }

  /* Set the new filename */
  if (file_chooser) {
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(file_chooser),
        new_image_filename);
  } else {
    s_log_message("x_image_update_dialog_filename: No parent file chooser found!.\n");
  }

  g_free(file_name);
  g_free(file_basename);
  g_free(new_image_filename);
}

/*! \brief Write the image file, with the desired options.
 *  \par This function writes the image file, with the options set in the
 *  dialog by the user.
 *  \param w_current [in] the GschemToplevel structure.
 *  \param filename  [in] the image filename.
 *  \param width     [in] the image width chosen by the user.
 *  \param height    [in] the image height chosen by the user.
 *  \param filetype  [in] image filetype.
 *  \return nothing
 *
 */
void x_image_lowlevel(GschemToplevel *w_current, const char* filename,
    int width, int height, char *filetype)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  int save_page_left, save_page_right, save_page_top, save_page_bottom;
  int page_width, page_height, page_center_left, page_center_top;
  GdkPixbuf *pixbuf;
  GError *gerror = NULL;
  GtkWidget *dialog;
  float prop;
  GschemPageView *view = gschem_toplevel_get_current_page_view (w_current);

  GschemPageGeometry *geometry = gschem_page_view_get_page_geometry (view);
  g_return_if_fail (geometry != NULL);

  /* Save geometry */
  save_page_left = geometry->viewport_left;
  save_page_right = geometry->viewport_right;
  save_page_top = geometry->viewport_top;
  save_page_bottom = geometry->viewport_bottom;

  page_width = geometry->viewport_right - geometry->viewport_left;
  page_height = geometry->viewport_bottom - geometry->viewport_top;

  page_center_left = geometry->viewport_left + (page_width / 2);
  page_center_top = geometry->viewport_top + (page_height / 2);

  /* Preserve proportions */
  prop = (float)width / height;
  if (((float)page_width / page_height) > prop) {
    page_height = (page_width / prop);
  }else{
    page_width = (page_height * prop);
  }

  gschem_page_geometry_set_viewport_left   (geometry, page_center_left - (page_width / 2));
  gschem_page_geometry_set_viewport_right  (geometry, page_center_left + (page_width / 2));
  gschem_page_geometry_set_viewport_top    (geometry, page_center_top - (page_height / 2));
  gschem_page_geometry_set_viewport_bottom (geometry, page_center_top + (page_height / 2));

  /* de select everything first */
  o_select_unselect_all( w_current );

  if (strcmp(filetype, "pdf") == 0)
    x_print_export_pdf (w_current, filename);
  else {
    pixbuf = x_image_get_pixbuf(w_current, width, height);
    if (pixbuf != NULL) {
      if (!gdk_pixbuf_save(pixbuf, filename, filetype, &gerror, NULL)) {
        s_log_message(_("x_image_lowlevel: Unable to write %s file %s.\n"),
            filetype, filename);
        s_log_message("%s", gerror->message);

        /* Warn the user */
        dialog = gtk_message_dialog_new (GTK_WINDOW(w_current->main_window),
            GTK_DIALOG_MODAL
            | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            _("There was the following error when saving image with type %s to filename:\n%s\n\n%s.\n"),
            filetype, filename, gerror->message
            );

        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

        /* Free the gerror */
        g_error_free(gerror);
        gerror = NULL;

        /* Unlink the output file */
        /* It's not safe to unlink the file if there was an error.
           For example: if the operation was not allowed due to permissions,
           the _previous existing_ file will be removed */
        /* unlink(filename); */
      }
      else {
        if (toplevel->image_color == TRUE) {
          s_log_message(_("Wrote color image to [%s] [%d x %d]\n"), filename, width, height);
        } else {
          s_log_message(_("Wrote black and white image to [%s] [%d x %d]\n"), filename, width, height);
        }
      }
      g_free(filetype);
      if (pixbuf != NULL)
        g_object_unref(pixbuf);
    }
    else {
      s_log_message(_("x_image_lowlevel: Unable to get pixbuf from gschem's window.\n"));
    }
  }

  /* Restore geometry */
  gschem_page_geometry_set_viewport_left   (geometry, save_page_left  );
  gschem_page_geometry_set_viewport_right  (geometry, save_page_right );
  gschem_page_geometry_set_viewport_top    (geometry, save_page_top   );
  gschem_page_geometry_set_viewport_bottom (geometry, save_page_bottom);

  gschem_page_view_invalidate_all (view);
}

/*! \brief Display the image file selection dialog.
 *  \par Display the image file selection dialog, allowing the user to
 *  set several options, like image size and image type.
 *  When the user hits "ok", then it writes the image file.
 *  \param w_current [in] the GschemToplevel structure.
 *  \return nothing
 */
void x_image_setup(GschemToplevel *w_current)
{
  GtkWidget *dialog;
  GtkWidget *hbox;
  GtkWidget *vbox1, *label1, *size_combo;
  GtkWidget *vbox2, *label2, *type_combo;
  char *image_size = NULL;
  char *image_type_descr = NULL;
  char *image_type = NULL;
  char *filename = NULL;
  int width = 0, height = 0;

  /* Create layout for dialog options */
  hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

  /* Image size selection */
  vbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  label1 = gtk_label_new(_("Width x Height"));
  gtk_widget_set_halign(label1, GTK_ALIGN_START);
  gtk_widget_set_valign(label1, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(vbox1), label1, FALSE, FALSE, 0);

  size_combo = gtk_combo_box_text_new();
  create_size_menu(GTK_COMBO_BOX(size_combo));
  gtk_box_pack_start(GTK_BOX(vbox1), size_combo, TRUE, TRUE, 0);

  /* Image type selection */
  vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  label2 = gtk_label_new(_("Image type"));
  gtk_widget_set_halign(label2, GTK_ALIGN_START);
  gtk_widget_set_valign(label2, GTK_ALIGN_START);
  gtk_box_pack_start(GTK_BOX(vbox2), label2, FALSE, FALSE, 0);

  type_combo = gtk_combo_box_text_new();
  create_type_menu(GTK_COMBO_BOX(type_combo));
  gtk_box_pack_start(GTK_BOX(vbox2), type_combo, TRUE, TRUE, 0);

  /* Pack both option areas into hbox */
  gtk_box_pack_start(GTK_BOX(hbox), vbox1, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(hbox), vbox2, FALSE, FALSE, 10);

  /* Connect image type change to filename updater */
  g_signal_connect(type_combo, "changed",
                   G_CALLBACK(x_image_update_dialog_filename),
                   w_current);

  /* Create file chooser dialog */
  dialog = gtk_file_chooser_dialog_new(_("Write image..."),
                                       GTK_WINDOW(w_current->main_window),
                                       GTK_FILE_CHOOSER_ACTION_SAVE,
                                       _("_Cancel"), GTK_RESPONSE_CANCEL,
                                       _("_Save"), GTK_RESPONSE_ACCEPT,
                                       NULL);

  gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(dialog), hbox);
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);
  gtk_container_set_border_width(GTK_CONTAINER(dialog), DIALOG_BORDER_SPACING);
  gtk_box_set_spacing(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
                      DIALOG_V_SPACING);

  x_image_update_dialog_filename(GTK_COMBO_BOX(type_combo), w_current);
  gtk_widget_show_all(dialog);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    image_size = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(size_combo));
    image_type_descr = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(type_combo));

    if (image_size && image_type_descr) {
      image_type = x_image_get_type_from_description(image_type_descr);
      if (sscanf(image_size, "%ix%i", &width, &height) == 2) {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename) {
          x_image_lowlevel(w_current, filename, width, height, image_type);
          g_free(filename);
        }
      }
      g_free(image_type);
    }

    g_free(image_size);
    g_free(image_type_descr);
  }

  gtk_widget_destroy(dialog);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
static void x_image_convert_to_greyscale(GdkPixbuf *pixbuf)
{
  int width, height, rowstride, n_channels;
  guchar *pixels, *p, new_value;
  int i, j;

  n_channels = gdk_pixbuf_get_n_channels (pixbuf);

  if (n_channels < 3)
    return;

  if (gdk_pixbuf_get_colorspace (pixbuf) != GDK_COLORSPACE_RGB)
    return;

  if (gdk_pixbuf_get_bits_per_sample (pixbuf) != 8)
    return;

  width = gdk_pixbuf_get_width (pixbuf);
  height = gdk_pixbuf_get_height (pixbuf);

  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  pixels = gdk_pixbuf_get_pixels (pixbuf);

  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      p = pixels + j * rowstride + i * n_channels;

      new_value = 0.3 * p[0] + 0.59 * p[1] + 0.11 * p[2];
      p[0] = new_value;
      p[1] = new_value;
      p[2] = new_value;
      // Leave alpha channel (p[3]) unchanged, if present
    }
  }
}
/*! \brief Renders the current page to a GdkPixbuf.
 *  \par Function Description
 *  This function creates an offscreen Cairo surface and draws the current schematic
 *  page into it using o_redraw_rect(). It then extracts a GdkPixbuf for export or use elsewhere.
 */
GdkPixbuf *
x_image_get_pixbuf(GschemToplevel *w_current, int width, int height)
{
  GdkPixbuf *pixbuf = NULL;
  cairo_surface_t *surface = NULL;
  cairo_t *cr = NULL;
  GschemPageView *page_view;
  GtkWidget *widget;
  GdkRectangle rect;
  int origin_x = 0, origin_y = 0;
  GschemPageGeometry *old_geometry, *new_geometry;
  GschemToplevel new_w_current;
  GschemOptions options;
  TOPLEVEL toplevel;

  page_view = gschem_toplevel_get_current_page_view(w_current);
  widget = GTK_WIDGET(page_view);

  old_geometry = gschem_page_view_get_page_geometry(page_view);

  // Clone working context
  memcpy(&new_w_current, w_current, sizeof(GschemToplevel));
  memcpy(&options, w_current->options, sizeof(GschemOptions));
  memcpy(&toplevel, w_current->toplevel, sizeof(TOPLEVEL));
  new_w_current.options = &options;
  new_w_current.toplevel = &toplevel;

  gschem_options_set_grid_mode(new_w_current.options, GRID_MODE_NONE);

  rect.x = origin_x;
  rect.y = origin_y;
  rect.width = width;
  rect.height = height;

  // Create offscreen surface
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
    fprintf(stderr, "Failed to create Cairo surface\n");
    return NULL;
  }

  cr = cairo_create(surface);
  if (cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
    fprintf(stderr, "Failed to create Cairo context\n");
    cairo_surface_destroy(surface);
    return NULL;
  }

  // Construct geometry for this rendering target
  new_geometry = gschem_page_geometry_new_with_values(
    width,
    height,
    old_geometry->viewport_left,
    old_geometry->viewport_top,
    old_geometry->viewport_right,
    old_geometry->viewport_bottom,
    toplevel.init_left,
    toplevel.init_top,
    toplevel.init_right,
    toplevel.init_bottom
  );

  // Redraw the new geometry into Cairo context
  o_redraw_rect(&new_w_current, cr, toplevel.page_current, new_geometry, &rect);

  // Clean up geometry
  gschem_page_geometry_free(new_geometry);
  cairo_destroy(cr);

  // Convert surface to GdkPixbuf
  pixbuf = gdk_pixbuf_get_from_surface(surface, 0, 0, width, height);
  cairo_surface_destroy(surface);

  // Optional grayscale conversion
  if (pixbuf != NULL && !toplevel.image_color) {
    x_image_convert_to_greyscale(pixbuf);
  }

  return pixbuf;
}