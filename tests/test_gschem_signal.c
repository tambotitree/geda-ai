/* Headless regression checks for matching and nested signal blocking. */
#include "gschem_signal.h"

/* Count emissions for one caller's connection. */
static void
increment (GObject *instance, guint *count)
{
  (void) instance;
  (*count)++;
}

int
main (void)
{
  GObject *object = g_object_new (G_TYPE_OBJECT, NULL);
  guint signal = g_signal_new ("gschem-test", G_TYPE_OBJECT, G_SIGNAL_RUN_LAST,
                               0, NULL, NULL, NULL, G_TYPE_NONE, 0);
  guint first = 0, second = 0;
  gschem_signal_connect (object, "gschem-test", G_CALLBACK (increment), &first);
  gschem_signal_connect (object, "gschem-test", G_CALLBACK (increment), &second);
  g_signal_emit (object, signal, 0);
  g_assert_cmpuint (first, ==, 1);
  g_assert_cmpuint (second, ==, 1);
  g_assert_cmpuint (gschem_signal_handlers_block (object, G_CALLBACK (increment), &first), ==, 1);
  gschem_signal_handlers_block (object, G_CALLBACK (increment), &first);
  g_signal_emit (object, signal, 0);
  g_assert_cmpuint (first, ==, 1);
  g_assert_cmpuint (second, ==, 2);
  gschem_signal_handlers_unblock (object, G_CALLBACK (increment), &first);
  g_signal_emit (object, signal, 0);
  g_assert_cmpuint (first, ==, 1);
  gschem_signal_handlers_unblock (object, G_CALLBACK (increment), &first);
  g_signal_emit (object, signal, 0);
  g_assert_cmpuint (first, ==, 2);
  g_assert_cmpuint (gschem_signal_handlers_disconnect (object, G_CALLBACK (increment), &first), ==, 1);
  g_signal_emit (object, signal, 0);
  g_assert_cmpuint (first, ==, 2);
  g_assert_cmpuint (second, ==, 5);
  g_object_unref (object);
  g_print ("PASS: callback/data matching, nested blocking, disconnection, teardown\n");
  return 0;
}
