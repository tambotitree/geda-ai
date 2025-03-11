/* Copyright (C) 2013-2020 Roland Lutz

   AUTOMATICALLY GENERATED FROM data_text.m4 -- DO NOT EDIT

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


PyObject *construct_text(const struct xornsch_text *data)
{
	PyObject *no_args = PyTuple_New(0);
	Text *self = (Text *)PyObject_CallObject(
		(PyObject *)&TextType, no_args);
	Py_DECREF(no_args);

	if (self == NULL)
		return NULL;

	self->data = *data;

	if (data->text.len != 0) {
		Py_DECREF(self->text);
		self->text = PyString_FromStringAndSize(
			data->text.s, data->text.len);
		if (self->text == NULL) {
			Py_DECREF(self);
			return NULL;
		}
	}
	return (PyObject *)self;
}

void prepare_text(Text *self,
	xorn_obtype_t *type_return, const void **data_return)
{
	self->data.text.s = PyString_AS_STRING(self->text);
	self->data.text.len = PyString_GET_SIZE(self->text);
	*type_return = xornsch_obtype_text;
	*data_return = &self->data;
}

static PyObject *Text_new(
	PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	Text *self = (Text *)type->tp_alloc(type, 0);
	if (self == NULL)
		return NULL;

	PyObject *no_args = PyTuple_New(0);
	self->text = PyString_FromString("");
	Py_DECREF(no_args);

	if (self->text == NULL) {
		Py_DECREF(self);
		return NULL;
	}
	return (PyObject *)self;
}

static int Text_init(Text *self, PyObject *args, PyObject *kwds)
{
	double x_arg = 0., y_arg = 0.;
	int color_arg = 0;
	int text_size_arg = 0;
	PyObject *visibility_arg = NULL;
	int show_name_value_arg = 0;
	int angle_arg = 0;
	int alignment_arg = 0;
	PyObject *text_arg = NULL;

	static char *kwlist[] = {
		"x", "y",
		"color",
		"text_size",
		"visibility",
		"show_name_value",
		"angle",
		"alignment",
		"text",
		NULL
	};

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "|ddiiOiiiO:Text", kwlist,
		    &x_arg, &y_arg,
		    &color_arg,
		    &text_size_arg,
		    &visibility_arg,
		    &show_name_value_arg,
		    &angle_arg,
		    &alignment_arg,
		    &text_arg))
		return -1;

	int visibility = 0;
	if (visibility_arg != NULL) {
		visibility = PyObject_IsTrue(visibility_arg);
		if (visibility == -1)
			return -1;
	}
	if (text_arg != NULL && !PyString_Check(text_arg)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ,
			 "text attribute must be %.50s, not %.50s",
			 PyString_Type.tp_name, text_arg->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return -1;
	}

	self->data.pos.x = x_arg;
	self->data.pos.y = y_arg;
	self->data.color = color_arg;
	self->data.text_size = text_size_arg;
	self->data.visibility = !!visibility;
	self->data.show_name_value = show_name_value_arg;
	self->data.angle = angle_arg;
	self->data.alignment = alignment_arg;
	if (text_arg != NULL) {
		Py_INCREF(text_arg);
		Py_DECREF(self->text);
		self->text = text_arg;
	}

	return 0;
}

static int Text_traverse(Text *self, visitproc visit, void *arg)
{
	Py_VISIT(self->text);
	return 0;
}

static int Text_clear(Text *self)
{
	Py_CLEAR(self->text);
	return 0;
}

static void Text_dealloc(Text *self)
{
	Text_clear(self);
	self->ob_type->tp_free((PyObject *)self);
}

static PyMemberDef Text_members[] = {
	{ "x", T_DOUBLE, offsetof(Text, data.pos.x), 0,
	  PyDoc_STR("") },
	{ "y", T_DOUBLE, offsetof(Text, data.pos.y), 0,
	  PyDoc_STR("") },
	{ "color", T_INT, offsetof(Text, data.color), 0,
	  PyDoc_STR("") },
	{ "text_size", T_INT, offsetof(Text, data.text_size), 0,
	  PyDoc_STR("") },
	{ "visibility", T_BOOL, offsetof(Text, data.visibility), 0,
	  PyDoc_STR("") },
	{ "show_name_value", T_INT, offsetof(Text, data.show_name_value), 0,
	  PyDoc_STR("") },
	{ "angle", T_INT, offsetof(Text, data.angle), 0,
	  PyDoc_STR("") },
	{ "alignment", T_INT, offsetof(Text, data.alignment), 0,
	  PyDoc_STR("") },
	{ NULL, 0, 0, 0, NULL }  /* Sentinel */
};

static PyObject *Text_gettext(Text *self, void *closure)
{
	Py_INCREF(self->text);
	return self->text;
}

static int Text_settext(Text *self, PyObject *value, void *closure)
{
	if (value == NULL) {
		PyErr_SetString(PyExc_TypeError,
				"text attribute cannot be deleted");
		return -1;
	}

	if (!PyString_Check(value)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ,
			 "text attribute must be %.50s, not %.50s",
			 PyString_Type.tp_name, value->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return -1;
	}

	Py_INCREF(value);
	Py_DECREF(self->text);
	self->text = value;
	return 0;
}

static PyGetSetDef Text_getset[] = {
	{ "text", (getter)Text_gettext, (setter)Text_settext,
	  PyDoc_STR(""), NULL },
	{ NULL, NULL, NULL, NULL, NULL }  /* Sentinel */
};

PyTypeObject TextType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/

	/* For printing, in format "<module>.<name>" */
	"xorn.storage.Text",		/* const char *tp_name */

	/* For allocation */
	sizeof(Text),			/* Py_ssize_t tp_basicsize */
	0,				/* Py_ssize_t tp_itemsize */

	/* Methods to implement standard operations */
	(destructor)Text_dealloc,	/* destructor tp_dealloc */
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
	PyDoc_STR("Schematic text or attribute."),
					/* const char *tp_doc */

	/* Assigned meaning in release 2.0 */
	/* call function for all accessible objects */
	(traverseproc)Text_traverse,/* traverseproc tp_traverse */

	/* delete references to contained objects */
	(inquiry)Text_clear,	/* inquiry tp_clear */

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
	Text_members,		/* struct PyMemberDef *tp_members */
	Text_getset,		/* struct PyGetSetDef *tp_getset */
	NULL,				/* struct _typeobject *tp_base */
	NULL,				/* PyObject *tp_dict */
	NULL,				/* descrgetfunc tp_descr_get */
	NULL,				/* descrsetfunc tp_descr_set */
	0,				/* Py_ssize_t tp_dictoffset */
	(initproc)Text_init,	/* initproc tp_init */
	NULL,				/* allocfunc tp_alloc */
	Text_new,			/* newfunc tp_new */
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
