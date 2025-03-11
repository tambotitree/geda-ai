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
#include <stdlib.h>

#define E_OK      ((xorn_error_t) -1)
#define E_NOTRANS xorn_error_revision_not_transient
#define E_NOEXIST xorn_error_object_doesnt_exist
#define E_NOSUCC  xorn_error_successor_doesnt_exist


static void relocate_and_assert_3(
	xorn_revision_t rev,
	xorn_object_t ob, xorn_object_t insert_before,
	xorn_error_t expected_result,
	xorn_object_t ob0, xorn_object_t ob1, xorn_object_t ob2)
{
	xorn_error_t err;
	xorn_object_t *objects;
	size_t count;

	err = E_OK;
	assert(xorn_relocate_object(rev, ob, NULL, insert_before, &err)
		   == (expected_result == E_OK ? 0 : -1));
	assert(err == expected_result);

	assert(xorn_get_objects(rev, &objects, &count) == 0);
	assert(objects != NULL);
	assert(count == 3);
	assert(objects[0] == ob0);
	assert(objects[1] == ob1);
	assert(objects[2] == ob2);
	free(objects);
}

static void relocate_and_assert_2(
	xorn_revision_t rev,
	xorn_object_t ob, xorn_object_t insert_before,
	xorn_error_t expected_result,
	xorn_object_t ob0, xorn_object_t ob1)
{
	xorn_error_t err;
	xorn_object_t *objects;
	size_t count;

	err = E_OK;
	assert(xorn_relocate_object(rev, ob, NULL, insert_before, &err)
		   == (expected_result == E_OK ? 0 : -1));
	assert(err == expected_result);

	assert(xorn_get_objects(rev, &objects, &count) == 0);
	assert(objects != NULL);
	assert(count == 2);
	assert(objects[0] == ob0);
	assert(objects[1] == ob1);
	free(objects);
}

int main(void)
{
	xorn_revision_t rev0, rev1, rev2, rev3;
	xorn_object_t ob0, ob1a, ob1b;

	xorn_object_t *objects;
	size_t count;

	xorn_revision_t rev4;

	setup(&rev0, &rev1, &rev2, &rev3, &ob0, &ob1a, &ob1b);

	rev4 = xorn_new_revision(rev2);

	assert(xorn_get_objects(rev4, &objects, &count) == 0);
	assert(objects != NULL);
	assert(count == 3);
	assert(objects[0] == ob0);
	assert(objects[1] == ob1a);
	assert(objects[2] == ob1b);
	free(objects);

	relocate_and_assert_3(rev4, ob0,  ob0,  E_OK,      ob0, ob1a, ob1b);
	relocate_and_assert_3(rev4, ob0,  ob1a, E_OK,      ob0, ob1a, ob1b);
	relocate_and_assert_3(rev4, ob0,  ob1b, E_OK,      ob1a, ob0, ob1b);
	relocate_and_assert_3(rev4, ob0,  NULL, E_OK,      ob1a, ob1b, ob0);
	relocate_and_assert_3(rev4, ob1a, ob0,  E_OK,      ob1b, ob1a, ob0);
	relocate_and_assert_3(rev4, ob1a, ob1a, E_OK,      ob1b, ob1a, ob0);
	relocate_and_assert_3(rev4, ob1a, ob1b, E_OK,      ob1a, ob1b, ob0);
	relocate_and_assert_3(rev4, ob1a, NULL, E_OK,      ob1b, ob0, ob1a);
	relocate_and_assert_3(rev4, ob1b, ob0,  E_OK,      ob1b, ob0, ob1a);
	relocate_and_assert_3(rev4, ob1b, ob1a, E_OK,      ob0, ob1b, ob1a);
	relocate_and_assert_3(rev4, ob1b, ob1b, E_OK,      ob0, ob1b, ob1a);
	relocate_and_assert_3(rev4, ob1b, NULL, E_OK,      ob0, ob1a, ob1b);
	relocate_and_assert_3(rev4, NULL, ob0,  E_NOEXIST, ob0, ob1a, ob1b);
	relocate_and_assert_3(rev4, NULL, ob1a, E_NOEXIST, ob0, ob1a, ob1b);
	relocate_and_assert_3(rev4, NULL, ob1b, E_NOEXIST, ob0, ob1a, ob1b);
	relocate_and_assert_3(rev4, NULL, NULL, E_NOEXIST, ob0, ob1a, ob1b);

	relocate_and_assert_3(rev4, ob0,  ob0,  E_OK,      ob0, ob1a, ob1b);
	relocate_and_assert_3(rev4, ob1a, ob1a, E_OK,      ob0, ob1a, ob1b);
	relocate_and_assert_3(rev4, ob1b, ob1b, E_OK,      ob0, ob1a, ob1b);

	assert(xorn_delete_object(rev4, ob0, NULL) == 0);

	assert(xorn_get_objects(rev4, &objects, &count) == 0);
	assert(objects != NULL);
	assert(count == 2);
	assert(objects[0] == ob1a);
	assert(objects[1] == ob1b);
	free(objects);

	relocate_and_assert_2(rev4, ob0,  ob0,  E_NOEXIST, ob1a, ob1b);
	relocate_and_assert_2(rev4, ob0,  ob1a, E_NOEXIST, ob1a, ob1b);
	relocate_and_assert_2(rev4, ob0,  ob1b, E_NOEXIST, ob1a, ob1b);
	relocate_and_assert_2(rev4, ob0,  NULL, E_NOEXIST, ob1a, ob1b);
	relocate_and_assert_2(rev4, ob1a, ob0,  E_NOSUCC,  ob1a, ob1b);
	relocate_and_assert_2(rev4, ob1a, ob1a, E_OK,      ob1a, ob1b);
	relocate_and_assert_2(rev4, ob1a, ob1b, E_OK,      ob1a, ob1b);
	relocate_and_assert_2(rev4, ob1a, NULL, E_OK,      ob1b, ob1a);
	relocate_and_assert_2(rev4, ob1b, ob0,  E_NOSUCC,  ob1b, ob1a);
	relocate_and_assert_2(rev4, ob1b, ob1a, E_OK,      ob1b, ob1a);
	relocate_and_assert_2(rev4, ob1b, ob1b, E_OK,      ob1b, ob1a);
	relocate_and_assert_2(rev4, ob1b, NULL, E_OK,      ob1a, ob1b);
	relocate_and_assert_2(rev4, NULL, ob0,  E_NOEXIST, ob1a, ob1b);
	relocate_and_assert_2(rev4, NULL, ob1a, E_NOEXIST, ob1a, ob1b);
	relocate_and_assert_2(rev4, NULL, ob1b, E_NOEXIST, ob1a, ob1b);
	relocate_and_assert_2(rev4, NULL, NULL, E_NOEXIST, ob1a, ob1b);

	xorn_finalize_revision(rev4);

	relocate_and_assert_2(rev4, ob1a, ob1a, E_NOTRANS, ob1a, ob1b);
	relocate_and_assert_2(rev4, ob1a, ob1b, E_NOTRANS, ob1a, ob1b);
	relocate_and_assert_2(rev4, ob1a, NULL, E_NOTRANS, ob1a, ob1b);
	relocate_and_assert_2(rev4, ob1b, ob1a, E_NOTRANS, ob1a, ob1b);
	relocate_and_assert_2(rev4, ob1b, ob1b, E_NOTRANS, ob1a, ob1b);
	relocate_and_assert_2(rev4, ob1b, NULL, E_NOTRANS, ob1a, ob1b);

	xorn_free_revision(rev4);
	xorn_free_revision(rev3);
	xorn_free_revision(rev2);
	xorn_free_revision(rev1);
	xorn_free_revision(rev0);
	return 0;
}
