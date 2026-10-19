/*
 * unit_tests.h
 *
 * Runtime Test Object Interface for GTK Widget Validation
 * -------------------------------------------------------
 *
 * This header defines the data structures and function prototypes used to
 * register, track, and validate key GTK widgets in large procedural subsystems
 * (e.g., gschem’s main window layout in x_window_create_main).
 *
 * DESIGN GOALS:
 *  - Decouple layout creation from validation logic
 *  - Register testable objects with metadata (name, location, visibility)
 *  - Enable post-construction inspection and runtime assertions
 *
 * Typical usage is limited to debug builds or CLI-driven self-tests.
 * Extendable to other subsystems beyond the main window.
 *
 * Copyleft 2025 John Ryan, maintainer geda-ai
 * Licensed under the GNU GPL v2 or later.
 */

 #ifndef GSCHEM_UNIT_TESTS_H
 #define GSCHEM_UNIT_TESTS_H
 
 #include <gtk/gtk.h>
 #include "gschem_toplevel.h"
 #include "roster.h"

 
 /* Register a widget for layout inspection and test validation */
 void add_test_object_x_window_main(XWindowRosterItem id, const char *name, GtkWidget *widget);
 
 /* Print all registered test objects for debugging */
 void print_test_objects_x_window_main (void);
 
 /* Internal helper: check each roster entry is non-NULL */
 gboolean check_test_objects_x_window_main(GschemMainWindowRoster *roster, int count);
 
 /* Public API: run the full set of tests for the main window layout */
 gboolean run_all_tests_x_window_main (void);
 
 #endif /* GSCHEM_UNIT_TESTS_H */
