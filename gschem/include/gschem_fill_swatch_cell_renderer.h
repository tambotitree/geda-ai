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
 * \file gschem_fill_swatch_cell_renderer.h
 *
 * \brief A cell renderer for fill swatches.
 */

#define GSCHEM_TYPE_FILL_SWATCH_CELL_RENDERER           (gschem_fill_swatch_cell_renderer_get_type())
#define GSCHEM_FILL_SWATCH_CELL_RENDERER(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSCHEM_TYPE_FILL_SWATCH_CELL_RENDERER, GschemFillSwatchCellRenderer))
#define GSCHEM_FILL_SWATCH_CELL_RENDERER_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass),  GSCHEM_TYPE_FILL_SWATCH_CELL_RENDERER, GschemFillSwatchCellRendererClass))
#define IS_GSCHEM_FILL_SWATCH_CELL_RENDERER(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSCHEM_TYPE_FILL_SWATCH_CELL_RENDERER))

typedef struct _GschemFillSwatchCellRendererClass GschemFillSwatchCellRendererClass;
typedef struct _GschemFillSwatchCellRenderer GschemFillSwatchCellRenderer;

struct _GschemFillSwatchCellRendererClass
{
  GtkCellRendererTextClass parent_class;
};

struct _GschemFillSwatchCellRenderer
{
  GtkCellRendererText parent;

  gboolean enabled;
  OBJECT_FILLING fill_type;
};

GType
gschem_fill_swatch_cell_renderer_get_type ();

GtkCellRenderer*
gschem_fill_swatch_cell_renderer_new ();
