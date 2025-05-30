doc_DATA = nc.pdf

EXTRA_DIST = nc.doc geda.fig geda.pic nc.ps nc.pdf

BUILT_SOURCES = geda.pic nc.pdf nc.ps

SUFFIXES = .fig .pic .ps .doc .pdf

#
#  some standard rules for groff based documentation
#
.fig.pic:
	@echo "Creating pic (using fig2dev)"
	@if which fig2dev >/dev/null 2>/dev/null; then           \
		fig2dev -L pic $< > $@		               ;\
	else                                                    \
		echo "** Did not find fig2dev **"              ;\
		echo "Some documentation might not be correct!";\
		touch $@				       ;\
	fi

.doc.ps:
	@echo "Creating ps (using groff)"
	@rm -rf $@.tmp
	@if test -n "$(GROFF)" && test "x$(GROFF)" != xno; then  \
		LC_NUMERIC=C $(GROFF) -t -sp -ms $< > $@.tmp    ;\
		mv -f $@.tmp $@                                 ;\
	else						         \
		echo "** Did not find groff or GROFF variable is not set properly **" ;\
		echo "Some Documentation might not be correct!" ;\
		echo "Please ensure groff is installed and detected by ./configure." ;\
		touch $@                                     ;\
	fi

.ps.pdf:
	@echo "Creating pdf (using ps2pdf)"
	@rm -rf $@
	@if which ps2pdf >/dev/null 2>/dev/null; then             \
		ps2pdf $<					;\
	else                                                     \
		echo "** Did not find ps2pdf **"                ;\
		echo "Some Documentation might not be correct!" ;\
		touch $@                                        ;\
	fi

#
#  dependencies
#
nc.ps: geda.pic
nc.pdf: nc.ps

MOSTLYCLEANFILES = *.log *~
CLEANFILES = *.log *~ *.ps *.pdf 
DISTCLEANFILES = *.log core FILE *~
MAINTAINERCLEANFILES = *.log *~ Makefile.in geda.pic *.pdf *.ps
