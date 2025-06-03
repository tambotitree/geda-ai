theme=hicolor
geda_ai_datadir=$(pkgdatadir)
icondir=$(geda_ai_datadir)/icons
themedir=$(icondir)/$(theme)

mime_icons = \
	application-x-geda-gsch2pcb-project \
	application-x-geda-schematic        \
	application-x-geda-symbol

mime_icon_files = \
	$(mime_icons:%=%-16.png) \
	$(mime_icons:%=%-22.png) \
	$(mime_icons:%=%-24.png) \
	$(mime_icons:%=%-32.png) \
	$(mime_icons:%=%-48.png) \
	$(mime_icons:%=%.svg)

mime_icon_sources = \
	$(mime_icons:%=%-16.svg) \
	$(mime_icons:%=%-22.svg) \
	$(mime_icons:%=%-32.svg) \
	$(mime_icons:%=%-48.svg)

theme_icons = \
	$(mime_icon_files:%=mimetypes,%)

geda_ai_mimedir = $(geda_ai_datadir)/mime
xmldir = $(geda_ai_mimedir)/packages
xmldirdir = $(xmldir) # Define the installation directory for dist_xmldir_DATA

xml_in_files = libgeda.xml.in
# Install $(xml_DATA) into the directory specified by the variable xmldir
dist_xmldir_DATA = $(xml_in_files:.xml.in=.xml)

application_in_files = \
	x-geda-gsch2pcb-project.desktop.in \
	x-geda-schematic.desktop.in \
	x-geda-symbol.desktop.in

if ENABLE_KDE_DESKTOP_DATA
kdedatadir=@KDEDATADIR@
mimelnkdir = $(kdedatadir)/mimelnk
applicationdir = $(mimelnkdir)/application
application_DATA = $(application_in_files:.desktop.in=.desktop)
endif ENABLE_KDE_DESKTOP_DATA

DOMAIN=$(LIBGEDA_GETTEXT_DOMAIN)

@DESKTOP_I18N_LANGS_RULE@
@DESKTOP_I18N_DESKTOP_RULE@
@DESKTOP_I18N_XML_RULE@

# Custom icon installation to be prefix-relative and Hicolor compliant
install-data-local:
	$(MKDIR_P) "$(DESTDIR)$(themedir)/16x16/mimetypes"
	$(MKDIR_P) "$(DESTDIR)$(themedir)/22x22/mimetypes"
	$(MKDIR_P) "$(DESTDIR)$(themedir)/24x24/mimetypes"
	$(MKDIR_P) "$(DESTDIR)$(themedir)/32x32/mimetypes"
	$(MKDIR_P) "$(DESTDIR)$(themedir)/48x48/mimetypes"
	$(MKDIR_P) "$(DESTDIR)$(themedir)/scalable/mimetypes"
	for icon_base_name in $(mime_icons); do \
	  $(INSTALL_DATA) "$(srcdir)/$${icon_base_name}-16.png" "$(DESTDIR)$(themedir)/16x16/mimetypes/$${icon_base_name}.png"; \
	  $(INSTALL_DATA) "$(srcdir)/$${icon_base_name}-22.png" "$(DESTDIR)$(themedir)/22x22/mimetypes/$${icon_base_name}.png"; \
	  $(INSTALL_DATA) "$(srcdir)/$${icon_base_name}-24.png" "$(DESTDIR)$(themedir)/24x24/mimetypes/$${icon_base_name}.png"; \
	  $(INSTALL_DATA) "$(srcdir)/$${icon_base_name}-32.png" "$(DESTDIR)$(themedir)/32x32/mimetypes/$${icon_base_name}.png"; \
	  $(INSTALL_DATA) "$(srcdir)/$${icon_base_name}-48.png" "$(DESTDIR)$(themedir)/48x48/mimetypes/$${icon_base_name}.png"; \
	  $(INSTALL_DATA) "$(srcdir)/$${icon_base_name}.svg" "$(DESTDIR)$(themedir)/scalable/mimetypes/$${icon_base_name}.svg"; \
	done

# Pass the parent of 'packages' dir to the script, as it expects to append '/packages'
install-data-hook:
	@echo "Running install-data-hook for gEDA-AI mime database (libgeda/data/Makefile.am)"
	$(SHELL) $(top_srcdir)/admin/geda-update-databases.sh mime "$(DESTDIR)$(geda_ai_mimedir)"
	@echo "gEDA-AI mime database update script called for $(DESTDIR)$(geda_ai_mimedir)"

uninstall-hook:
if ENABLE_UPDATE_XDG_DATABASE
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(geda_ai_mimedir)"
endif

MAINTAINERCLEANFILES = *.log core FILE *~ Makefile.in
# $(xml_DATA) are the generated files from $(xml_in_files)
DISTCLEANFILES = $(xml_in_files:.xml.in=.xml) $(application_DATA)
EXTRA_DIST = $(mime_icon_files) $(mime_icon_sources) \
             $(xml_in_files) $(application_in_files)

