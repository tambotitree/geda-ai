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

#include "module.h"


PyObject *build_object(xorn_object_t ob)
{
	Object *self = (Object *)ObjectType.tp_alloc(&ObjectType, 0);
	if (self == NULL)
		return NULL;

	self->ob = ob;
	return (PyObject *)self;
}

static void Object_dealloc(Object *self)
{
	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *Object_richcompare(PyObject *a, PyObject *b, int op)
{
    if (!PyObject_TypeCheck(a, &ObjectType) || !PyObject_TypeCheck(b, &ObjectType)) {
        Py_RETURN_NOTIMPLEMENTED;
    }

    xorn_object_t ob_a = ((Object *)a)->ob;
    xorn_object_t ob_b = ((Object *)b)->ob;

    int cmp;
    if (ob_a < ob_b) cmp = -1;
    else if (ob_a > ob_b) cmp = 1;
    else cmp = 0;

    bool result = false;
    switch (op) {
        case Py_LT: result = (cmp < 0); break;
        case Py_LE: result = (cmp <= 0); break;
        case Py_EQ: result = (cmp == 0); break;
        case Py_NE: result = (cmp != 0); break;
        case Py_GT: result = (cmp > 0); break;
        case Py_GE: result = (cmp >= 0); break;
        default:
            Py_RETURN_NOTIMPLEMENTED;
    }

    if (result)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

static long Object_hash(Object *self)
{
	return _Py_HashPointer(self->ob);
}

PyTypeObject ObjectType = {
    .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "xorn.storage.Object",
    .tp_basicsize = sizeof(Object),
    .tp_dealloc = (destructor)Object_dealloc,
    .tp_hash = (hashfunc)Object_hash,
    .tp_richcompare = Object_richcompare,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "The identity of an object across revisions.",
};