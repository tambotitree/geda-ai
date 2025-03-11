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
#include <math.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <libgeda/libgeda.h>

#include "../include/globals.h"
#include "../include/prototype.h"
#include "../include/gettext.h"


static int char_index = 0;

void verbose_print(char *string)
{
    if (verbose_mode) {
	printf("%s", string);
	char_index++;
	if ((char_index + 1) >= 78) {
	    printf("\n");
	    char_index = 0;
	}
    }
}

void verbose_done(void)
{
    if (verbose_mode) {
	if (char_index >= 70) {
	    printf(_("\nDONE\n"));
	} else {
	    printf(_(" DONE\n"));
	}

	char_index = 0;
    }
}

void verbose_reset_index(void)
{
    char_index = 0;
}
