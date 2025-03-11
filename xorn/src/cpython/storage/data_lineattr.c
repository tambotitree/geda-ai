/* Copyright (C) 2013-2020 Roland Lutz

   AUTOMATICALLY GENERATED FROM data_lineattr.m4 -- DO NOT EDIT

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


PyObject *construct_line_attr(const struct xornsch_line_attr *data)
{
	PyObject *no_args = PyTuple_New(0);
	LineAttr *self = (LineAttr *)PyObject_CallObject(
		(PyObject *)&LineAttrType, no_args);
	Py_DECREF(no_args);

	if (self == NULL)
		return NULL;

	self->data = *data;
	return (PyObject *)self;
}

static PyObject *LineAttr_new(
	PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	LineAttr *self = (LineAttr *)type->tp_alloc(type, 0);
	if (self == NULL)
		return NULL;
	return (PyObject *)self;
}

static int LineAttr_init(LineAttr *self, PyObject *args, PyObject *kwds)
{
	double width_arg = 0.;
	int cap_style_arg = 0;
	int dash_style_arg = 0;
	double dash_length_arg = 0.;
	double dash_space_arg = 0.;

	static char *kwlist[] = {
		"width",
		"cap_style",
		"dash_style",
		"dash_length",
		"dash_space",
		NULL
	};

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "|diidd:LineAttr", kwlist,
		    &width_arg,
		    &cap_style_arg,
		    &dash_style_arg,
		    &dash_length_arg,
		    &dash_space_arg))
		return -1;


	self->data.width = width_arg;
	self->data.cap_style = cap_style_arg;
	self->data.dash_style = dash_style_arg;
	self->data.dash_length = dash_length_arg;
	self->data.dash_space = dash_space_arg;

	return 0;
}

static void LineAttr_dealloc(LineAttr *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyMemberDef LineAttr_members[] = {
	{ "width", T_DOUBLE, offsetof(LineAttr, data.width), 0,
	  PyDoc_STR("") },
	{ "cap_style", T_INT, offsetof(LineAttr, data.cap_style), 0,
	  PyDoc_STR("") },
	{ "dash_style", T_INT, offsetof(LineAttr, data.dash_style), 0,
	  PyDoc_STR("") },
	{ "dash_length", T_DOUBLE, offsetof(LineAttr, data.dash_length), 0,
	  PyDoc_STR("") },
	{ "dash_space", T_DOUBLE, offsetof(LineAttr, data.dash_space), 0,
	  PyDoc_STR("") },
	{ NULL, 0, 0, 0, NULL }  /* Sentinel */
};

PyTypeObject LineAttrType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/

	/* For printing, in format "<module>.<name>" */
	"xorn.storage.LineAttr",		/* const char *tp_name */

	/* For allocation */
	sizeof(LineAttr),			/* Py_ssize_t tp_basicsize */
	0,				/* Py_ssize_t tp_itemsize */

	/* Methods to implement standard operations */
	(destructor)LineAttr_dealloc,	/* destructor tp_dealloc */
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
	PyDoc_STR("Schematic line style."),
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
	LineAttr_members,		/* struct PyMemberDef *tp_members */
	NULL,				/* struct PyGetSetDef *tp_getset */
	NULL,				/* struct _typeobject *tp_base */
	NULL,				/* PyObject *tp_dict */
	NULL,				/* descrgetfunc tp_descr_get */
	NULL,				/* descrsetfunc tp_descr_set */
	0,				/* Py_ssize_t tp_dictoffset */
	(initproc)LineAttr_init,	/* initproc tp_init */
	NULL,				/* allocfunc tp_alloc */
	LineAttr_new,			/* newfunc tp_new */
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
