EXTRA_DIST = gattrib.dox what-is-gattrib.html what-is-geda.html \
	     gattrib-main.txt logo.png gattrib.1.in

all: 
	@echo 'Type: make doxygen  to create doxygen documentation for gattrib'

doxygen: gattrib.dox
	@echo 'Creating doxygen documentation for gattrib...'
if ENABLE_DOXYGEN
	cd $(srcdir) && $(DOXYGEN) gattrib.dox
else
	@echo 'configure was not run with the --enable-doxygen switch.'
	@echo 'The documentation cannot be built from the sources.'
endif

dist_man_MANS = gattrib.1

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
html_man_files += gattrib.html
endif
SUFFIXES = .1.in .1 .html

docsmandir=$(docdir)/man
dist_docsman_DATA=$(html_man_files)
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

MOSTLYCLEANFILES = 	*.log *~
CLEANFILES = 		*.log *~
DISTCLEANFILES =        *.log
MAINTAINERCLEANFILES = 	*.log *~ Makefile.in

distclean-local:
	-rm -rf html latex

maintainer-clean-local:
	-rm -rf html latex
