/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 2013 Ales Hvezda
 * Copyright (C) 2013-2020 gEDA Contributors (see ChangeLog for details)
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
 * \file gschem_fill_swatch_cell_renderer.c
 *
 * \brief A cell renderer for fill type swatches.
 */

#include <config.h>
#include "gschem.h"



/* Specifies the width of the border around the swatch, in pixels (double).
 */
#define SWATCH_BORDER_WIDTH (1.0)


/* Specifies the pitch of hatch and mesh lines, in pixels (integer).
 */
#define SWATCH_LINE_PITCH (7)


/* Specifies the width of hatch and mesh lines, in pixels (double).
 */
#define SWATCH_LINE_WIDTH (2.0)


enum
{
  PROP_0,
  PROP_ENABLED,
  PROP_FILL_TYPE
};



static void
class_init (GschemFillSwatchCellRendererClass *klass);

static void
get_property (GObject    *object,
              guint      param_id,
              GValue     *value,
              GParamSpec *pspec);

static void
instance_init (GschemFillSwatchCellRenderer *swatch);

static void
render (GtkCellRenderer      *cell,
        GdkWindow            *window,
        GtkWidget            *widget,
        GdkRectangle         *background_area,
        GdkRectangle         *cell_area,
        GdkRectangle         *expose_area,
        GtkCellRendererState flags);

static void
set_property (GObject      *object,
              guint        param_id,
              const GValue *value,
              GParamSpec   *pspec);



/*! \brief Get/register GschemFillSwatchCellRenderer type.
 */
GType
gschem_fill_swatch_cell_renderer_get_type ()
{
  static GType type = 0;

  if (type == 0) {
    static const GTypeInfo info = {
      sizeof (GschemFillSwatchCellRendererClass),
      NULL,                                         /* base_init */
      NULL,                                         /* base_finalize */
      (GClassInitFunc) class_init,
      NULL,                                         /* class_finalize */
      NULL,                                         /* class_data */
      sizeof (GschemFillSwatchCellRenderer),
      0,                                            /* n_preallocs */
      (GInstanceInitFunc) instance_init,
    };

    type = g_type_register_static (GTK_TYPE_CELL_RENDERER_TEXT,
                                   "GschemFillSwatchCellRenderer",
                                   &info,
                                   0);
  }

  return type;
}



/*! \brief Create a new GschemFillSwatchCellRenderer
 *
 *  \return The new cell renderer
 */
GtkCellRenderer*
gschem_fill_swatch_cell_renderer_new ()
{
  return g_object_new (GSCHEM_TYPE_FILL_SWATCH_CELL_RENDERER, NULL);
}



/*! \private
 *  \brief Initialize swatch cell renderer class
 *
 *  \param [in,out] klass The swatch cell renderer class
 */
static void
class_init (GschemFillSwatchCellRendererClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  klass->parent_class.parent_class.render = render;

  object_class->get_property = get_property;
  object_class->set_property = set_property;

  g_object_class_install_property (object_class,
                                   PROP_ENABLED,
                                   g_param_spec_boolean ("enabled",
                                                         "Swatch Enabled",
                                                         "Swatch Enabled",
                                                         TRUE,
                                                         G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_FILL_TYPE,
                                   g_param_spec_int ("fill-type",
                                                     "Fill type",
                                                     "Fill Type",
                                                     0,
                                                     10,
                                                     FILLING_HOLLOW,
                                                     G_PARAM_READWRITE));
}



/*! \private
 *  \brief Initialize fill swatch cell renderer instance
 *
 *  \param [in,out] renderer The fill swatch cell renderer
 */
static void
instance_init (GschemFillSwatchCellRenderer *swatch)
{
  swatch->enabled = TRUE;
  swatch->fill_type = FILLING_HOLLOW;
}



/*! \private
 *  \brief Get a property.
 *
 *  \param [in]  object   The object with the property
 *  \param [in]  param_id The id of the property
 *  \param [out] value    The value of the property
 *  \param [in]  pspec    The property param spec
 */
static void
get_property (GObject    *object,
              guint      param_id,
              GValue     *value,
              GParamSpec *pspec)
{
  GschemFillSwatchCellRenderer *swatch = GSCHEM_FILL_SWATCH_CELL_RENDERER (object);

  switch (param_id) {
    case PROP_ENABLED:
      g_value_set_boolean (value, swatch->enabled);
      break;

    case PROP_FILL_TYPE:
      g_value_set_int (value, swatch->fill_type);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
  }
}



/*! \private
 *  \brief Render the swatch into the cell
 *
 *  \param [in] cell this cell renderer
 *  \param [in] window the window to renter to
 *  \param [in] widget the widget owning the window
 *  \param [in] background_area entire cell area
 *  \param [in] cell_area area rendered normally
 *  \param [in] expose_area the area requiring update
 *  \param [in] flags
 */
static void
render (GtkCellRenderer      *cell,
        GdkWindow            *window,
        GtkWidget            *widget,
        GdkRectangle         *background_area,
        GdkRectangle         *cell_area,
        GdkRectangle         *expose_area,
        GtkCellRendererState flags)
{
  GschemFillSwatchCellRenderer *swatch = GSCHEM_FILL_SWATCH_CELL_RENDERER (cell);

  if (swatch->enabled) {
    GdkColor color;
    cairo_t *cr = gdk_cairo_create (window);
    double offset = SWATCH_BORDER_WIDTH / 2.0;
    gboolean success;

    if (expose_area) {
      gdk_cairo_rectangle (cr, expose_area);
      cairo_clip (cr);
    }

    /* Paint the swatch using the text color to match the user's desktop theme.
     */

    success = gtk_style_lookup_color (gtk_widget_get_style (widget),
                                      "text_color",
                                      &color);

    if (success) {
      cairo_set_source_rgb (cr,
                            color.red   / 65535.0,
                            color.green / 65535.0,
                            color.blue  / 65535.0);
    }

    cairo_move_to (cr,
                   (double) cell_area->x + offset,
                   (double) cell_area->y + offset);

    cairo_line_to (cr,
                   (double) cell_area->x + (double) cell_area->width - offset,
                   (double) cell_area->y + offset);

    cairo_line_to (cr,
                   (double) cell_area->x + (double) cell_area->width - offset,
                   (double) cell_area->y + (double) cell_area->height - offset);

    cairo_line_to (cr,
                   (double) cell_area->x + offset,
                   (double) cell_area->y + (double) cell_area->height - offset);

    cairo_close_path (cr);

    if ((swatch->fill_type == FILLING_HATCH) || (swatch->fill_type == FILLING_MESH)) {
      BOX box;
      int index;
      GArray *lines = g_array_new (FALSE, FALSE, sizeof (LINE));
      cairo_path_t *save_path = cairo_copy_path (cr);

      cairo_save (cr);
      cairo_clip (cr);

      box.lower_x = cell_area->x;
      box.lower_y = cell_area->y;
      box.upper_x = cell_area->x + cell_area->width;
      box.upper_y = cell_area->y + cell_area->height;

      m_hatch_box (&box, 135, SWATCH_LINE_PITCH, lines);

      if (swatch->fill_type == FILLING_MESH) {
        m_hatch_box (&box, 45, SWATCH_LINE_PITCH, lines);
      }

      for (index=0; index<lines->len; index++) {
        LINE *line = &g_array_index (lines, LINE, index);

        cairo_move_to (cr, line->x[0], line->y[0]);
        cairo_line_to (cr, line->x[1], line->y[1]);
      }

      g_array_free (lines, TRUE);

      cairo_set_line_width (cr, SWATCH_LINE_WIDTH);
      cairo_stroke (cr);
      cairo_restore (cr);

      cairo_append_path (cr, save_path);
      cairo_path_destroy (save_path);
    }

    if (swatch->fill_type == FILLING_FILL) {
      cairo_fill_preserve (cr);
    }

    cairo_set_line_width (cr, SWATCH_BORDER_WIDTH);
    cairo_stroke (cr);
    cairo_destroy (cr);
  }
}



/*! \private
 *  \brief Set a property.
 *
 *  \param [in,out] object   The object with the property
 *  \param [in]     param_id The id of the property
 *  \param [in]     value    The value of the property
 *  \param [in]     pspec    The property param spec
 */
static void
set_property (GObject      *object,
              guint        param_id,
              const GValue *value,
              GParamSpec   *pspec)
{
  GschemFillSwatchCellRenderer *swatch = GSCHEM_FILL_SWATCH_CELL_RENDERER (object);

  switch (param_id) {
    case PROP_ENABLED:
      swatch->enabled =  g_value_get_boolean (value);
      break;

    case PROP_FILL_TYPE:
      swatch->fill_type =  g_value_get_int (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
      break;
  }
}
