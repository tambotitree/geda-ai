/* Standalone smoke test for GattribSheet.
 *
 * Builds and runs without a display by using the broadway/offscreen path and
 * only exercising the model API, which does not require a mapped window.
 *
 *   gcc -o /tmp/test_sheet test_gattrib_sheet.c ../src/gattrib_sheet.c \
 *       $(pkg-config --cflags --libs gtk+-3.0)
 */

#include <gtk/gtk.h>
#include <string.h>

#include "gattrib_sheet.h"

#define RED   2
#define BLUE  4
#define GREY  7
#define BLACK 0

static int failures = 0;

static void
check (gboolean cond, const char *what)
{
  if (!cond) {
    g_printerr ("FAIL: %s\n", what);
    failures++;
  } else {
    g_print ("ok: %s\n", what);
  }
  fflush (stdout);
}

static void
check_str (const gchar *got, const gchar *want, const char *what)
{
  if (g_strcmp0 (got, want) != 0) {
    g_printerr ("FAIL: %s (got '%s', want '%s')\n", what,
                got ? got : "(null)", want ? want : "(null)");
    failures++;
  } else {
    g_print ("ok: %s\n", what);
  }
}

int
main (int argc, char **argv)
{
  GtkWidget *widget;
  GattribSheet *sheet;
  gchar *titles[] = { (gchar *) "value", (gchar *) "footprint", NULL };
  gchar *rows[] = { (gchar *) "R1", (gchar *) "R2", (gchar *) "C1", NULL };
  GattribSheetSelection sel;
  gint row, col;

  gtk_init (&argc, &argv);

  widget = gattrib_sheet_new (3, 2, "Components");
  sheet = GATTRIB_SHEET (widget);

  gattrib_sheet_set_column_labels (sheet, 2, titles);
  gattrib_sheet_set_row_labels (sheet, 3, rows);

  check (gattrib_sheet_get_n_rows (sheet) == 3, "row count");
  check (gattrib_sheet_get_n_cols (sheet) == 2, "column count");

  check_str (gattrib_sheet_get_row_label (sheet, 0), "R1", "row label 0");
  check_str (gattrib_sheet_get_row_label (sheet, 2), "C1", "row label 2");
  check_str (gattrib_sheet_get_column_label (sheet, 0), "value",
             "column label 0");
  check_str (gattrib_sheet_get_column_label (sheet, 1), "footprint",
             "column label 1");

  /* An empty cell reads as NULL, as gattrib_sheet.h documents and as the
   * BOM hook needs in order to tell "no value" from "value is empty". */
  check (gattrib_sheet_get_cell_text (sheet, 0, 0) == NULL, "empty cell");

  gattrib_sheet_set_cell (sheet, 0, 0, "10k", BLUE);
  gattrib_sheet_set_cell (sheet, 0, 1, "0603", BLACK);
  gattrib_sheet_set_cell (sheet, 1, 0, "4k7", RED);
  gattrib_sheet_set_cell (sheet, 2, 1, "0805", GREY);

  check_str (gattrib_sheet_get_cell_text (sheet, 0, 0), "10k", "cell 0,0");
  check_str (gattrib_sheet_get_cell_text (sheet, 0, 1), "0603", "cell 0,1");
  check_str (gattrib_sheet_get_cell_text (sheet, 1, 0), "4k7", "cell 1,0");
  check_str (gattrib_sheet_get_cell_text (sheet, 2, 1), "0805", "cell 2,1");

  /* Out-of-range access must not crash and must read as empty. */
  check (gattrib_sheet_get_cell_text (sheet, 99, 0) == NULL, "row out of range");
  check (gattrib_sheet_get_cell_text (sheet, 0, 99) == NULL, "col out of range");
  gattrib_sheet_set_cell (sheet, 99, 99, "x", RED);
  gattrib_sheet_set_cell_colour (sheet, 99, 99, RED);
  g_print ("ok: out-of-range writes ignored\n");

  /* Setting text must not clobber the colour, and vice versa. */
  gattrib_sheet_set_cell_colour (sheet, 0, 0, GREY);
  check_str (gattrib_sheet_get_cell_text (sheet, 0, 0), "10k",
             "colour change preserves text");
  gattrib_sheet_set_cell (sheet, 0, 0, "22k", BLUE);
  check_str (gattrib_sheet_get_cell_text (sheet, 0, 0), "22k",
             "text change after colour");

  /* Row labels survive column appends. */
  {
    gchar *extra[] = { (gchar *) "vendor", NULL };
    gattrib_sheet_append_columns (sheet, 1, extra);
    check (gattrib_sheet_get_n_cols (sheet) == 3, "column append count");
    check_str (gattrib_sheet_get_column_label (sheet, 2), "vendor",
               "appended column label");
    check_str (gattrib_sheet_get_cell_text (sheet, 0, 0), "22k",
               "append preserves cell text");
    check_str (gattrib_sheet_get_cell_text (sheet, 0, 1), "0603",
               "append preserves column 1");
  }

  /* Selection API. */
  gattrib_sheet_get_selection (sheet, &sel);
  check (sel.row0 == -1 && sel.col0 == -1, "no selection initially");
  check (gattrib_sheet_get_selected_column (sheet) == -1,
         "no selected column initially");

  gattrib_sheet_select_column (sheet, 0);
  check (gattrib_sheet_get_selected_column (sheet) == 0, "select column 0");
  gattrib_sheet_get_active_cell (sheet, &row, &col);
  check (row == 0 && col == 0, "active cell after select_column");

  gattrib_sheet_select_column (sheet, 1);
  check (gattrib_sheet_get_selected_column (sheet) == 1, "select column 1");

  gattrib_sheet_unselect_all (sheet);
  check (gattrib_sheet_get_selected_column (sheet) == -1, "unselect_all");

  /* Row re-population keeps the same store width.  Passing NULL labels keeps
   * the existing names rather than blanking them, so a resize never loses
   * component identity. */
  gattrib_sheet_set_row_labels (sheet, 2, NULL);
  check (gattrib_sheet_get_n_rows (sheet) == 2, "shrink to 2 rows");
  check_str (gattrib_sheet_get_row_label (sheet, 0), "R1",
             "names preserved when labels omitted");

  /* --- End-to-end mirror of gattrib's own population sequence ---------------
   * This follows x_gtksheet_init() -> x_gtksheet_add_row_labels() ->
   * x_gtksheet_add_col_labels() -> x_gtksheet_add_cell_item(), including the
   * colour-for-state mapping, then reads everything back the way
   * s_table_gtksheet_to_table() does. */
  {
    GtkWidget *w2 = gattrib_sheet_new (2, 3, "Components");
    GattribSheet *s2 = GATTRIB_SHEET (w2);
    gchar *cols[] = { (gchar *) "value", (gchar *) "footprint",
                      (gchar *) "vendor", NULL };
    gchar *rows2[] = { (gchar *) "R1", (gchar *) "C1", NULL };

    gattrib_sheet_set_row_labels (s2, 2, rows2);
    gattrib_sheet_set_column_labels (s2, 3, cols);

    /* visibility == INVISIBLE -> GREY; otherwise state decides. */
    gattrib_sheet_set_cell (s2, 0, 0, "10k", BLUE);   /* SHOW_NAME_VALUE */
    gattrib_sheet_set_cell (s2, 0, 1, "0603", RED);   /* SHOW_NAME       */
    gattrib_sheet_set_cell (s2, 0, 2, "Acme", BLACK); /* SHOW_VALUE      */
    gattrib_sheet_set_cell (s2, 1, 0, "100n", GREY);  /* INVISIBLE       */

    check (gattrib_sheet_get_n_rows (s2) == 2, "e2e row count");
    check (gattrib_sheet_get_n_cols (s2) == 3, "e2e col count");
    check_str (gattrib_sheet_get_row_label (s2, 0), "R1", "e2e row 0 label");
    check_str (gattrib_sheet_get_row_label (s2, 1), "C1", "e2e row 1 label");
    check_str (gattrib_sheet_get_column_label (s2, 0), "value",
               "e2e col 0 header");
    check_str (gattrib_sheet_get_column_label (s2, 2), "vendor",
               "e2e col 2 header");

    check_str (gattrib_sheet_get_cell_text (s2, 0, 0), "10k", "e2e cell R1/value");
    check_str (gattrib_sheet_get_cell_text (s2, 0, 1), "0603",
               "e2e cell R1/footprint");
    check_str (gattrib_sheet_get_cell_text (s2, 0, 2), "Acme",
               "e2e cell R1/vendor");
    check_str (gattrib_sheet_get_cell_text (s2, 1, 0), "100n",
               "e2e cell C1/value");
    /* A cell that was never set reads as NULL, which is what makes
     * s_table_gtksheet_to_table() treat the attribute as absent. */
    check (gattrib_sheet_get_cell_text (s2, 1, 1) == NULL,
           "e2e unset cell is empty");

    /* Column deletion, as used by s_toplevel_delete_attrib_col(). */
    gattrib_sheet_delete_column (s2, 1);
    check (gattrib_sheet_get_n_cols (s2) == 2, "e2e after delete_column");
    check_str (gattrib_sheet_get_column_label (s2, 1), "vendor",
               "e2e column shifted after delete");

    g_object_ref_sink (w2);
    g_object_unref (w2);
  }

  g_object_ref_sink (widget);
  g_object_unref (widget);

  if (failures == 0)
    g_print ("\nALL PASS\n");
  else
    g_print ("\n%d FAILURE(S)\n", failures);

  return failures == 0 ? 0 : 1;
}
