theme=hicolor
datadir=@datadir@ # Standard data directory, becomes $(prefix)/share
icondir=$(datadir)/icons

mimedir = $(datadir)/mime # This will be $(prefix)/share/mime
xmldir = $(mimedir)/packages # This will be $(prefix)/share/mime/packages
themedir=$(icondir)/$(theme) # This will be $(prefix)/share/icons/hicolor

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


xml_in_files = libgeda.xml.in
xmldirdir = $(xmldir) # This will be $(prefix)/share/mime/packages
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
install_mime_icons_exec = \
	$(ICON_THEME_INSTALLER) \
	-t $(theme) \
	-s $(srcdir) \
	-d "x$(DESTDIR)" \
	-b $(themedir) \
	-m "$(mkinstalldirs)" \
	-x "$(INSTALL_DATA)"

install-data-local:
	$(install_mime_icons_exec) -i $(theme_icons)


# Pass the parent of 'packages' dir to the script, as it expects to append '/packages'
install-data-hook:
	@echo "Running install-data-hook for gEDA-AI mime database (libgeda/data/Makefile.am)"
	$(SHELL) $(top_srcdir)/admin/geda-update-databases.sh mime "$(DESTDIR)$(mimedir)"
	@echo "gEDA-AI mime database update script called for $(DESTDIR)$(mimedir)"

uninstall-hook:
# if ENABLE_UPDATE_XDG_DATABASE # This conditional might be removed if custom script is sole authority
#	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(mimedir)"
# endif
	$(SHELL) $(top_srcdir)/admin/geda-update-databases.sh mime "$(DESTDIR)$(mimedir)" uninstall
	$(install_mime_icons_exec) -u $(theme_icons)


MAINTAINERCLEANFILES = *.log core FILE *~ Makefile.in
# $(xml_DATA) are the generated files from $(xml_in_files)
DISTCLEANFILES = $(xml_in_files:.xml.in=.xml) $(application_DATA)
EXTRA_DIST = $(mime_icon_files) $(mime_icon_sources) \
             $(xml_in_files) $(application_in_files)

