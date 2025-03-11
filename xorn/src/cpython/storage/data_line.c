/* Copyright (C) 2013-2020 Roland Lutz

   AUTOMATICALLY GENERATED FROM data_line.m4 -- DO NOT EDIT

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


PyObject *construct_line(const struct xornsch_line *data)
{
	PyObject *no_args = PyTuple_New(0);
	Line *self = (Line *)PyObject_CallObject(
		(PyObject *)&LineType, no_args);
	Py_DECREF(no_args);

	if (self == NULL)
		return NULL;

	self->data = *data;
	((LineAttr *)self->line)->data = data->line;
	return (PyObject *)self;
}

void prepare_line(Line *self,
	xorn_obtype_t *type_return, const void **data_return)
{
	self->data.line = ((LineAttr *)self->line)->data;
	*type_return = xornsch_obtype_line;
	*data_return = &self->data;
}

static PyObject *Line_new(
	PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	Line *self = (Line *)type->tp_alloc(type, 0);
	if (self == NULL)
		return NULL;

	PyObject *no_args = PyTuple_New(0);
	self->line = PyObject_CallObject((PyObject *)&LineAttrType, no_args);
	Py_DECREF(no_args);

	if (self->line == NULL) {
		Py_DECREF(self);
		return NULL;
	}
	return (PyObject *)self;
}

static int Line_init(Line *self, PyObject *args, PyObject *kwds)
{
	double x_arg = 0., y_arg = 0.;
	double width_arg = 0., height_arg = 0.;
	int color_arg = 0;
	PyObject *line_arg = NULL;

	static char *kwlist[] = {
		"x", "y",
		"width", "height",
		"color",
		"line",
		NULL
	};

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "|ddddiO:Line", kwlist,
		    &x_arg, &y_arg,
		    &width_arg, &height_arg,
		    &color_arg,
		    &line_arg))
		return -1;

	if (line_arg != NULL && !PyObject_TypeCheck(line_arg, &LineAttrType)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ,
			 "line attribute must be %.50s, not %.50s",
			 LineAttrType.tp_name, line_arg->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return -1;
	}

	self->data.pos.x = x_arg;
	self->data.pos.y = y_arg;
	self->data.size.x = width_arg;
	self->data.size.y = height_arg;
	self->data.color = color_arg;
	if (line_arg != NULL) {
		Py_INCREF(line_arg);
		Py_DECREF(self->line);
		self->line = line_arg;
	}

	return 0;
}

static int Line_traverse(Line *self, visitproc visit, void *arg)
{
	Py_VISIT(self->line);
	return 0;
}

static int Line_clear(Line *self)
{
	Py_CLEAR(self->line);
	return 0;
}

static void Line_dealloc(Line *self)
{
	Line_clear(self);
	self->ob_type->tp_free((PyObject *)self);
}

static PyMemberDef Line_members[] = {
	{ "x", T_DOUBLE, offsetof(Line, data.pos.x), 0,
	  PyDoc_STR("") },
	{ "y", T_DOUBLE, offsetof(Line, data.pos.y), 0,
	  PyDoc_STR("") },
	{ "width", T_DOUBLE, offsetof(Line, data.size.x), 0,
	  PyDoc_STR("") },
	{ "height", T_DOUBLE, offsetof(Line, data.size.y), 0,
	  PyDoc_STR("") },
	{ "color", T_INT, offsetof(Line, data.color), 0,
	  PyDoc_STR("") },
	{ NULL, 0, 0, 0, NULL }  /* Sentinel */
};

static PyObject *Line_getline(Line *self, void *closure)
{
	Py_INCREF(self->line);
	return self->line;
}

static int Line_setline(Line *self, PyObject *value, void *closure)
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

static PyGetSetDef Line_getset[] = {
	{ "line", (getter)Line_getline, (setter)Line_setline,
	  PyDoc_STR(""), NULL },
	{ NULL, NULL, NULL, NULL, NULL }  /* Sentinel */
};

PyTypeObject LineType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/

	/* For printing, in format "<module>.<name>" */
	"xorn.storage.Line",		/* const char *tp_name */

	/* For allocation */
	sizeof(Line),			/* Py_ssize_t tp_basicsize */
	0,				/* Py_ssize_t tp_itemsize */

	/* Methods to implement standard operations */
	(destructor)Line_dealloc,	/* destructor tp_dealloc */
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
	PyDoc_STR("Schematic line."),
					/* const char *tp_doc */

	/* Assigned meaning in release 2.0 */
	/* call function for all accessible objects */
	(traverseproc)Line_traverse,/* traverseproc tp_traverse */

	/* delete references to contained objects */
	(inquiry)Line_clear,	/* inquiry tp_clear */

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
	Line_members,		/* struct PyMemberDef *tp_members */
	Line_getset,		/* struct PyGetSetDef *tp_getset */
	NULL,				/* struct _typeobject *tp_base */
	NULL,				/* PyObject *tp_dict */
	NULL,				/* descrgetfunc tp_descr_get */
	NULL,				/* descrsetfunc tp_descr_set */
	0,				/* Py_ssize_t tp_dictoffset */
	(initproc)Line_init,	/* initproc tp_init */
	NULL,				/* allocfunc tp_alloc */
	Line_new,			/* newfunc tp_new */
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
