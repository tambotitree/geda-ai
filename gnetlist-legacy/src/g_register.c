/* gEDA - GPL Electronic Design Automation
 * gnetlist - gEDA Netlist
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2020 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <config.h>

#include <stdio.h>
#include <sys/stat.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <libgeda/libgeda.h>

#include "../include/globals.h"
#include "../include/prototype.h"


/*! \brief Register a 0-argument Scheme procedure with a checked C signature. */
static void
register_subr_0(const char *name, SCM (*callback)(void))
{
  scm_c_define_gsubr(name, 0, 0, 0, (scm_t_subr) callback);
}

/*! \brief Register a 1-argument Scheme procedure with a checked C signature. */
static void
register_subr_1(const char *name, SCM (*callback)(SCM))
{
  scm_c_define_gsubr(name, 1, 0, 0, (scm_t_subr) callback);
}

/*! \brief Register a 2-argument Scheme procedure with a checked C signature. */
static void
register_subr_2(const char *name, SCM (*callback)(SCM, SCM))
{
  scm_c_define_gsubr(name, 2, 0, 0, (scm_t_subr) callback);
}

/*! \brief Register a 3-argument Scheme procedure with a checked C signature. */
static void
register_subr_3(const char *name, SCM (*callback)(SCM, SCM, SCM))
{
  scm_c_define_gsubr(name, 3, 0, 0, (scm_t_subr) callback);
}

/*! \brief Register RC and netlist procedures at the Guile boundary. */
void g_register_funcs(void)
{
  register_subr_0("quit", g_quit);
  register_subr_0("exit", g_quit);
  register_subr_1("gnetlist-version", g_rc_gnetlist_version);
  register_subr_1("net-naming-priority", g_rc_net_naming_priority);
  register_subr_1("hierarchy-traversal", g_rc_hierarchy_traversal);
  register_subr_1("hierarchy-uref-mangle", g_rc_hierarchy_uref_mangle);
  register_subr_1("hierarchy-netname-mangle", g_rc_hierarchy_netname_mangle);
  register_subr_1("hierarchy-netattrib-mangle", g_rc_hierarchy_netattrib_mangle);
  register_subr_1("hierarchy-uref-separator", g_rc_hierarchy_uref_separator);
  register_subr_1("hierarchy-netname-separator", g_rc_hierarchy_netname_separator);
  register_subr_1("hierarchy-netattrib-separator", g_rc_hierarchy_netattrib_separator);
  register_subr_1("hierarchy-netattrib-order", g_rc_hierarchy_netattrib_order);
  register_subr_1("hierarchy-netname-order", g_rc_hierarchy_netname_order);
  register_subr_1("hierarchy-uref-order", g_rc_hierarchy_uref_order);
  register_subr_1("unnamed-netname", g_rc_unnamed_netname);
  register_subr_1("unnamed-busname", g_rc_unnamed_busname);
  register_subr_1("gnetlist:get-packages", g_get_packages);
  register_subr_1("gnetlist:get-non-unique-packages", g_get_non_unique_packages);
  register_subr_1("gnetlist:get-pins", g_get_pins);
  register_subr_1("gnetlist:get-all-nets", g_get_all_nets);
  register_subr_1("gnetlist:get-all-unique-nets", g_get_all_unique_nets);
  register_subr_1("gnetlist:get-all-connections", g_get_all_connections);
  register_subr_2("gnetlist:get-nets", g_get_nets);
  register_subr_1("gnetlist:get-pins-nets", g_get_pins_nets);
  register_subr_2("gnetlist:get-all-package-attributes", g_get_all_package_attributes);
  register_subr_1("gnetlist:get-toplevel-attribute", g_get_toplevel_attribute);
  register_subr_1("gnetlist:get-renamed-nets", g_get_renamed_nets);
  register_subr_3("gnetlist:get-attribute-by-pinseq", g_get_attribute_by_pinseq);
  register_subr_3("gnetlist:get-attribute-by-pinnumber", g_get_attribute_by_pinnumber);
  register_subr_1("gnetlist:vams-get-package-attributes", vams_get_package_attributes);
  register_subr_3("gnetlist:graphical-objs-in-net-with-attrib-get-attrib", g_graphical_objs_in_net_with_attrib_get_attrib);
  register_subr_0("gnetlist:get-backend-arguments", g_get_backend_arguments);
  register_subr_0("gnetlist:get-input-files", g_get_input_files);
  register_subr_0("gnetlist:get-verbosity", g_get_verbosity);
}

SCM g_quit(void)
{
    gnetlist_quit();
    exit(0);
}
