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

/*! \file gattrib_bom_hook.c
 *  \brief Write supplier data into the component attribute matrix.
 */

#include <string.h>

#include <libgeda/libgeda.h>

#include "../include/struct.h"
#include "../include/globals.h"
#include "gattrib_sheet.h"
#include "gattrib_bom_hook.h"
#include "../include/prototype.h"

/* The attributes this hook owns.  These are deliberately the plain gEDA
 * attribute names rather than anything distributor-specific: a filled-in
 * schematic should still mean something to a tool that has never heard of
 * Nexar. */
#define BOM_ATTRIB_MPN          "mpn"
#define BOM_ATTRIB_MANUFACTURER "manufacturer"
#define BOM_ATTRIB_PRICE        "price"
#define BOM_ATTRIB_DATASHEET    "datasheet"
#define BOM_ATTRIB_STATUS       "lifecycle"

const gchar *gattrib_bom_hook_search_columns[] = {
  "mpn", "part_number", "value", NULL
};

/*! Default column creator: the real gattrib one. */
static void
default_add_column (const gchar *name)
{
  s_toplevel_add_new_attrib ((gchar *) name);
}

/*! Default design-modified notifier. */
static void
default_touch (void)
{
  if (sheet_head != NULL) {
    sheet_head->CHANGED = TRUE;   /* prompt to save on exit */
  }
}

static GattribBomHookAddColumnFunc add_column_func = default_add_column;

static GattribBomHookTouchFunc touch_func = default_touch;

void
gattrib_bom_hook_set_add_column_func (GattribBomHookAddColumnFunc func)
{
  add_column_func = (func != NULL) ? func : default_add_column;
}

void
gattrib_bom_hook_set_touch_func (GattribBomHookTouchFunc func)
{
  touch_func = (func != NULL) ? func : default_touch;
}

void
gattrib_bom_hook_stats_clear (GattribBomHookStats *stats)
{
  if (stats == NULL) {
    return;
  }
  g_free (stats->first_error);
  memset (stats, 0, sizeof *stats);
}

/*! Find the index of attribute column \a name.
 *
 *  \return The column index, or -1.  Comparison is case-insensitive because
 *  the attribute list is sorted by name and users type "MPN" and "mpn"
 *  interchangeably.
 */
static gint
find_column (GattribSheet *sheet, const gchar *name)
{
  gint n_cols, c;

  n_cols = gattrib_sheet_get_n_cols (sheet);
  for (c = 0; c < n_cols; c++) {
    const gchar *label = gattrib_sheet_get_column_label (sheet, c);
    if (label != NULL && g_ascii_strcasecmp (label, name) == 0) {
      return c;
    }
  }
  return -1;
}

/*! Get attribute column \a name, creating it if it does not exist.
 *
 *  Creating a column goes through \a add_column_func so that the internal
 *  attribute table and the widget stay in step; see the header.
 *
 *  \return The column index, or -1 on failure.
 */
static gint
ensure_column (GattribSheet *sheet, const gchar *name)
{
  gint col = find_column (sheet, name);

  if (col < 0) {
    add_column_func (name);
    col = find_column (sheet, name);
  }
  return col;
}

/*! Write \a value into cell (\a row, \a col), honouring \a overwrite.
 *
 *  \return TRUE if the cell was written.
 */
static gboolean
store_cell (GattribSheet *sheet, gint row, gint col, const gchar *value,
            gboolean overwrite)
{
  const gchar *old;

  if (value == NULL || value[0] == '\0') {
    return FALSE;
  }

  old = gattrib_sheet_get_cell_text (sheet, row, col);
  if (!overwrite && old != NULL && old[0] != '\0') {
    return FALSE;
  }

  gattrib_sheet_set_cell (sheet, row, col, value, BLUE);
  return TRUE;
}

/*! Look up the part number for \a row.
 *
 *  \return A newly-allocated part number, or NULL if the row has none.
 */
static gchar *
row_part_number (GattribSheet *sheet, gint row)
{
  gint i;

  for (i = 0; gattrib_bom_hook_search_columns[i] != NULL; i++) {
    gint col = find_column (sheet, gattrib_bom_hook_search_columns[i]);
    const gchar *text;

    if (col < 0) {
      continue;
    }
    text = gattrib_sheet_get_cell_text (sheet, row, col);
    if (text != NULL) {
      /* Values are user-typed, so allow surrounding whitespace. */
      gchar *trimmed = g_strstrip (g_strdup (text));
      if (trimmed[0] != '\0') {
        return trimmed;
      }
      g_free (trimmed);
    }
  }
  return NULL;
}

gboolean
gattrib_bom_hook_enrich_sheet (GattribSheet *sheet,
                               GattribBomSupplier *supplier,
                               gboolean overwrite,
                               GattribBomHookStats *stats,
                               GError **error)
{
  GattribBomHookStats local = { 0 };
  gint n_rows, row, c_mpn, c_mfr, c_price, c_ds, c_status;
  gboolean updated = FALSE;

  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (stats == NULL) {
    stats = &local;
  }
  memset (stats, 0, sizeof *stats);

  if (sheet == NULL || supplier == NULL) {
    g_set_error_literal (error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                         "no sheet or supplier to enrich");
    if (stats == &local) {
      gattrib_bom_hook_stats_clear (&local);
    }
    return FALSE;
  }

  /* Resolve every column once, before touching any cells: creating a column
   * can renumber the others. */
  c_mpn = ensure_column (sheet, BOM_ATTRIB_MPN);
  c_mfr = ensure_column (sheet, BOM_ATTRIB_MANUFACTURER);
  c_price = ensure_column (sheet, BOM_ATTRIB_PRICE);
  c_ds = ensure_column (sheet, BOM_ATTRIB_DATASHEET);
  c_status = ensure_column (sheet, BOM_ATTRIB_STATUS);

  n_rows = gattrib_sheet_get_n_rows (sheet);
  for (row = 0; row < n_rows; row++) {
    GattribBomPartInfo *info;
    gchar *part;
    GError *row_error = NULL;
    gboolean row_updated = FALSE;
    gchar *price;

    stats->rows++;

    part = row_part_number (sheet, row);
    if (part == NULL) {
      continue;
    }
    stats->looked_up++;

    info = gattrib_bom_supplier_lookup (supplier, part, &row_error);
    g_free (part);

    if (info == NULL) {
      stats->failed++;
      if (stats->first_error == NULL) {
        /* Own a copy: row_error is freed every iteration. */
        stats->first_error = g_strdup (
          row_error != NULL ? row_error->message : "lookup failed");
      }
      g_clear_error (&row_error);
      continue;
    }

    if (!info->found) {
      gattrib_bom_part_info_free (info);
      continue;
    }
    stats->found++;

    row_updated |= store_cell (sheet, row, c_mpn, info->mpn, overwrite);
    row_updated |= store_cell (sheet, row, c_mfr, info->manufacturer,
                               overwrite);
    row_updated |= store_cell (sheet, row, c_ds, info->datasheet_url,
                               overwrite);
    row_updated |= store_cell (sheet, row, c_status, info->lifecycle,
                               overwrite);

    price = gattrib_bom_format_best_offer (info);
    row_updated |= store_cell (sheet, row, c_price, price, overwrite);
    g_free (price);

    gattrib_bom_part_info_free (info);

    if (row_updated) {
      stats->updated++;
      updated = TRUE;
    }
  }

  if (updated) {
    /* Prompt to save on exit, exactly as a hand edit would. */
    touch_func ();
  }

  if (stats == &local) {
    gattrib_bom_hook_stats_clear (&local);
  }
  return updated;
}

gboolean
gattrib_bom_hook_enrich_with_default_supplier (GattribSheet *sheet,
                                               gboolean overwrite,
                                               GattribBomHookStats *stats,
                                               GError **error)
{
  GattribBomSupplier *supplier;
  GattribBomConfig *config;
  gboolean result;

  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  config = gattrib_bom_config_new ();

  if (g_getenv ("GATTRIB_BOM_OFFLINE") != NULL ||
      !gattrib_bom_config_is_configured (config)) {
    supplier = gattrib_bom_supplier_new_offline ();
  } else {
    supplier = gattrib_bom_supplier_new_octopart (config, NULL, NULL);
  }
  gattrib_bom_config_free (config);

  if (supplier == NULL) {
    g_set_error_literal (error, G_FILE_ERROR, G_FILE_ERROR_NOMEM,
                         "could not create a supplier");
    return FALSE;
  }

  result = gattrib_bom_hook_enrich_sheet (sheet, supplier, overwrite, stats,
                                          error);
  gattrib_bom_supplier_free (supplier);
  return result;
}
