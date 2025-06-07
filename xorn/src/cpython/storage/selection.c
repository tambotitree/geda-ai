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


PyObject *build_selection(xorn_selection_t sel)
{
	Selection *self =
		(Selection *)SelectionType.tp_alloc(&SelectionType, 0);
	if (self == NULL)
		return NULL;

	self->sel = sel;
	return (PyObject *)self;
}

static void Selection_dealloc(Selection *self)
{
	xorn_free_selection(self->sel);
	Py_TYPE(self)->tp_free((PyObject *)self);
}

PyTypeObject SelectionType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "xorn.storage.Selection",
    .tp_basicsize = sizeof(Selection),
    .tp_itemsize = 0,

    /* Standard operations */
    .tp_dealloc = (destructor)Selection_dealloc,
    .tp_getattr = NULL,
    .tp_setattr = NULL,
    .tp_as_async = NULL,           /* Python 3+: replaces tp_compare/cmpfunc */
    .tp_repr = NULL,

    /* Numeric / Sequence / Mapping protocol */
    .tp_as_number = NULL,
    .tp_as_sequence = NULL,
    .tp_as_mapping = NULL,

    /* Object methods */
    .tp_hash = NULL,
    .tp_call = NULL,
    .tp_str = NULL,
    .tp_getattro = NULL,
    .tp_setattro = NULL,

    /* Buffer interface */
    .tp_as_buffer = NULL,

    /* Type flags */
    .tp_flags = Py_TPFLAGS_DEFAULT,

    /* Docstring */
    .tp_doc = PyDoc_STR("The identity of a set of objects across revisions."),

    /* GC and comparison */
    .tp_traverse = NULL,
    .tp_clear = NULL,
    .tp_richcompare = NULL,
    .tp_weaklistoffset = 0,

    /* Iterators */
    .tp_iter = NULL,
    .tp_iternext = NULL,

    /* Class attributes */
    .tp_methods = NULL,
    .tp_members = NULL,
    .tp_getset = NULL,
    .tp_base = NULL,
    .tp_dict = NULL,
    .tp_descr_get = NULL,
    .tp_descr_set = NULL,
    .tp_dictoffset = 0,
    .tp_init = NULL,
    .tp_alloc = NULL,
    .tp_new = NULL,
    .tp_free = NULL,
    .tp_is_gc = NULL,

    /* Python 2.2+ subclassing internals */
    .tp_bases = NULL,
    .tp_mro = NULL,
    .tp_cache = NULL,
    .tp_subclasses = NULL,
    .tp_weaklist = NULL,
    .tp_del = NULL,

    /* Python 2.6+ */
    .tp_version_tag = 0,

    /* Python 3.4+ */
#if PY_VERSION_HEX >= 0x03040000
    .tp_finalize = NULL,
#endif

    /* Python 3.8+ */
#if PY_VERSION_HEX >= 0x03080000
    .tp_vectorcall = NULL,
#endif
};