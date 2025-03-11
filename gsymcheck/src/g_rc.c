/* gEDA - GPL Electronic Design Automation
 * gsymcheck - gEDA Symbol Check 
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02111-1301 USA.
 */

#include <config.h>
#include <version.h>

#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <libgeda/libgeda.h>

#include "../include/struct.h"
#include "../include/globals.h"
#include "../include/i_vars.h"
#include "../include/prototype.h"
#include "../include/gettext.h"

SCM g_rc_gsymcheck_version(SCM scm_version)
{
  char *version;
  SCM ret = SCM_BOOL_T;

  SCM_ASSERT (scm_is_string (scm_version), scm_version,
	      SCM_ARG1, "gsymcheck-version");

  version = scm_to_utf8_string (scm_version);
  if (g_ascii_strcasecmp (version, PACKAGE_DATE_VERSION) != 0) {
    fprintf(stderr, _(
            "You are running gEDA/gaf version [%s%s.%s],\n"
            "but you have a version [%s] gsymcheckrc file:\n[%s]\n"
            "Please be sure that you have the latest rc file.\n"),
            PREPEND_VERSION_STRING, PACKAGE_DOTTED_VERSION, PACKAGE_DATE_VERSION,
            version, rc_filename);
    ret = SCM_BOOL_F;
  }

  free (version);
  return ret;
}

/*************************** GUILE end done *********************************/
