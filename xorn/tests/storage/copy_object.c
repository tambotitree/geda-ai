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

#define NO_ERROR ((xorn_error_t) -1)


int main(void)
{
	xorn_revision_t rev0, rev1, rev2, rev3;
	xorn_object_t ob0, ob1a, ob1b;

	xorn_revision_t rev4;
	xorn_object_t ob0copy;
	xorn_error_t err;

	xorn_object_t *objects;
	size_t count;

	setup(&rev0, &rev1, &rev2, &rev3, &ob0, &ob1a, &ob1b);


	/* can't copy object which doesn't exist in source revision */

	rev4 = xorn_new_revision(rev3);
	assert(rev4 != NULL);

	err = NO_ERROR;
	ob0copy = xorn_copy_object(rev4, rev0, ob0, &err);
	assert(ob0copy == NULL);
	assert(err == xorn_error_object_doesnt_exist);

	xorn_finalize_revision(rev4);

	assert(xorn_get_objects(rev4, &objects, &count) == 0);
	assert(objects != NULL);
	assert(count == 2);
	assert(objects[0] == ob0);
	assert(objects[1] == ob1b);
	free(objects);

	xorn_free_revision(rev4);


	/* can copy object otherwise */

	rev4 = xorn_new_revision(rev3);
	assert(rev4 != NULL);

	err = NO_ERROR;
	ob0copy = xorn_copy_object(rev4, rev1, ob0, &err);
	assert(ob0copy != NULL);
	assert(err == NO_ERROR);

	xorn_finalize_revision(rev4);

	assert(xorn_get_object_type(rev4, ob0copy) !=
	       xorn_get_object_type(rev4, ob0));
	assert(xorn_get_object_type(rev4, ob0copy) ==
	       xorn_get_object_type(rev1, ob0));

	assert(xorn_get_objects(rev4, &objects, &count) == 0);
	assert(objects != NULL);
	assert(count == 3);
	assert(objects[0] == ob0);
	assert(objects[1] == ob1b);
	assert(objects[2] == ob0copy);
	free(objects);

	xorn_free_revision(rev4);


	xorn_free_revision(rev3);
	xorn_free_revision(rev2);
	xorn_free_revision(rev1);
	xorn_free_revision(rev0);
	return 0;
}
