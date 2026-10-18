/* gEDA - GPL Electronic Design Automation
 * gattrib -- gEDA component and net attribute manipulation using spreadsheet.
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

/*! \file gattrib_bom_hook.h
 *  \brief Fill component attribute columns from a supplier lookup.
 *
 *  This is the glue between the supplier module (gattrib_bom.h) and the
 *  attribute matrix (gattrib_sheet.h).  It knows which column holds the part
 *  number, which columns to create or overwrite, and how to touch the
 *  gattrib data structures so the change is saved with the schematic.
 *
 *  Keeping it out of x_window.c means the interesting part -- deciding which
 *  attribute goes where -- is testable without a display, using the offline
 *  supplier.
 */

#ifndef GATTRIB_BOM_HOOK_H
#define GATTRIB_BOM_HOOK_H

#include <glib.h>

#include "gattrib_bom.h"
#include "gattrib_sheet.h"

G_BEGIN_DECLS

/*! \brief Column searched, in order, for the part number to look up.
 *
 *  The first of these that has a value in a given row is used; a row with
 *  none of them is skipped.  Exposed so the UI can tell the user. */
extern const gchar *gattrib_bom_hook_search_columns[];

/*! \brief Summary of what one enrichment run did. */
typedef struct {
  gint rows;        /*!< Rows considered. */
  gint looked_up;   /*!< Rows that had a part number. */
  gint found;       /*!< Rows the supplier knew about. */
  gint updated;     /*!< Rows whose cells were written. */
  gint failed;      /*!< Rows whose lookup returned an error. */
  gchar *first_error; /*!< Message from the first failure, or NULL. */
} GattribBomHookStats;

/*! \brief Annotate the component sheet with supplier data.
 *
 *  For every row of \a sheet, takes the part number from the first of
 *  gattrib_bom_hook_search_columns[] that is non-empty and asks \a supplier
 *  about it.  When a part is found, the attributes mpn, manufacturer, price
 *  and datasheet are created if absent and their cells set.  Existing values
 *  are only overwritten when \a overwrite is TRUE, so a designer's hand-typed
 *  price survives a refresh.
 *
 *  Columns are created through the same path as Edit->Add new attrib column,
 *  which keeps the internal attribute table in step with the widget.
 *
 *  \param sheet      The component sheet to annotate.
 *  \param supplier   Supplier to query; must not be NULL.
 *  \param overwrite  Whether to replace non-empty cells.
 *  \param stats      Filled in with a summary; may be NULL.
 *  \param error      Set if the sheet or supplier is missing.
 *  \return TRUE when at least one row was updated.
 */
gboolean gattrib_bom_hook_enrich_sheet (GattribSheet *sheet,
                                        GattribBomSupplier *supplier,
                                        gboolean overwrite,
                                        GattribBomHookStats *stats,
                                        GError **error);

/*! \brief Free the strings owned by \a stats and zero it. */
void gattrib_bom_hook_stats_clear (GattribBomHookStats *stats);

/*! \brief Callback used to create a missing attribute column.
 *
 *  \param name  Attribute name of the column to create.
 *
 *  Why this is a hook: the application creates columns through
 *  s_toplevel_add_new_attrib(), which updates the internal attribute table
 *  and the widget together, but pulling that in would drag most of gattrib
 *  (and its globals) into anything that wants to test this module.  The
 *  default implementation calls s_toplevel_add_new_attrib() when gattrib's
 *  globals are available; tests can supply their own.
 */
typedef void (*GattribBomHookAddColumnFunc) (const gchar *name);

/*! \brief Install (or clear, with NULL) the column-creation callback.
 *
 *  This is process-wide because gattrib has a single component sheet. */
void gattrib_bom_hook_set_add_column_func (GattribBomHookAddColumnFunc func);

/*! \brief Callback used to flag the design as modified.
 *
 *  Called once when at least one cell was written, so the user is prompted to
 *  save.  Injected for the same reason as the column creator: it keeps this
 *  module free of gattrib's globals so it can be tested on its own.  The
 *  default implementation sets sheet_head->CHANGED when libgeda is linked. */
typedef void (*GattribBomHookTouchFunc) (void);

/*! \brief Install (or clear, with NULL) the design-modified callback. */
void gattrib_bom_hook_set_touch_func (GattribBomHookTouchFunc func);

/*! \brief Choose a supplier from the environment and run the hook.
 *
 *  Uses the offline supplier when GATTRIB_BOM_OFFLINE is set or when no
 *  Nexar credentials are configured; otherwise the Octopart/Nexar client.
 *  Returns the outcome of gattrib_bom_hook_enrich_sheet(). */
gboolean gattrib_bom_hook_enrich_with_default_supplier (
    GattribSheet *sheet, gboolean overwrite, GattribBomHookStats *stats,
    GError **error);

G_END_DECLS

#endif /* GATTRIB_BOM_HOOK_H */
