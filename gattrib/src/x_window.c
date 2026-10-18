/* gEDA - GPL Electronic Design Automation
 * gattrib -- gEDA component and net attribute manipulation using spreadsheet.
 * Copyright (C) 2003-2010 Stuart D. Brorson.
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

/*------------------------------------------------------------------*/
/*! \file
 * \brief Functions for the toplevel window
 *
 * This file holds functions used to handle the toplevel window and
 * various widgets held by that window.  Widges used to handle
 * (GattribSheet *sheet) itself are held in a different file.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


/*------------------------------------------------------------------
 * Includes required to run graphical widgets.
 *------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

/*------------------------------------------------------------------
 * Gattrib specific includes
 *------------------------------------------------------------------*/
#include <libgeda/libgeda.h>       /* geda library fcns  */
#include "../include/struct.h"     /* typdef and struct declarations */
#include "../include/prototype.h"  /* function prototypes */
#include "../include/globals.h"
#include "../include/gattrib_bom_hook.h"
#include "../include/gettext.h"

/*------------------------------------------------------------------
 * Gattrib specific defines
 *------------------------------------------------------------------*/
#define GATTRIB_THEME_ICON_NAME "geda-gattrib"

static void
x_window_create_menu(GtkWindow *window, GtkWidget **menubar);

static void
x_window_set_default_icon();

static void
x_window_debug_dump_sheet (GattribSheet *sheet);

/*! \brief Initialises the toplevel gtksheet
 *
 * This function initializes the toplevel gtksheet stuff.
 *
 *  It basically just initializes the following widgets:
 *  GTK_WINDOW *window 
 *  GTK_CONTAINER *main_vbox
 *  GTK_MENU 
 * 
 *  Note that it doesn't display the spreadsheet itself.  This is done
 *  in x_sheet_build_sheet. I suppose I could postpone all initialization 
 *  until x_sheet_build_sheet, but I figured that I could at least do 
 *  some initialization here. In particular, the stuff to put up the 
 *  menus is long & it is worthwhile to separate it from other code.  
 *  Maybe I'll refactor this later.
 */
void
x_window_init()
{
  GtkWidget *menu_bar;
  GtkWidget *main_vbox;

  /* Set default icon */
  x_window_set_default_icon();

  /*  window is a global declared in globals.h.  */
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);  

  gtk_window_set_title( GTK_WINDOW(window), _("gattrib -- gEDA attribute editor")); 
  
  g_signal_connect(window, "delete_event",
                   G_CALLBACK (gattrib_really_quit), 0);

  /* -----  Now create main_vbox.  This is a container which organizes child  ----- */  
  /* -----  widgets into a vertical column.  ----- */  
  main_vbox = gtk_vbox_new(FALSE,1);
  gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 1);
  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(main_vbox) );

  /* -----  Now create menu bar  ----- */  
  x_window_create_menu(GTK_WINDOW(window), &menu_bar);
  gtk_box_pack_start(GTK_BOX (main_vbox), menu_bar, FALSE, TRUE, 0);

  /* -----  Now init notebook widget  ----- */  
  notebook = gtk_notebook_new();
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_BOTTOM);
  gtk_box_pack_start(GTK_BOX(main_vbox), notebook, TRUE, TRUE, 0);
  
  /* -----  Now malloc -- but don't fill out -- space for sheets  ----- */  
  /* This sets up the slot for the attribute sheet.  The sheet widget itself
   * is created later, by x_gtksheet_init().  */
  sheets = g_malloc0(NUM_SHEETS * sizeof(GattribSheet *));
}


/*------------------------------------------------------------------
 * \brief File Open menu
 *
 * File open menu. Currently unimplemented.
 * \todo this should really be done in two stages:
 * -# close the current project and reinitialize structures
 * -# load the new project
 */
#ifdef UNIMPLEMENTED_FEATURES
static void
menu_file_open()
{
  x_dialog_unimplemented_feature();
#if 0
  GSList *file_list;

  file_list = x_fileselect_open();
  
  /* Load the files, don't check if it went OK */
  x_fileselect_load_files(file_list);
  
  g_slist_foreach(file_list, (GFunc)g_free, NULL);
  g_slist_free(file_list);
#endif
}
#endif

/*!
 * \brief File->Save menu item
 *
 * Implement the File->Save menu
 */
static void
menu_file_save()
{
  s_toplevel_gtksheet_to_toplevel(pr_current);  /* Dumps sheet data into TOPLEVEL */
  s_page_save_all(pr_current);  /* saves all pages in design */

  sheet_head->CHANGED = FALSE;
}

/*!
 * \brief File->Export CSV menu item
 *
 * Implement the File->Export CSV menu item
 */
static void 
menu_file_export_csv()
{
  gint cur_page;

  /* first verify that we are on the correct page (components) */
  cur_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

  /* Check that we are on components page. */
  if (cur_page == 0) {
    x_dialog_export_file();
  } else {
    x_dialog_unimplemented_feature();  /* We only support export 
                                          of components now */
  }
}

/*!
 * \brief Edit->New attrib menu item
 *
 * Implement the New attrib menu item
 */
static void 
menu_edit_newattrib()
{
  gint cur_page;

  /* first verify that we are on the correct page (components) */
  cur_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

  /* Check that we are on components page. */
  if (cur_page == 0) {
    x_dialog_newattrib();  /* This creates dialog box  */
  }
}

/*!
 * \brief Edit->Delete Attribute menu item
 *
 * Implements the Delete Attribute menu item
 */
static void
menu_edit_delattrib()
{
  x_dialog_delattrib();
}

/*!
 * \brief Edit->Fetch BOM data menu item
 *
 * Asks the configured supplier about every part in the component table and
 * writes the manufacturer part number, stock/price summary and datasheet link
 * into attribute columns, creating them if needed.
 *
 * Existing values are kept, so a price the designer corrected by hand is not
 * clobbered by a refresh.  The heavy lifting is in gattrib_bom_hook.c; this
 * function only reports the outcome.
 */
static void
menu_edit_fetch_bom_data()
{
  gint cur_page;
  GattribSheet *sheet;
  GattribBomHookStats stats = { 0 };
  GError *error = NULL;
  GtkWidget *dialog;

  /* Component table only: the net and pin tables have no part numbers. */
  cur_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
  if (cur_page != 0) {
    x_dialog_unimplemented_feature();
    return;
  }

  sheet = GATTRIB_SHEET(sheets[0]);
  if (sheet == NULL) {
    return;
  }

  if (!gattrib_bom_hook_enrich_with_default_supplier (sheet, FALSE,
                                                      &stats, &error)) {
    if (error != NULL) {
      dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                       GTK_DIALOG_MODAL,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_OK,
                                       _("Fetching BOM data failed: %s"),
                                       error->message);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
      g_clear_error (&error);
    } else {
      /* No error but nothing written: nothing was matched or the cells were
       * already filled in. */
      dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                       GTK_DIALOG_MODAL,
                                       GTK_MESSAGE_INFO,
                                       GTK_BUTTONS_OK,
                                       _("%d parts looked up, %d found; "
                                         "no cells needed updating."),
                                       stats.looked_up, stats.found);
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (dialog);
    }
    gattrib_bom_hook_stats_clear (&stats);
    return;
  }

  /* No widget refresh is needed: gattrib_sheet_set_cell updates the tree
   * model directly, and the table itself is rebuilt from the widget on save
   * by s_toplevel_gtksheet_to_toplevel(). */

  dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                   GTK_DIALOG_MODAL,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   _("Filled in %d of %d parts (%d not "
                                     "found by the supplier)."),
                                   stats.updated, stats.looked_up,
                                   stats.looked_up - stats.found);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
  gattrib_bom_hook_stats_clear (&stats);
}

/*!
 * The Gtk action table
 */
static const GtkActionEntry actions[] = {
  /* name, stock-id, label, accelerator, tooltip, callback function */
  /* File menu */
  { "file", NULL, "_File"},
  /* { "file-open", GTK_STOCK_OPEN, "Open", "<Control>O", "", menu_file_open}, */
  { "file-save", GTK_STOCK_SAVE, "Save", "<Control>S", "", menu_file_save},
  { "file-export-csv", NULL, "Export CSV", "", "", menu_file_export_csv},
  /* { "file-print", GTK_STOCK_PRINT, "Print", "<Control>P", "", x_dialog_unimplemented_feature}, */
  { "file-quit", GTK_STOCK_QUIT, "Quit", "<Control>Q", "", G_CALLBACK(gattrib_really_quit)},

  /* Edit menu */
  { "edit", NULL, "_Edit"},
  { "edit-add-attrib", NULL, "Add new attrib column", "", "", menu_edit_newattrib},
  { "edit-delete-attrib", NULL, "Delete attrib column", "", "", menu_edit_delattrib},
  { "edit-fetch-bom-data", NULL, "Fetch BOM data", "", "Fill in part number, "
    "price and datasheet columns from a supplier", menu_edit_fetch_bom_data},
  /* { "edit-find-attrib", GTK_STOCK_FIND, "Find attrib value", "<Control>F", "", x_dialog_unimplemented_feature}, */
  /* { "edit-search-replace-attrib-value", NULL, "Search and replace attrib value", "", "", x_dialog_unimplemented_feature}, */
  /* { "edit-search-for-refdes", NULL, "Search for refdes", "", "", x_dialog_unimplemented_feature}, */

  /* Visibility menu */
  { "visibility", NULL, "_Visibility"},
  { "visibility-invisible", NULL, "Set selected invisible", "", "", s_visibility_set_invisible},
  { "visibility-name-only", NULL, "Set selected name visible only", "", "", s_visibility_set_name_only},
  { "visibility-value-only", NULL, "Set selected value visible only", "", "", s_visibility_set_value_only},
  { "visibility-name-value", NULL, "Set selected name and value visible", "", "", s_visibility_set_name_and_value},

  /* Help menu */
  { "help", NULL, "_Help"},
  { "help-about", GTK_STOCK_ABOUT, "About", "", "", x_dialog_about_dialog},
};


/*! \brief Create and attach the menu bar
 *
 * Create the menu bar and attach it to the main window.
 *
 *  First, the GtkActionGroup object is created and filled with
 *  entries of type GtkActionEntry (each entry specifies a single
 *  action, such as opening a file). Then the GtkUIManager object
 *  is created and used to load menus.xml file with the menu
 *  description. Finally, the GtkAccelGroup is added to the
 *  main window to enable keyboard accelerators and a pointer
 *  to the menu bar is retrieved from the GtkUIManager object.
 * \param window Window to add the menubar to
 * \param [out] menubar Created menubar
 */
static void
x_window_create_menu(GtkWindow *window, GtkWidget **menubar)
{
  gchar *menu_file;
  GtkUIManager *ui;
  GtkActionGroup *action_group;
  GError *error = NULL;

  /* Create and fill the action group object */
  action_group = gtk_action_group_new("");
  gtk_action_group_add_actions(action_group, actions, G_N_ELEMENTS(actions), NULL);

  /* Create the UI manager object */
  ui = gtk_ui_manager_new();

  gtk_ui_manager_insert_action_group(ui, action_group, 0);

  menu_file = g_build_filename(s_path_sys_data (), "gattrib-menus.xml", NULL);

  gtk_ui_manager_add_ui_from_file(ui, menu_file, &error);
  if(error != NULL) {
    /* An error occured, terminate */
    fprintf(stderr, _("Error loading %s:\n%s\n"), menu_file, error->message);
    exit(1);
  }

  g_free(menu_file);

  gtk_window_add_accel_group (window, gtk_ui_manager_get_accel_group(ui));

  *menubar = gtk_ui_manager_get_widget(ui, "/ui/menubar/");
}


/*! \brief Add all items to the top level window
 *
 * This function updates the top level window
 *         after a new page is read in.  
 *
 *  It does the following:
 * 
 *  -# Create a new gtksheet having the current dimensions.
 *  -# Call x_gktsheet_add_row_labels(comp_count, master_*_list_head)
 *  -# Call x_gktsheet_add_col_labels(comp_attrib_count, master_*_attrib_list_head)
 *  -# Call x_gktsheet_add_row_labels(net_count, master_*_list_head)
 *  -# Call x_gktsheet_add_col_labels(net_attrib_count, master_*_attrib_list_head)
 *  -# loop on i, j -- call x_gtksheet_add_entry(i, j, attrib_value)
 *  -# Call gtk_widget_show(window) to show new window.
 */
void
x_window_add_items()
{
  gint i, j;
  gint num_rows, num_cols;
  gchar *text, *error_string;
  gint visibility, show_name_value;
  
  /* Do these sanity check to prevent later segfaults */
  if (sheet_head->comp_count == 0) {
    error_string = _("No components found in entire design!\nDo you have refdeses on your components?");
    x_dialog_fatal_error(error_string, 1);
  }

  if (sheet_head->comp_attrib_count == 0) {
    error_string = _("No configurable component attributes found in entire design!\nPlease attach at least some attributes before running gattrib.");
    x_dialog_fatal_error(error_string, 2);
  }

  if (sheet_head->pin_count == 0) {
    error_string = _("No pins found on any components!\nPlease check your design.");
    x_dialog_fatal_error(error_string, 3);
  }


  /*  initialize the gtksheet. */
  x_gtksheet_init();  /* this creates a new gtksheet having dimensions specified
		       * in sheet_head->comp_count, etc. . .  */

  if (sheet_head->comp_count > 0 ) {
    x_gtksheet_add_row_labels(GATTRIB_SHEET(sheets[0]), 
			      sheet_head->comp_count, sheet_head->master_comp_list_head);
    x_gtksheet_add_col_labels(GATTRIB_SHEET(sheets[0]), 
			      sheet_head->comp_attrib_count, sheet_head->master_comp_attrib_list_head);
  }

#ifdef UNIMPLEMENTED_FEATURES
  /* This is not ready.  I need to implement net attributes */
  if (sheet_head->net_count > 0 ) {
    x_gtksheet_add_row_labels(GATTRIB_SHEET(sheets[1]), 
			      sheet_head->net_count, sheet_head->master_net_list_head);
    x_gtksheet_add_col_labels(GATTRIB_SHEET(sheets[1]), 
			      sheet_head->net_attrib_count, sheet_head->master_net_attrib_list_head);
  } else {
    x_gtksheet_add_row_labels(GATTRIB_SHEET(sheets[1]), 1, NULL);
    x_gtksheet_add_col_labels(GATTRIB_SHEET(sheets[1]), 1, NULL);
  }  
#endif

#ifdef UNIMPLEMENTED_FEATURES
  if (sheet_head->pin_count > 0 ) {
    x_gtksheet_add_row_labels(GATTRIB_SHEET(sheets[2]), 
			      sheet_head->pin_count, sheet_head->master_pin_list_head);
    x_gtksheet_add_col_labels(GATTRIB_SHEET(sheets[2]), 
			      sheet_head->pin_attrib_count, sheet_head->master_pin_attrib_list_head);
  }
#endif

  /* ------ Comp sheet: put values in the individual cells ------- */
  num_rows = sheet_head->comp_count;
  num_cols = sheet_head->comp_attrib_count;
  for (i = 0; i < num_rows; i++) {
    for (j = 0; j < num_cols; j++) {
      if ( (sheet_head->component_table)[i][j].attrib_value ) { /* NULL = no entry */
	text = (gchar *) g_strdup( (sheet_head->component_table)[i][j].attrib_value );
	visibility = (sheet_head->component_table)[i][j].visibility;
	show_name_value = (sheet_head->component_table)[i][j].show_name_value;
	x_gtksheet_add_cell_item( GATTRIB_SHEET(sheets[0]), i, j, (gchar *) text, 
				  visibility, show_name_value );
	g_free(text);
      }
    }
  }

#ifdef UNIMPLEMENTED_FEATURES
  /* ------ Net sheet: put values in the individual cells ------- */
  num_rows = sheet_head->net_count;
  num_cols = sheet_head->net_attrib_count;
  for (i = 0; i < num_rows; i++) {
    for (j = 0; j < num_cols; j++) {
      if ( (sheet_head->net_table)[i][j].attrib_value ) { /* NULL = no entry */
	text = (gchar *) g_strdup( (sheet_head->net_table)[i][j].attrib_value );
	visibility = (sheet_head->net_table)[i][j].visibility;
	show_name_value = (sheet_head->component_table)[i][j].show_name_value;
	x_gtksheet_add_cell_item( GATTRIB_SHEET(sheets[1]), i, j, (gchar *) text,
				  visibility, show_name_value );
	g_free(text);
      }
    }
  }
#endif

#ifdef UNIMPLEMENTED_FEATURES
  /* ------ Pin sheet: put pin attribs in the individual cells ------- */
  num_rows = sheet_head->pin_count;
  num_cols = sheet_head->pin_attrib_count;
  for (i = 0; i < num_rows; i++) {
    for (j = 0; j < num_cols; j++) {
      if ( (sheet_head->pin_table)[i][j].attrib_value ) { /* NULL = no entry */
	text = (gchar *) g_strdup( (sheet_head->pin_table)[i][j].attrib_value );
	/* pins have no visibility attributes, must therefore provide default. */
	x_gtksheet_add_cell_item( GATTRIB_SHEET(sheets[2]), i, j, (gchar *) text, 
				  VISIBLE, SHOW_VALUE );
	g_free(text);
      }
    }
  }
#endif

  x_window_debug_dump_sheet (GATTRIB_SHEET (sheets[0]));

  gtk_widget_show_all( GTK_WIDGET(window) );
}


/*! \brief Print the contents of a sheet to stderr, if requested
 *
 *  \par Function Description
 *  Dumps the row and column labels and every populated cell of \a sheet to
 *  stderr.  This exists because the tables are the only place the component
 *  and attribute data are visible, and inspecting them by eye in a GUI is
 *  awkward when debugging the loading path.
 *
 *  The dump is enabled by setting the GATTRIB_DEBUG_DUMP environment
 *  variable to any non-empty value; this is a diagnostic aid only, so it is
 *  silent unless explicitly asked for.
 *
 *  \param [in] sheet The sheet to dump.
 */
static void
x_window_debug_dump_sheet (GattribSheet *sheet)
{
  gint r, c, n_rows, n_cols;

  if (g_getenv ("GATTRIB_DEBUG_DUMP") == NULL) {
    return;
  }

  n_rows = gattrib_sheet_get_n_rows (sheet);
  n_cols = gattrib_sheet_get_n_cols (sheet);

  g_printerr ("GATTRIB_DEBUG_DUMP: %d rows, %d columns\n", n_rows, n_cols);

  for (c = 0; c < n_cols; c++) {
    g_printerr ("  column %d: %s\n", c,
                gattrib_sheet_get_column_label (sheet, c));
  }
  for (r = 0; r < n_rows; r++) {
    g_printerr ("  row %d: %s", r, gattrib_sheet_get_row_label (sheet, r));
    for (c = 0; c < n_cols; c++) {
      const gchar *text = gattrib_sheet_get_cell_text (sheet, r, c);
      g_printerr (" | %s=%s", gattrib_sheet_get_column_label (sheet, c),
                  text != NULL ? text : "");
    }
    g_printerr ("\n");
  }
}


/*! \brief Set application icon
 *
 * Setup default icon for GTK windows
 *
 *  Sets the default window icon by name, to be found in the current icon
 *  theme. The name used is #defined above as GATTRIB_THEME_ICON_NAME.
 */
static void
x_window_set_default_icon()
{
  gtk_window_set_default_icon_name( GATTRIB_THEME_ICON_NAME );
}

