/*
 * unit_tests.c
 *
 * Runtime GUI Test Object Registry for gschem
 * --------------------------------------------
 *
 * This file implements a modular system for registering and validating
 * key GTK widget objects created during the construction of complex UI
 * subsystems (such as x_window_create_main, page_view creation, etc.).
 *
 * DESIGN PATTERN: Deferred Test Object Registration + Validation
 *
 * GOAL:
 *  - Allow large procedural UI construction functions to remain clean
 *    and focused, without interleaving validation or diagnostics.
 *  - Collect important GUI objects during construction and defer all
 *    validation or layout checks until after initialization completes.
 *  - Enable optional GUI test runs during development or debug builds.
 *
 * MOTIVATION:
 *  - GTK UIs often require widgets to be shown and realized before their
 *    allocation, visibility, and mapped states are meaningful.
 *  - Inserting validation too early (e.g. before gtk_widget_show_all())
 *    leads to inconsistent or false results.
 *  - Centralized registration enables runtime layout assertions, diagnostics,
 *    and eventual automated tests of widget presence and geometry.
 *
 * EXAMPLE USAGE:
 *     add_test_object_x_window_main(XWIN_MAIN_BOX, "Main Vertical Box", main_box);
 *     ...
 *     gtk_widget_show_all(w_current->main_window);
 *     validate_test_objects_x_window_main();  // Run all tests AFTER realization
 *
 * NAMING CONVENTIONS:
 *     - add_test_object_<subsystem_name>(...)
 *         Called during construction to record test objects and metadata.
 *
 *     - validate_test_objects_<subsystem_name>()
 *         Called after construction (typically after gtk_widget_show_all)
 *         to evaluate object presence, visibility, size, etc.
 *
 * FUTURE DIRECTIONS:
 *     - Attach expectations (e.g. "must be visible", "must be >200px wide")
 *     - Validate widget hierarchy relations (e.g. child-of, packed-in)
 *     - Add a GUI self-test CLI flag (--run-gui-tests)
 *     - Extend to other object types (menus, actions, state machines)
 *
 * Copyleft 2025 John Ryan, maintainer geda-ai
 *
 * This file is part of gEDA and is licensed under the GNU GPL v2 or later.
 */
#include "gschem.h"
#include "unit_tests.h"

GschemMainWindowRoster gschem_widget_roster[XWIN_LAST];
/**
 * add_test_object_x_window_main:
 * @id:      The unique roster enumeration ID (from XWindowRosterItem)
 * @name:    Descriptive label for the test object (e.g., "Main Vertical Box")
 * @widget:  The GtkWidget pointer to be tracked; may be NULL on failure
 *
 * Registers a GTK widget as a testable object in the main window roster.
 *
 * This function captures key construction and layout metadata for GUI validation,
 * including screen coordinates, dimensions, and visibility state. It supports
 * a design pattern where large functions register subcomponents for later inspection.
 *
 * Future unit test phases can use this registry to:
 *   - Verify that all required widgets were constructed
 *   - Inspect placement, size, and visibility/mapping
 *   - Enforce completeness before main GTK loop
 */
void add_test_object_x_window_main(XWindowRosterItem id, const char *name, GtkWidget *widget) {
    if (id < 0 || id >= XWIN_LAST) return;

    gschem_widget_roster[id].id = id;
    gschem_widget_roster[id].name = name;
    gschem_widget_roster[id].widget = widget;

    if (widget != NULL) {
        GtkAllocation allocation;
        gtk_widget_get_allocation(widget, &allocation);

        gschem_widget_roster[id].top_left_x = allocation.x;
        gschem_widget_roster[id].top_left_y = allocation.y;
        gschem_widget_roster[id].width = allocation.width;
        gschem_widget_roster[id].height = allocation.height;
        gschem_widget_roster[id].visible = gtk_widget_get_visible(widget);
        gschem_widget_roster[id].mapped = gtk_widget_get_mapped(widget);
    }
}

/**
 * print_test_objects_x_window_main:
 *
 * Prints the test object roster for the main window to stdout.
 *
 * Useful during debugging or when inspecting the layout state after all
 * widgets are constructed but before the main GTK loop runs.
 */
void print_test_objects_x_window_main() {
    for (int i = 0; i < XWIN_LAST; ++i) {
        GschemMainWindowRoster *r = &gschem_widget_roster[i];
        printf("ID: %2d (%-20s) → Widget: %p [%dx%d at (%d,%d)] visible=%d mapped=%d\n",
            r->id, r->name ? r->name : "unnamed",
            r->widget,
            r->width, r->height,
            r->top_left_x, r->top_left_y,
            r->visible, r->mapped);
    }
}

/**
 * check_test_objects_x_window_main:
 * @roster: Pointer to the widget roster array
 * @count:  Number of expected items (should match XWIN_LAST)
 *
 * Returns TRUE if all widgets are non-NULL, otherwise prints the first failure
 * and returns FALSE.
 */
gboolean check_test_objects_x_window_main(GschemMainWindowRoster *roster, int count) {
    for (int i = 0; i < count; ++i) {
        if (!roster[i].widget) {
            fprintf(stderr, "Roster check failed: item %d (%s) is NULL\n",
                    i, roster[i].name ? roster[i].name : "unnamed");
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * run_all_tests_x_window_main:
 *
 * Convenience function to run roster tests on the main window layout.
 * Returns TRUE if all widgets are present and valid.
 */
gboolean run_all_tests_x_window_main() {
    return check_test_objects_x_window_main(gschem_widget_roster, XWIN_LAST);
}