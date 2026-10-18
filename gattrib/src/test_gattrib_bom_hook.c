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

/*! \file test_gattrib_bom_hook.c
 *  \brief Tests for writing supplier data into the attribute matrix.
 *
 *  The hook is the part of the BOM feature with real decisions in it -- which
 *  column is the part number, what may be overwritten -- so it is worth
 *  pinning down.  runs without a display via GtkOffscreen, and uses the
 *  offline supplier, so the tests are deterministic and need no network.
 */

#include <gtk/gtk.h>
#include <string.h>

#include <libgeda/libgeda.h>

#include "../include/struct.h"
#include "../include/globals.h"
#include "gattrib_bom_hook.h"
#include "gattrib_sheet.h"

static gint checks = 0;
static gint failures = 0;

#define CHECK(cond)                                                     \
  do {                                                                  \
    checks++;                                                           \
    if (!(cond)) {                                                      \
      failures++;                                                       \
      g_printerr ("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);       \
    }                                                                   \
  } while (0)

#define CHECK_STR(actual, expected)                                     \
  do {                                                                  \
    checks++;                                                           \
    const gchar *_a = (actual);                                         \
    if (!g_str_equal (_a != NULL ? _a : "", (expected))) {              \
      failures++;                                                       \
      g_printerr ("FAIL %s:%d: '%s' != '%s'\n", __FILE__, __LINE__,     \
                  _a != NULL ? _a : "(null)", (expected));              \
    }                                                                   \
  } while (0)

/*! The sheet the test column creator works on. */
static GattribSheet *test_current_sheet = NULL;

/*! Build a sheet shaped like the component table: a value column with one
 *  part number per row. */
static GattribSheet *
sheet_with_values (const gchar **values, gint n_values)
{
  GattribSheet *sheet = g_object_new (GATTRIB_TYPE_SHEET, NULL);
  gchar *labels[] = { "value", NULL };
  gchar **rows = g_new0 (gchar *, n_values + 1);
  gint i;

  /* GattribSheet is a GtkBox, so it arrives floating; take ownership. */
  g_object_ref_sink (sheet);

  rows[0] = g_strdup ("C1");        /* row labels, unused by the hook */
  for (i = 0; i < n_values; i++) {
    rows[i + 1] = g_strdup ("U1");
  }

  gattrib_sheet_set_row_labels (sheet, n_values + 1, rows);
  gattrib_sheet_set_column_labels (sheet, 1, labels);
  for (i = 0; i < n_values; i++) {
    gattrib_sheet_set_cell (sheet, i, 0, values[i], BLUE);
  }

  for (i = 0; i <= n_values; i++) {
    g_free (rows[i]);
  }
  g_free (rows);

  test_current_sheet = sheet;
  return sheet;
}

static gint touch_calls = 0;

/*! Records that the hook flagged the design as modified. */
static void
test_touch (void)
{
  touch_calls++;
}

/*! Test column creator: adds the column to the sheet directly.
 *
 *  In gattrib the real one also updates the attribute table; here the sheet
 *  is the only state, which is all the hook needs to see. */
static void
test_add_column (const gchar *name)
{
  gattrib_sheet_append_columns (test_current_sheet, 1, NULL);
  gattrib_sheet_set_column_label (test_current_sheet,
                                  gattrib_sheet_get_n_cols (test_current_sheet) - 1,
                                  name);
}

/*! Index of attribute column \a name, or -1. */
static gint
column_index (GattribSheet *sheet, const gchar *name)
{
  gint n = gattrib_sheet_get_n_cols (sheet), c;
  for (c = 0; c < n; c++) {
    const gchar *label = gattrib_sheet_get_column_label (sheet, c);
    if (label != NULL && g_ascii_strcasecmp (label, name) == 0) {
      return c;
    }
  }
  return -1;
}

static void
test_enrich_offline (void)
{
  const gchar *values[] = { "LM555CN", "nosuchpart" };
  GattribSheet *sheet = sheet_with_values (values, 2);
  GattribBomSupplier *supplier = gattrib_bom_supplier_new_offline ();
  GattribBomHookStats stats;
  GError *error = NULL;
  gint c_mpn, c_price;

  CHECK (gattrib_bom_hook_enrich_sheet (sheet, supplier, FALSE, &stats,
                                        &error));
  CHECK (error == NULL);

  /* The columns did not exist, so the hook should have created them. */
  c_mpn = column_index (sheet, "mpn");
  c_price = column_index (sheet, "price");
  CHECK (c_mpn >= 0);
  CHECK (c_price >= 0);
  CHECK (column_index (sheet, "manufacturer") >= 0);
  CHECK (column_index (sheet, "datasheet") >= 0);

  CHECK_STR (gattrib_sheet_get_cell_text (sheet, 0, c_mpn), "LM555CN");
  CHECK_STR (gattrib_sheet_get_cell_text (sheet, 0, c_price),
             "USD 0.4200 @1 (12500 in stock)");
  CHECK (column_index (sheet, "lifecycle") >= 0);
  CHECK_STR (gattrib_sheet_get_cell_text (
               sheet, 0, column_index (sheet, "lifecycle")), "Active");

  /* Row 1 was searched for and not found: its cells stay empty. */
  CHECK (gattrib_sheet_get_cell_text (sheet, 1, c_mpn) == NULL);
  CHECK (gattrib_sheet_get_cell_text (sheet, 1, c_price) == NULL);

  /* Row 2 has no part number at all, so it was never looked up. */
  CHECK (stats.rows == 3);
  CHECK (stats.looked_up == 2);
  CHECK (stats.found == 1);
  CHECK (stats.updated == 1);
  CHECK (stats.failed == 0);
  CHECK (stats.first_error == NULL);

  /* The design was flagged as modified so the user is asked to save. */
  CHECK (touch_calls == 1);

  gattrib_bom_hook_stats_clear (&stats);
  gattrib_bom_supplier_free (supplier);
  g_object_unref (sheet);
}

static void
test_enrich_preserves_existing (void)
{
  const gchar *values[] = { "LM555CN" };
  GattribSheet *sheet = sheet_with_values (values, 1);
  GattribBomSupplier *supplier = gattrib_bom_supplier_new_offline ();
  GattribBomHookStats stats;
  GError *error = NULL;
  gint c_price, c_mpn;

  /* The designer has already corrected this price by hand. */
  gattrib_sheet_append_columns (sheet, 1, NULL);
  gattrib_sheet_set_column_label (sheet,
                                  gattrib_sheet_get_n_cols (sheet) - 1,
                                  "price");
  c_price = gattrib_sheet_get_n_cols (sheet) - 1;
  gattrib_sheet_set_cell (sheet, 0, c_price, "0.10", BLUE);

  /* First run: the existing price must survive. */
  gattrib_bom_hook_enrich_sheet (sheet, supplier, FALSE, &stats, &error);
  CHECK (error == NULL);
  CHECK_STR (gattrib_sheet_get_cell_text (sheet, 0, c_price), "0.10");

  /* The rest was filled in. */
  c_mpn = column_index (sheet, "mpn");
  CHECK_STR (gattrib_sheet_get_cell_text (sheet, 0, c_mpn), "LM555CN");

  /* Second run with overwrite: now the supplier wins. */
  gattrib_bom_hook_enrich_sheet (sheet, supplier, TRUE, &stats, &error);
  CHECK_STR (gattrib_sheet_get_cell_text (sheet, 0, c_price),
             "USD 0.4200 @1 (12500 in stock)");

  gattrib_bom_hook_stats_clear (&stats);
  gattrib_bom_supplier_free (supplier);
  g_object_unref (sheet);
}

static void
test_enrich_prefers_mpn_column (void)
{
  const gchar *values[] = { "LM555CN" };
  GattribSheet *sheet = sheet_with_values (values, 1);
  GattribBomSupplier *supplier = gattrib_bom_supplier_new_offline ();
  GattribBomHookStats stats;
  GError *error = NULL;

  /* value says LM555CN, but an existing mpn column says NE555P.  The mpn
   * column is the more specific answer and must win. */
  gattrib_sheet_append_columns (sheet, 1, NULL);
  gattrib_sheet_set_column_label (sheet,
                                  gattrib_sheet_get_n_cols (sheet) - 1,
                                  "mpn");
  gattrib_sheet_set_cell (sheet, 0, gattrib_sheet_get_n_cols (sheet) - 1,
                          "NE555P", BLUE);
  gattrib_sheet_set_cell (sheet, 0, 0, "SOMETHINGELSE", BLUE);

  gattrib_bom_hook_enrich_sheet (sheet, supplier, FALSE, &stats, &error);
  CHECK (error == NULL);
  CHECK (stats.looked_up == 1);
  CHECK (stats.found == 1);

  gattrib_bom_hook_stats_clear (&stats);
  gattrib_bom_supplier_free (supplier);
  g_object_unref (sheet);
}

static void
test_enrich_bad_arguments (void)
{
  GattribBomSupplier *supplier = gattrib_bom_supplier_new_offline ();
  GError *error = NULL;

  CHECK (!gattrib_bom_hook_enrich_sheet (NULL, supplier, FALSE, NULL, &error));
  CHECK (error != NULL);
  g_clear_error (&error);

  error = NULL;
  CHECK (!gattrib_bom_hook_enrich_sheet (NULL, NULL, FALSE, NULL, &error));
  CHECK (error != NULL);
  g_clear_error (&error);

  gattrib_bom_supplier_free (supplier);

  /* A NULL stats pointer is allowed. */
  gattrib_bom_hook_stats_clear (NULL);
}

/*! Stand-in for gattrib's global-variable setter, which lives in globals.c
 *  and drags in the rest of gattrib.  The hook never calls it. */
int
i_vars_set (TOPLEVEL *toplevel)
{
  (void) toplevel;
  return 0;
}

/*! Stand-in for the real column creator.  The tests replace it via
 *  gattrib_bom_hook_set_add_column_func(); this exists only to satisfy the
 *  default, which is never called here. */
void
s_toplevel_add_new_attrib (gchar *new_attrib_name)
{
  (void) new_attrib_name;
}

int
main (int argc, char **argv)
{
  gint result;

  (void) argc;
  (void) argv;

  gtk_init (&argc, &argv);

  gattrib_bom_hook_set_add_column_func (test_add_column);
  gattrib_bom_hook_set_touch_func (test_touch);

  test_enrich_offline ();
  test_enrich_preserves_existing ();
  test_enrich_prefers_mpn_column ();
  test_enrich_bad_arguments ();

  g_print ("%d checks, %d failures\n", checks, failures);
  result = failures == 0 ? 0 : 1;
  return result;
}

