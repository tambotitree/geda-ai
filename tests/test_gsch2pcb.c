/* Regression checks for PCB element formats accepted by gsch2pcb. */
int gsch2pcb_program_main (int argc, char **argv);
#define main gsch2pcb_program_main
#include "../utils/src/gsch2pcb.c"
#undef main

/*! \brief Verify edits preserve quoting, coordinates and trailing PCB fields. */
static void
check_header (const gchar *input, const gchar *expected)
{
  gchar *line = g_strdup (input);
  PcbElement *element = pcb_element_line_parse (line);
  FILE *output = tmpfile ();
  gchar actual[1024];
  g_assert_nonnull (element);
  g_assert_nonnull (output);
  write_element_header (output, element, "new-footprint", "R12", "22k");
  rewind (output);
  g_assert_nonnull (fgets (actual, sizeof actual, output));
  g_assert_cmpstr (actual, ==, expected);
  fclose (output);
  pcb_element_free (element);
  g_free (line);
}

/*! \brief Exercise both flag styles and reject PCB layouts as footprints. */
int
main (void)
{
  FILE *source;
  FILE *output;
  gchar *path = NULL;
  gint fd;
  check_header ("Element[\"onsolder\" \"old\" \"R1\" \"10k\" 100 200 0 0 0 100 \"\"]\n",
                "Element[\"onsolder\" \"new-footprint\" \"R12\" \"22k\" 100 200 0 0 0 100 \"\"]\n");
  check_header ("Element(0 \"old\" \"R1\" \"10k\" 10 20 0 100 0)\n",
                "Element(0 \"new-footprint\" \"R12\" \"22k\" 10 20 0 100 0)\n");
  fd = g_file_open_tmp ("geda-footprint-XXXXXX", &path, NULL);
  g_assert_cmpint (fd, >=, 0);
  source = fdopen (fd, "w");
  g_assert_nonnull (source);
  fputs ("\n \nPCB[\"board\" 100 100]\n", source);
  fclose (source);
  output = tmpfile ();
  g_assert_nonnull (output);
  g_assert_false (insert_element (output, path, "test", "R1", "1k"));
  fclose (output);
  g_assert_cmpint (remove (path), ==, 0);
  g_free (path);
  puts ("PASS: quoted/numeric PCB headers and layout rejection");
  return 0;
}
