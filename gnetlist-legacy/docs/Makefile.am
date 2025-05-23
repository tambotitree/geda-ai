
SUBDIRS = vams

EXTRA_DIST = $(html_man_files) logo.png \
	     gnetlist-legacy-main.txt what-is-geda.html \
	     what-is-gnetlist-legacy.html gnetlist-legacy.dox \
	     gnetlist-legacy.1.in mk_verilog_syms.1.in

docsreadmedir = $(docdir)/readmes
dist_docsreadme_DATA = \
	README.bom README.pcb README.switcap README.verilog \
	README.vhdl README.sysc README.eagle

dist_man_MANS = gnetlist-legacy.1
noinst_MANS = mk_verilog_syms.1

.1.in.1:
	$(AM_V_GEN)\
	d=`$(GUILE) -c '(setlocale LC_ALL "C") \
	  (display (strftime "%B %-d, %Y" \
	  (car (strptime "%Y%m%d" (cadr (program-arguments))))))' $(DATE_VERSION)` ; \
	v="$(DOTTED_VERSION).$(DATE_VERSION)" ; \
	y=`echo $(DATE_VERSION)| sed -e"s/\(....\).*/\1/"`; \
	sed -e"s:[@]DATE@:$$d:" -e"s:[@]VERSION@:$$v:" -e"s:[@]YEAR@:$$y:" < $< > $@

# Rules for converting man->html
html_man_files =
if ENABLE_GROFF_HTML
html_man_files += $(dist_man_MANS:%.1=%.html)
endif
SUFFIXES = .1.in .1 .html

docsmandir=$(docdir)/man
docsman_DATA=$(html_man_files)
if ENABLE_GROFF_HTML
.1.html:
	$(AM_V_GEN)\
	rm -rf $@.tmp && \
	LC_NUMERIC=C groff -man -T html  $< >$@.tmp && \
	mv -f $@.tmp $@
else
.1.html:
	@echo "No working groff available: cannot create $@"
	@! :
endif

doxygen: gnetlist-legacy.dox
	@echo 'Creating doxygen documentation for gnetlist-legacy...'
if ENABLE_DOXYGEN
	cd $(srcdir) && $(DOXYGEN) gnetlist-legacy.dox
else
	@echo 'configure was not run with the --enable-doxygen switch.'
	@echo 'The documentation cannot be built from the sources.'
endif

MOSTLYCLEANFILES = *.log *~
CLEANFILES = *.log *~
DISTCLEANFILES = *.log core FILE *~
MAINTAINERCLEANFILES = *.log *~ Makefile.in $(html_man_files)

distclean-local:
	-rm -rf html latex

maintainer-clean-local:
	-rm -rf html latex
