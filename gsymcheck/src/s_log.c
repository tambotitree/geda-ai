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

#include <stdio.h>

#include <libgeda/libgeda.h>

#include "../include/struct.h"
#include "../include/prototype.h"

/*! \brief Forward preformatted log text to the configured terminal destination.
 * \param log_domain Required GLib callback argument; no prefix is added here.
 * \param log_level Required callback argument; severity is handled upstream.
 * \param buf Preformatted message, or NULL.
 */
void s_log_update (const gchar *log_domain, GLogLevelFlags log_level,
                   const gchar *buf)
{
  (void) log_domain;
  (void) log_level;

  if (buf == NULL)
    return;

  switch (logging_dest) {
    case STDOUT_TTY:
      fputs (buf, stdout);
      break;

    default:
      break;
  }
}
