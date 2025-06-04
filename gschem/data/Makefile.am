theme=hicolor

# --------------------------------------------------------------------
# These icons get installed to a gEDA-specific icons
# directory. They're used within gschem.
datadir=@datadir@
icondir=$(datadir)/icons
iconsvg = gschem-icons.svg

action_icons = \
	insert-box \
	insert-line \
	insert-circle \
	insert-arc \
	insert-path \
	insert-symbol \
	insert-net \
	insert-bus \
	insert-pin \
	insert-attribute \
	insert-text \
	select \
	symbol-datasheet \
	attribute-attach \
	attribute-detach \
	attribute-show-value \
	attribute-show-name \
	attribute-show-both \
	clone \
	multi-clone \
	deselect

action_icon_files = \
	$(action_icons:%=%-16.png) \
	insert-symbol-22.png \
	insert-net-22.png \
	insert-bus-22.png \
	insert-text-22.png \
	select-22.png

regen-svg-icons: $(iconsvg)
	if test -x "`which inkscape`"; then \
	  for f in $(action_icon_files); do \
	    id=`echo $$f | sed -e's:.png::'`; \
	    size=`echo $$id | sed -e's:.*-::'`; \
	    inkscape --export-id=$$id \
	             --export-png=$$id.out \
	             $(srcdir)/$(iconsvg) | tee $$id.log; \
	    if grep "exported to $${size} x $${size} pixels" $$id.log >/dev/null; then \
	      mv $$id.out $(srcdir)/$$f; \
	    else \
	      echo "*** ERROR: Exported pixmap has wrong size ***" >&2; \
	    fi; \
	    rm $$id.log; \
	  done; \
	fi

icons = \
	$(action_icon_files:%=actions,%)

# --------------------------------------------------------------------
# The application icon gets installed to the system icons
# directory. It's used by e.g. the desktop environment to make buttons
# that launch gEDA look nice.
themedir=$(xdgdatadir)/icons/$(theme)

app_icon = geda-gschem

app_icon_files = \
	$(app_icon:%=%-48.png) \
	$(app_icon:%=%.svg)
#	$(app_icon:%=%-16.png)
#	$(app_icon:%=%-22.png)
#	$(app_icon:%=%-24.png)
#	$(app_icon:%=%-32.png)
#

theme_icons = \
	$(app_icon_files:%=apps,%)

# --------------------------------------------------------------------
# We also install a .desktop file.
# This is for standard XDG paths, we'll use pkgdatadir for gEDA-AI specific
xdgdatadir=@XDGDATADIR@

# Install to gEDA-AI specific application directory
# This will be $(prefix)/share/applications
desktopdir = $(datadir)/applications
desktop_in_files = geda-gschem.desktop.in
# Use dist_PRIMARY_DATA for automake to correctly handle installation to PRIMARYdir
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)




DOMAIN=geda-gschem

@DESKTOP_I18N_LANGS_RULE@
@DESKTOP_I18N_DESKTOP_RULE@

# AppData/AppStream metadata file - this still goes to a standard location
metainfodir = $(datadir)/metainfo
dist_metainfo_DATA = geda-gschem.metainfo.xml

# --------------------------------------------------------------------
# Special rules for installing icon files
install_icons_exec = \
	$(ICON_THEME_INSTALLER) \
	-t $(theme) \
	-s $(srcdir) \
	-d "x$(DESTDIR)" \
	-b $(icondir) \
	-m "$(mkinstalldirs)" \
	-x "$(INSTALL_DATA)"

install_theme_icons_exec = \
	$(ICON_THEME_INSTALLER) \
	-t $(theme) \
	-s $(srcdir) \
	-d "x$(DESTDIR)" \
	-b $(themedir) \
	-m "$(mkinstalldirs)" \
	-x "$(INSTALL_DATA)"

install-data-local:
	$(install_icons_exec) -i $(icons)
	$(install_theme_icons_exec) -i $(theme_icons)


install-data-hook:
	@echo "Running install-data-hook for gEDA-AI desktop database (gschem/data/Makefile.am)"
	$(SHELL) $(top_srcdir)/admin/geda-update-databases.sh desktop "$(DESTDIR)$(desktopdir)"
	@echo "gEDA-AI desktop database update script called for $(DESTDIR)$(desktopdir)"

uninstall-hook:
# The uninstall part for XDG database might need to be re-evaluated if geda-update-databases.sh
# is to handle cache removal in the future. For now, this part related to XDG can be removed or commented.
# if ENABLE_UPDATE_XDG_DATABASE # This conditional might be removed if custom script is sole authority
#	$(UPDATE_DESKTOP_DATABASE) "$(DESTDIR)$(desktopdir)"
# endif
	$(SHELL) $(top_srcdir)/admin/geda-update-databases.sh desktop "$(DESTDIR)$(desktopdir)" uninstall
	$(install_icons_exec) -u $(icons)
	$(install_theme_icons_exec) -u $(theme_icons)

MAINTAINERCLEANFILES = *.log core FILE *~ Makefile.in
DISTCLEANFILES = $(desktop_DATA)
EXTRA_DIST = $(iconsvg) $(action_icon_files) \
             $(app_icon_files) \
             $(desktop_in_files)
