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

static struct xornsch_line line_data;
static struct xornsch_box box_data;
static struct xornsch_circle circle_data;
static struct xornsch_net net_data;


static void setup(
	xorn_revision_t *rev0, xorn_revision_t *rev1,
	xorn_revision_t *rev2, xorn_revision_t *rev3,
	xorn_object_t *ob0, xorn_object_t *ob1a, xorn_object_t *ob1b)
{
	*rev0 = xorn_new_revision(NULL);
	assert(*rev0 != NULL);
	xorn_finalize_revision(*rev0);

	/* first change */

	*rev1 = xorn_new_revision(*rev0);
	assert(*rev1 != NULL);

	memset(&line_data, 0, sizeof line_data);
	line_data.pos.x = 0;
	line_data.pos.y = 1;
	line_data.size.x = 3;
	line_data.size.y = 2;
	line_data.color = 3;
	line_data.line.width = 1;

	*ob0 = xornsch_add_line(*rev1, &line_data, NULL);
	assert(*ob0 != NULL);

	xorn_finalize_revision(*rev1);

	/* second change */

	*rev2 = xorn_new_revision(*rev1);
	assert(*rev2 != NULL);

	memset(&box_data, 0, sizeof box_data);
	box_data.pos.x = 1;
	box_data.pos.y = 1;
	box_data.size.x = 2;
	box_data.size.y = 2;
	box_data.color = 3;
	box_data.line.width = 1;

	*ob1a = xornsch_add_box(*rev2, &box_data, NULL);
	assert(*ob1a != NULL);

	memset(&circle_data, 0, sizeof circle_data);
	circle_data.pos.x = -1;
	circle_data.pos.y = -1;
	circle_data.radius = 2;
	circle_data.color = 3;
	circle_data.line.width = 1;
	circle_data.fill.type = 1;

	*ob1b = xornsch_add_circle(*rev2, &circle_data, NULL);
	assert(*ob1b != NULL);

	xorn_finalize_revision(*rev2);

	/* third change */

	*rev3 = xorn_new_revision(*rev2);
	assert(*rev3 != NULL);

	memset(&net_data, 0, sizeof net_data);
	net_data.pos.x = 0;
	net_data.pos.y = 1;
	net_data.size.x = 3;
	net_data.size.y = 2;
	net_data.color = 4;

	assert(xornsch_set_net_data(*rev3, *ob0, &net_data, NULL) == 0);

	assert(xorn_delete_object(*rev3, *ob1a, NULL) == 0);

	xorn_finalize_revision(*rev3);
}
