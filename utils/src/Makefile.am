## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gsch2pcb grenum

# don't forget all *.h files */
grenum_SOURCES = grenum.c grenum.h

gsch2pcb_SOURCES = gsch2pcb.c
gsch2pcb_CPPFLAGS = \
	-DPCBM4DIR=\"$(PCBM4DIR)\" \
	-DPCBLIBPATH=\"$(PCBLIBPATH)\"
gsch2pcb_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS)
gsch2pcb_LDFLAGS = $(GLIB_LIBS)


AM_CFLAGS = $(GCC_CFLAGS) $(MINGW_CFLAGS) \
	-Wall -Wextra -Werror -Wpedantic -Wstrict-prototypes \
	-Wmissing-prototypes -Wformat=2
AM_CPPFLAGS = -I$(top_srcdir)/libgeda/include -I$(srcdir)/../include \
	-I$(top_srcdir) -I$(includedir)

MOSTLYCLEANFILES = *.log *.ps core FILE *~ olib.c
CLEANFILES = *.log *.ps core FILE *~  olib.c
DISTCLEANFILES = *.log core FILE *~ olib.c
MAINTAINERCLEANFILES = *.log *.ps core FILE *~ Makefile.in olib.c

check_PROGRAMS = test-gsch2pcb
TESTS = $(check_PROGRAMS)
test_gsch2pcb_SOURCES = $(top_srcdir)/tests/test_gsch2pcb.c
test_gsch2pcb_CPPFLAGS = $(AM_CPPFLAGS) $(gsch2pcb_CPPFLAGS)
test_gsch2pcb_CFLAGS = $(gsch2pcb_CFLAGS)
test_gsch2pcb_LDADD = $(GLIB_LIBS)

EXTRA_DIST = $(top_srcdir)/tests/test_gsch2pcb_workflow.py
check-local: gsch2pcb
	GEDA_TEST_BUILD_ROOT='$(abs_top_builddir)' $(PYTHON) \
	  $(top_srcdir)/tests/test_gsch2pcb_workflow.py
