/* Copyright (C) 2013-2020 Roland Lutz

   AUTOMATICALLY GENERATED FROM data_circle.m4 -- DO NOT EDIT

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


PyObject *construct_circle(const struct xornsch_circle *data)
{
	PyObject *no_args = PyTuple_New(0);
	Circle *self = (Circle *)PyObject_CallObject(
		(PyObject *)&CircleType, no_args);
	Py_DECREF(no_args);

	if (self == NULL)
		return NULL;

	self->data = *data;
	((LineAttr *)self->line)->data = data->line;
	((FillAttr *)self->fill)->data = data->fill;
	return (PyObject *)self;
}

void prepare_circle(Circle *self,
	xorn_obtype_t *type_return, const void **data_return)
{
	self->data.line = ((LineAttr *)self->line)->data;
	self->data.fill = ((FillAttr *)self->fill)->data;
	*type_return = xornsch_obtype_circle;
	*data_return = &self->data;
}

static PyObject *Circle_new(
	PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	Circle *self = (Circle *)type->tp_alloc(type, 0);
	if (self == NULL)
		return NULL;

	PyObject *no_args = PyTuple_New(0);
	self->line = PyObject_CallObject((PyObject *)&LineAttrType, no_args);
	self->fill = PyObject_CallObject((PyObject *)&FillAttrType, no_args);
	Py_DECREF(no_args);

	if (self->line == NULL) {
		Py_DECREF(self);
		return NULL;
	}
	if (self->fill == NULL) {
		Py_DECREF(self);
		return NULL;
	}
	return (PyObject *)self;
}

static int Circle_init(Circle *self, PyObject *args, PyObject *kwds)
{
	double x_arg = 0., y_arg = 0.;
	double radius_arg = 0.;
	int color_arg = 0;
	PyObject *line_arg = NULL;
	PyObject *fill_arg = NULL;

	static char *kwlist[] = {
		"x", "y",
		"radius",
		"color",
		"line",
		"fill",
		NULL
	};

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "|dddiOO:Circle", kwlist,
		    &x_arg, &y_arg,
		    &radius_arg,
		    &color_arg,
		    &line_arg,
		    &fill_arg))
		return -1;

	if (line_arg != NULL && !PyObject_TypeCheck(line_arg, &LineAttrType)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ,
			 "line attribute must be %.50s, not %.50s",
			 LineAttrType.tp_name, line_arg->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return -1;
	}
	if (fill_arg != NULL && !PyObject_TypeCheck(fill_arg, &FillAttrType)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ,
			 "fill attribute must be %.50s, not %.50s",
			 FillAttrType.tp_name, fill_arg->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return -1;
	}

	self->data.pos.x = x_arg;
	self->data.pos.y = y_arg;
	self->data.radius = radius_arg;
	self->data.color = color_arg;
	if (line_arg != NULL) {
		Py_INCREF(line_arg);
		Py_DECREF(self->line);
		self->line = line_arg;
	}
	if (fill_arg != NULL) {
		Py_INCREF(fill_arg);
		Py_DECREF(self->fill);
		self->fill = fill_arg;
	}

	return 0;
}

static int Circle_traverse(Circle *self, visitproc visit, void *arg)
{
	Py_VISIT(self->line);
	Py_VISIT(self->fill);
	return 0;
}

static int Circle_clear(Circle *self)
{
	Py_CLEAR(self->line);
	Py_CLEAR(self->fill);
	return 0;
}

static void Circle_dealloc(Circle *self)
{
	Circle_clear(self);
	self->ob_type->tp_free((PyObject *)self);
}

static PyMemberDef Circle_members[] = {
	{ "x", T_DOUBLE, offsetof(Circle, data.pos.x), 0,
	  PyDoc_STR("") },
	{ "y", T_DOUBLE, offsetof(Circle, data.pos.y), 0,
	  PyDoc_STR("") },
	{ "radius", T_DOUBLE, offsetof(Circle, data.radius), 0,
	  PyDoc_STR("") },
	{ "color", T_INT, offsetof(Circle, data.color), 0,
	  PyDoc_STR("") },
	{ NULL, 0, 0, 0, NULL }  /* Sentinel */
};

static PyObject *Circle_getline(Circle *self, void *closure)
{
	Py_INCREF(self->line);
	return self->line;
}

static int Circle_setline(Circle *self, PyObject *value, void *closure)
{
	if (value == NULL) {
		PyErr_SetString(PyExc_TypeError,
				"line attribute cannot be deleted");
		return -1;
	}

	if (!PyObject_TypeCheck(value, &LineAttrType)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ,
			 "line attribute must be %.50s, not %.50s",
			 LineAttrType.tp_name, value->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return -1;
	}

	Py_INCREF(value);
	Py_DECREF(self->line);
	self->line = value;
	return 0;
}

static PyObject *Circle_getfill(Circle *self, void *closure)
{
	Py_INCREF(self->fill);
	return self->fill;
}

static int Circle_setfill(Circle *self, PyObject *value, void *closure)
{
	if (value == NULL) {
		PyErr_SetString(PyExc_TypeError,
				"fill attribute cannot be deleted");
		return -1;
	}

	if (!PyObject_TypeCheck(value, &FillAttrType)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ,
			 "fill attribute must be %.50s, not %.50s",
			 FillAttrType.tp_name, value->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return -1;
	}

	Py_INCREF(value);
	Py_DECREF(self->fill);
	self->fill = value;
	return 0;
}

static PyGetSetDef Circle_getset[] = {
	{ "line", (getter)Circle_getline, (setter)Circle_setline,
	  PyDoc_STR(""), NULL },
	{ "fill", (getter)Circle_getfill, (setter)Circle_setfill,
	  PyDoc_STR(""), NULL },
	{ NULL, NULL, NULL, NULL, NULL }  /* Sentinel */
};

PyTypeObject CircleType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/

	/* For printing, in format "<module>.<name>" */
	"xorn.storage.Circle",		/* const char *tp_name */

	/* For allocation */
	sizeof(Circle),			/* Py_ssize_t tp_basicsize */
	0,				/* Py_ssize_t tp_itemsize */

	/* Methods to implement standard operations */
	(destructor)Circle_dealloc,	/* destructor tp_dealloc */
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
					/* long tp_flags */

	/* Documentation string */
	PyDoc_STR("Schematic circle."),
					/* const char *tp_doc */

	/* Assigned meaning in release 2.0 */
	/* call function for all accessible objects */
	(traverseproc)Circle_traverse,/* traverseproc tp_traverse */

	/* delete references to contained objects */
	(inquiry)Circle_clear,	/* inquiry tp_clear */

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
	Circle_members,		/* struct PyMemberDef *tp_members */
	Circle_getset,		/* struct PyGetSetDef *tp_getset */
	NULL,				/* struct _typeobject *tp_base */
	NULL,				/* PyObject *tp_dict */
	NULL,				/* descrgetfunc tp_descr_get */
	NULL,				/* descrsetfunc tp_descr_set */
	0,				/* Py_ssize_t tp_dictoffset */
	(initproc)Circle_init,	/* initproc tp_init */
	NULL,				/* allocfunc tp_alloc */
	Circle_new,			/* newfunc tp_new */
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
