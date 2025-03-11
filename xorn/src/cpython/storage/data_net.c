/* Copyright (C) 2013-2020 Roland Lutz

   AUTOMATICALLY GENERATED FROM data_net.m4 -- DO NOT EDIT

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

#include "data.h"
#include <structmember.h>


PyObject *construct_net(const struct xornsch_net *data)
{
	PyObject *no_args = PyTuple_New(0);
	Net *self = (Net *)PyObject_CallObject(
		(PyObject *)&NetType, no_args);
	Py_DECREF(no_args);

	if (self == NULL)
		return NULL;

	self->data = *data;
	return (PyObject *)self;
}

void prepare_net(Net *self,
	xorn_obtype_t *type_return, const void **data_return)
{
	*type_return = xornsch_obtype_net;
	*data_return = &self->data;
}

static PyObject *Net_new(
	PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	Net *self = (Net *)type->tp_alloc(type, 0);
	if (self == NULL)
		return NULL;
	return (PyObject *)self;
}

static int Net_init(Net *self, PyObject *args, PyObject *kwds)
{
	double x_arg = 0., y_arg = 0.;
	double width_arg = 0., height_arg = 0.;
	int color_arg = 0;
	PyObject *is_bus_arg = NULL;
	PyObject *is_pin_arg = NULL;
	PyObject *is_inverted_arg = NULL;

	static char *kwlist[] = {
		"x", "y",
		"width", "height",
		"color",
		"is_bus",
		"is_pin",
		"is_inverted",
		NULL
	};

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "|ddddiOOO:Net", kwlist,
		    &x_arg, &y_arg,
		    &width_arg, &height_arg,
		    &color_arg,
		    &is_bus_arg,
		    &is_pin_arg,
		    &is_inverted_arg))
		return -1;

	int is_bus = 0;
	if (is_bus_arg != NULL) {
		is_bus = PyObject_IsTrue(is_bus_arg);
		if (is_bus == -1)
			return -1;
	}
	int is_pin = 0;
	if (is_pin_arg != NULL) {
		is_pin = PyObject_IsTrue(is_pin_arg);
		if (is_pin == -1)
			return -1;
	}
	int is_inverted = 0;
	if (is_inverted_arg != NULL) {
		is_inverted = PyObject_IsTrue(is_inverted_arg);
		if (is_inverted == -1)
			return -1;
	}

	self->data.pos.x = x_arg;
	self->data.pos.y = y_arg;
	self->data.size.x = width_arg;
	self->data.size.y = height_arg;
	self->data.color = color_arg;
	self->data.is_bus = !!is_bus;
	self->data.is_pin = !!is_pin;
	self->data.is_inverted = !!is_inverted;

	return 0;
}

static void Net_dealloc(Net *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyMemberDef Net_members[] = {
	{ "x", T_DOUBLE, offsetof(Net, data.pos.x), 0,
	  PyDoc_STR("") },
	{ "y", T_DOUBLE, offsetof(Net, data.pos.y), 0,
	  PyDoc_STR("") },
	{ "width", T_DOUBLE, offsetof(Net, data.size.x), 0,
	  PyDoc_STR("") },
	{ "height", T_DOUBLE, offsetof(Net, data.size.y), 0,
	  PyDoc_STR("") },
	{ "color", T_INT, offsetof(Net, data.color), 0,
	  PyDoc_STR("") },
	{ "is_bus", T_BOOL, offsetof(Net, data.is_bus), 0,
	  PyDoc_STR("") },
	{ "is_pin", T_BOOL, offsetof(Net, data.is_pin), 0,
	  PyDoc_STR("") },
	{ "is_inverted", T_BOOL, offsetof(Net, data.is_inverted), 0,
	  PyDoc_STR("") },
	{ NULL, 0, 0, 0, NULL }  /* Sentinel */
};

PyTypeObject NetType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/

	/* For printing, in format "<module>.<name>" */
	"xorn.storage.Net",		/* const char *tp_name */

	/* For allocation */
	sizeof(Net),			/* Py_ssize_t tp_basicsize */
	0,				/* Py_ssize_t tp_itemsize */

	/* Methods to implement standard operations */
	(destructor)Net_dealloc,	/* destructor tp_dealloc */
	NULL,				/* printfunc tp_print */
	NULL,				/* getattrfunc tp_getattr */
	NULL,				/* setattrfunc tp_setattr */
	NULL,				/* cmpfunc tp_compare */
	NULL,				/* reprfunc tp_repr */

	/* Method suites for standard classes */
	NULL,				/* PyNumberMethods *tp_as_number */
	NULL,				/* PySequenceMethods *tp_as_sequence */
	NULL,				/* PyMappingMethods *tp_as_mapping */

	/* More standard operations (here for binary compatibility) */
	NULL,				/* hashfunc tp_hash */
	NULL,				/* ternaryfunc tp_call */
	NULL,				/* reprfunc tp_str */
	NULL,				/* getattrofunc tp_getattro */
	NULL,				/* setattrofunc tp_setattro */

	/* Functions to access object as input/output buffer */
	NULL,				/* PyBufferProcs *tp_as_buffer */

	/* Flags to define presence of optional/expanded features */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
					/* long tp_flags */

	/* Documentation string */
	PyDoc_STR("Schematic net segment, bus segment, or pin."),
					/* const char *tp_doc */

	/* Assigned meaning in release 2.0 */
	/* call function for all accessible objects */
	NULL,				/* traverseproc tp_traverse */

	/* delete references to contained objects */
	NULL,				/* inquiry tp_clear */

	/* Assigned meaning in release 2.1 */
	/* rich comparisons */
	NULL,				/* richcmpfunc tp_richcompare */

	/* weak reference enabler */
	0,				/* Py_ssize_t tp_weaklistoffset */

	/* Added in release 2.2 */
	/* Iterators */
	NULL,				/* getiterfunc tp_iter */
	NULL,				/* iternextfunc tp_iternext */

	/* Attribute descriptor and subclassing stuff */
	NULL,				/* struct PyMethodDef *tp_methods */
	Net_members,		/* struct PyMemberDef *tp_members */
	NULL,				/* struct PyGetSetDef *tp_getset */
	NULL,				/* struct _typeobject *tp_base */
	NULL,				/* PyObject *tp_dict */
	NULL,				/* descrgetfunc tp_descr_get */
	NULL,				/* descrsetfunc tp_descr_set */
	0,				/* Py_ssize_t tp_dictoffset */
	(initproc)Net_init,	/* initproc tp_init */
	NULL,				/* allocfunc tp_alloc */
	Net_new,			/* newfunc tp_new */
	NULL,		/* freefunc tp_free--Low-level free-memory routine */
	NULL,		/* inquiry tp_is_gc--For PyObject_IS_GC */
	NULL,				/* PyObject *tp_bases */
	NULL,		/* PyObject *tp_mro--method resolution order */
	NULL,				/* PyObject *tp_cache */
	NULL,				/* PyObject *tp_subclasses */
	NULL,				/* PyObject *tp_weaklist */
	NULL,				/* destructor tp_del */

	/* Type attribute cache version tag. Added in version 2.6 */
	0,				/* unsigned int tp_version_tag */
};
