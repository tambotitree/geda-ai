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

#include <xornstorage.h>
#include <assert.h>
#include <string.h>

#define NO_ERROR ((xorn_error_t) -1)


int main(void)
{
	xorn_revision_t rev0, rev1, rev2;
	struct xornsch_line line_data;
	xorn_object_t ob;
	xorn_error_t err;

	rev0 = xorn_new_revision(NULL);
	assert(rev0 != NULL);
	xorn_finalize_revision(rev0);

	rev1 = xorn_new_revision(rev0);
	assert(rev1 != NULL);

	memset(&line_data, 0, sizeof line_data);
	line_data.pos.x = 0;
	line_data.pos.y = 1;
	line_data.size.x = 3;
	line_data.size.y = 2;
	line_data.color = 3;
	line_data.line.width = 1;

	err = NO_ERROR;
	ob = xorn_add_object(rev1, xorn_obtype_none, &line_data, &err);
	assert(ob == NULL);
	assert(err == xorn_error_invalid_argument);

	err = NO_ERROR;
	ob = xorn_add_object(rev1, xornsch_obtype_line, &line_data, &err);
	assert(ob != NULL);
	assert(err == NO_ERROR);

	xorn_finalize_revision(rev1);

	rev2 = xorn_new_revision(rev1);
	assert(rev2 != NULL);

	err = NO_ERROR;
	assert(xorn_set_object_data(
		       rev2, ob, xorn_obtype_none, &line_data, &err) == -1);
	assert(err == xorn_error_invalid_argument);

	err = NO_ERROR;
	assert(xorn_set_object_data(
		       rev2, ob, xornsch_obtype_line, &line_data, &err) == 0);
	assert(err == NO_ERROR);

	xorn_finalize_revision(rev2);

	xorn_free_revision(rev2);
	xorn_free_revision(rev1);
	xorn_free_revision(rev0);
	return 0;
}
