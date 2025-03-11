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


int main(void)
{
	xorn_revision_t rev0, rev1, rev2, rev3;
	xorn_object_t ob0, ob1a, ob1b;

	xorn_object_t *objects;
	size_t count;

	setup(&rev0, &rev1, &rev2, &rev3, &ob0, &ob1a, &ob1b);

	assert(xorn_get_added_objects(rev0, rev1, &objects, &count) == 0);
	assert(objects != NULL);
	assert(count == 1);
	assert(objects[0] == ob0);
	free(objects);
	assert(xorn_get_removed_objects(rev0, rev1, &objects, &count) == 0);
	assert(count == 0);
	free(objects);
	assert(xorn_get_modified_objects(rev0, rev1, &objects, &count) == 0);
	assert(count == 0);
	free(objects);

	assert(xorn_get_added_objects(rev1, rev2, &objects, &count) == 0);
	assert(objects != NULL);
	assert(count == 2);
	assert((objects[0] == ob1a && objects[1] == ob1b) ||
	       (objects[0] == ob1b && objects[1] == ob1a));
	free(objects);
	assert(xorn_get_removed_objects(rev1, rev2, &objects, &count) == 0);
	assert(count == 0);
	free(objects);
	assert(xorn_get_modified_objects(rev1, rev2, &objects, &count) == 0);
	assert(count == 0);
	free(objects);

	assert(xorn_get_added_objects(rev2, rev3, &objects, &count) == 0);
	assert(count == 0);
	free(objects);
	assert(xorn_get_removed_objects(rev2, rev3, &objects, &count) == 0);
	assert(objects != NULL);
	assert(count == 1);
	assert(objects[0] == ob1a);
	free(objects);
	assert(xorn_get_modified_objects(rev2, rev3, &objects, &count) == 0);
	assert(objects != NULL);
	assert(count == 1);
	assert(objects[0] == ob0);
	free(objects);

	xorn_free_revision(rev3);
	xorn_free_revision(rev2);
	xorn_free_revision(rev1);
	xorn_free_revision(rev0);
	return 0;
}
