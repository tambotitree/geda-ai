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

static struct xornsch_line line_data;
static struct xornsch_box box_data;


static void try_modify(xorn_revision_t rev, xorn_object_t existing_ob,
		       bool expected_result)
{
	xorn_object_t ob0, ob1;
	xorn_selection_t sel0, sel1;
	xorn_error_t err;
	xorn_error_t expected_err = expected_result
		? NO_ERROR : xorn_error_revision_not_transient;

	err = NO_ERROR;
	ob0 = xornsch_add_line(rev, &line_data, &err);
	assert((ob0 != NULL) == expected_result);
	assert(err == expected_err);

	err = NO_ERROR;
	assert((xornsch_set_box_data(rev, existing_ob, &box_data, &err) == 0)
		   == expected_result);
	assert(err == expected_err);

	err = NO_ERROR;
	ob1 = xorn_copy_object(rev, rev, existing_ob, &err);
	assert((ob1 != NULL) == expected_result);
	assert(err == expected_err);

	xorn_object_t del_ob = expected_result ? ob1 : existing_ob;
	err = NO_ERROR;
	assert((xorn_delete_object(rev, del_ob, &err) == 0)
		   == expected_result);
	assert(err == expected_err);
	assert(xorn_object_exists_in_revision(rev, del_ob)
		   == !expected_result);

	sel0 = xorn_select_all(rev);
	assert(sel0 != NULL);

	err = NO_ERROR;
	sel1 = xorn_copy_objects(rev, rev, sel0, &err);
	assert((sel1 != NULL) == expected_result);
	assert(err == expected_err);
	xorn_free_selection(sel1);

	err = NO_ERROR;
	assert((xorn_delete_selected_objects(rev, sel0, &err) == 0)
		   == expected_result);
	assert(err == expected_err);
	assert(xorn_object_exists_in_revision(rev, existing_ob)
		   == !expected_result);
	xorn_free_selection(sel0);
}

int main(void)
{
	xorn_revision_t rev0, rev1, rev2;
	xorn_object_t ob;

	memset(&line_data, 0, sizeof line_data);
	line_data.pos.x = 0;
	line_data.pos.y = 1;
	line_data.size.x = 3;
	line_data.size.y = 2;
	line_data.color = 3;
	line_data.line.width = 1;

	memset(&box_data, 0, sizeof box_data);
	box_data.pos.x = 1;
	box_data.pos.y = 1;
	box_data.size.x = 2;
	box_data.size.y = 2;
	box_data.color = 3;
	box_data.line.width = 1;

	rev0 = xorn_new_revision(NULL);
	assert(rev0 != NULL);
	assert(xorn_revision_is_transient(rev0) == true);
	xorn_finalize_revision(rev0);
	assert(xorn_revision_is_transient(rev0) == false);

	rev1 = xorn_new_revision(rev0);
	assert(rev1 != NULL);
	assert(xorn_revision_is_transient(rev1) == true);

	ob = xornsch_add_line(rev1, &line_data, NULL);
	assert(ob != NULL);

	xorn_finalize_revision(rev1);
	assert(xorn_revision_is_transient(rev1) == false);

	try_modify(rev1, ob, false);

	rev2 = xorn_new_revision(rev1);
	assert(rev2 != NULL);
	assert(xorn_revision_is_transient(rev2) == true);

	try_modify(rev2, ob, true);

	xorn_finalize_revision(rev2);
	assert(xorn_revision_is_transient(rev2) == false);

	xorn_free_revision(rev2);
	xorn_free_revision(rev1);
	xorn_free_revision(rev0);
	return 0;
}
