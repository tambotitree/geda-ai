/* Copyright (C) 2013-2020 Roland Lutz

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include "Setup.h"

#define NO_ERROR ((xorn_error_t) -1)


int main(void)
{
	xorn_revision_t rev0, rev1, rev2, rev3;
	xorn_object_t ob0, ob1a, ob1b;

	xorn_revision_t rev4;
	xorn_error_t err;

	setup(&rev0, &rev1, &rev2, &rev3, &ob0, &ob1a, &ob1b);

	rev4 = xorn_new_revision(rev3);
	assert(rev4 != NULL);

	err = NO_ERROR;
	assert(xornsch_add_line(rev4, NULL, &err) == NULL);
	assert(err == xorn_error_invalid_argument);

	err = NO_ERROR;
	assert(xornsch_set_line_data(rev4, ob0, NULL, &err) == -1);
	assert(err == xorn_error_invalid_argument);

	xorn_finalize_revision(rev4);

	xorn_free_revision(rev4);
	xorn_free_revision(rev3);
	xorn_free_revision(rev2);
	xorn_free_revision(rev1);
	xorn_free_revision(rev0);
	return 0;
}
