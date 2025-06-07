/* Copyright (C) 2024 John Ryan

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more detailsevision_method


   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/************************* DEFINES *****************************/
#define INITRETURN return NULL
#define INITRETURNVAL(v) return (v)

/************************* INCLUDES *****************************/
#include <Python.h>
#include "data.h"
#include "xornstorage.h"
#include "module.h"

// --- Forward declarations for all Revision_* functions used in Revision_methods ---

extern PyTypeObject ObjectType;
extern PyTypeObject SelectionType;

static PyObject *Revision_is_transient(Revision *self);
static PyObject *Revision_finalize(Revision *self);
static PyObject *Revision_get_objects(Revision *self, PyObject *args, PyObject *kwds);
static PyObject *Revision_object_exists(Revision *self, PyObject *args, PyObject *kwds);
static PyObject *Revision_get_object_data(Revision *self, PyObject *args, PyObject *kwds);
static PyObject *Revision_get_object_location(Revision *self, PyObject *args, PyObject *kwds);
static PyObject *Revision_add_object(Revision *self, PyObject *args, PyObject *kwds);
static PyObject *Revision_set_object_data(Revision *self, PyObject *args, PyObject *kwds);
static PyObject *Revision_relocate_object(Revision *self, PyObject *args, PyObject *kwds);
static PyObject *Revision_copy_object(Revision *self, PyObject *args, PyObject *kwds);
static PyObject *Revision_copy_objects(Revision *self, PyObject *args, PyObject *kwds);
static PyObject *Revision_delete_object(Revision *self, PyObject *args, PyObject *kwds);
static PyObject *Revision_delete_objects(Revision *self, PyObject *args, PyObject *kwds);
// Declarations for transient getter/setter
static PyObject *Revision_gettransient(Revision *self, void *closure);
static int Revision_settransient(Revision *self, PyObject *value, void *closure);

// From data.h
extern PyTypeObject ObjectType;

/* ========================== Type Definitions =========================== */
static PyObject *xorn_error = NULL;

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "xorn.storage",  // name of module
    "Bindings for xorn storage subsystem", // docstring
    -1,              // size of per-interpreter state or -1
    NULL,            // methods will be added later manually
    NULL, NULL, NULL, NULL
};
static PyMethodDef Revision_methods[] = {
	{ "is_transient", (PyCFunction)Revision_is_transient, METH_NOARGS,
	  PyDoc_STR("rev.is_transient() -> bool -- "
		    "whether the revision can be changed") },
	{ "finalize", (PyCFunction)Revision_finalize, METH_NOARGS,
	  PyDoc_STR("rev.finalize() -- "
		    "prevent further changes to the revision") },
	{ "get_objects", (PyCFunction)Revision_get_objects, METH_NOARGS,
	  PyDoc_STR("rev.get_objects() -> [Object] -- "
		    "a list of all objects in the revision") },
	{ "object_exists", (PyCFunction)Revision_object_exists,
	  METH_KEYWORDS,
	  PyDoc_STR("rev.object_exists(ob) -> bool -- "
		    "whether an object exists in the revision") },
	{ "get_object_data", (PyCFunction)Revision_get_object_data,
	  METH_KEYWORDS,
	  PyDoc_STR("rev.get_object_data(ob) -> Arc/Box/... -- "
		    "get the data of an object") },
	{ "get_object_location", (PyCFunction)Revision_get_object_location,
	  METH_KEYWORDS,
	  PyDoc_STR("rev.get_object_location(ob) -> Object, int -- "
		    "get the location of an object in the object structure") },

	{ "add_object", (PyCFunction)Revision_add_object, METH_KEYWORDS,
	  PyDoc_STR("rev.add_object(data) -> Object -- "
		    "add a new object to the revision\n\n"
		    "Only callable on a transient revision.\n") },
	{ "set_object_data", (PyCFunction)Revision_set_object_data,
	  METH_KEYWORDS,
	  PyDoc_STR("rev.set_object_data(ob, data) -- "
		    "set the data of an object\n\n"
		    "Only callable on a transient revision.\n") },
	{ "relocate_object", (PyCFunction)Revision_relocate_object,
	  METH_KEYWORDS,
	  PyDoc_STR("rev.relocate_object(ob, insert_before) -- "
		    "change the location of an object in the object "
		    "structure\n\n"
		    "Only callable on a transient revision.\n") },
	{ "copy_object", (PyCFunction)Revision_copy_object, METH_KEYWORDS,
	  PyDoc_STR("dest.copy_object(src, ob) -> Object -- "
		    "copy an object to the revision\n\n"
		    "Only callable on a transient revision.\n") },
	{ "copy_objects", (PyCFunction)Revision_copy_objects, METH_KEYWORDS,
	  PyDoc_STR("dest.copy_objects(src, sel) -> Selection -- "
		    "copy some objects to the revision\n\n"
		    "Only callable on a transient revision.\n") },
	{ "delete_object", (PyCFunction)Revision_delete_object, METH_KEYWORDS,
	  PyDoc_STR("rev.delete_object(ob) -- "
		    "delete an object from the revision\n\n"
		    "Only callable on a transient revision.\n") },
	{ "delete_objects", (PyCFunction)Revision_delete_objects, METH_KEYWORDS,
	  PyDoc_STR("rev.delete_objects(sel) -- "
		    "delete some objects from the revision\n\n"
		    "Only callable on a transient revision.\n") },

	{ NULL, NULL, 0, NULL }  /* Sentinel */
};

static struct PyModuleDef storagemodule = {
    PyModuleDef_HEAD_INIT,
    "xorn.storage",
    "Bindings for manipulating schematic objects.",
    -1,
    Revision_methods,
    NULL, NULL, NULL, NULL
};

/* ========================== Function Prototypes ======================== */
static PyObject *Revision_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
static int Revision_init(PyObject *self, PyObject *args, PyObject *kwds);
static void Revision_dealloc(PyObject *self);

/* ========================== Method and GetSet Tables =================== */
static PyGetSetDef Revision_getset[] = {
	{ "transient", (getter)Revision_gettransient,
		       (setter)Revision_settransient,
	  PyDoc_STR("Whether the revision can be changed."), NULL },
	{ NULL, NULL, NULL, NULL, NULL }  /* Sentinel */
};

/* ========================== Type Object ================================ */
PyTypeObject RevisionType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "xorn.Revision",
    .tp_basicsize = sizeof(Revision),
    .tp_itemsize = 0,
    .tp_dealloc = Revision_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "The identity of an object across revisions.",
    .tp_methods = Revision_methods,
    .tp_getset = Revision_getset,
    .tp_init = Revision_init,
    .tp_new = Revision_new,
};

/* ========================== Function Implementations =================== */
static PyObject *Revision_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Revision *self = (Revision *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->rev = NULL;
    }
    return (PyObject *)self;
}

static int Revision_init(PyObject *self, PyObject *args, PyObject *kwds) {
    ((Revision *)self)->rev = NULL;
    return 0;
}

static void Revision_dealloc(PyObject *self) {
    Revision *r = (Revision *)self;
    xorn_free_revision(r->rev);
    Py_TYPE(self)->tp_free(self);
}

/* ========================== Module Initialization ====================== */
static int add_revision_types(PyObject *m);
static int add_object_types(PyObject *m);
static int add_selection_types(PyObject *m);
static int add_data_types(PyObject *m);


static PyObject *Revision_is_transient(Revision *self)
{
	PyObject *result = xorn_revision_is_transient(self->rev) ? Py_True
								 : Py_False;
	Py_INCREF(result);
	return result;
}

static PyObject *Revision_finalize(Revision *self)
{
	xorn_finalize_revision(self->rev);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *Revision_get_objects(
	Revision *self, PyObject *args, PyObject *kwds)
{
	xorn_object_t *objects;
	size_t count;
	PyObject *list;
	size_t i;

	if (xorn_get_objects(self->rev, &objects, &count) == -1)
		return PyErr_NoMemory();

	list = PyList_New(count);
	if (list == NULL)
		return NULL;

	for (i = 0; i < count; i++) {
		PyObject *ob_item = build_object(objects[i]);
		if (ob_item == NULL) {
			Py_DECREF(list);
			free(objects);
			return NULL;
		}
		PyList_SET_ITEM(list, i, ob_item);
	}

	free(objects);
	return list;
}

static int add_revision_types(PyObject *m)
{
    if (PyModule_AddObject(m, "Revision", (PyObject *)&RevisionType) < 0)
        return -1;
    return 0;
}
static int add_object_types(PyObject *m)
{
    if (PyModule_AddObject(m, "Object", (PyObject *)&ObjectType) < 0)
        return -1;
    return 0;
}

static int add_selection_types(PyObject *m)
{
    if (PyModule_AddObject(m, "Selection", (PyObject *)&SelectionType) < 0)
        return -1;
    return 0;
}

static int add_data_types(PyObject *m)
{
    // If you have other data types (e.g. SegmentType, LayerType, etc.), add them here.
    // For now, assume nothing else is needed:
    return 0;
}

static PyObject *Revision_object_exists(
	Revision *self, PyObject *args, PyObject *kwds)
{
	PyObject *ob_arg = NULL;
	static char *kwlist[] = { "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!:Revision.object_exists", kwlist,
		    &ObjectType, &ob_arg))
		return NULL;

	PyObject *result = xorn_object_exists_in_revision(
	    self->rev, ((Object *)ob_arg)->ob) ? Py_True
					       : Py_False;
	Py_INCREF(result);
	return result;
}

static PyObject *Revision_get_object_data(
	Revision *self, PyObject *args, PyObject *kwds)
{
	PyObject *ob_arg = NULL;
	static char *kwlist[] = { "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!:Revision.get_object_data", kwlist,
		    &ObjectType, &ob_arg))
		return NULL;

	xorn_object_t ob = ((Object *)ob_arg)->ob;
	xorn_obtype_t type = xorn_get_object_type(self->rev, ob);

	switch (type) {
	case xorn_obtype_none:
		PyErr_SetString(PyExc_KeyError, "object does not exist");
		return NULL;
	case xornsch_obtype_arc:
		return construct_arc(xornsch_get_arc_data(self->rev, ob));
	case xornsch_obtype_box:
		return construct_box(xornsch_get_box_data(self->rev, ob));
	case xornsch_obtype_circle:
		return construct_circle(
			xornsch_get_circle_data(self->rev, ob));
	case xornsch_obtype_component:
		return construct_component(
			xornsch_get_component_data(self->rev, ob));
	case xornsch_obtype_line:
		return construct_line(xornsch_get_line_data(self->rev, ob));
	case xornsch_obtype_net:
		return construct_net(xornsch_get_net_data(self->rev, ob));
	case xornsch_obtype_path:
		return construct_path(xornsch_get_path_data(self->rev, ob));
	case xornsch_obtype_picture:
		return construct_picture(
			xornsch_get_picture_data(self->rev, ob));
	case xornsch_obtype_text:
		return construct_text(xornsch_get_text_data(self->rev, ob));
	}

	char buf[BUFSIZ];
	snprintf(buf, BUFSIZ, "object type not supported (%d)", type);
	PyErr_SetString(PyExc_ValueError, buf);
	return NULL;
}

static PyObject *Revision_get_object_location(
	Revision *self, PyObject *args, PyObject *kwds)
{
	PyObject *ob_arg = NULL;
	static char *kwlist[] = { "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!:Revision.get_object_location", kwlist,
		    &ObjectType, &ob_arg))
		return NULL;

	xorn_object_t attached_to = NULL;
	unsigned int position = -1;

	if (xorn_get_object_location(self->rev, ((Object *)ob_arg)->ob,
				     &attached_to, &position) == -1) {
		PyErr_SetString(PyExc_KeyError, "object does not exist");
		return NULL;
	}

	if (attached_to == NULL)
		return Py_BuildValue("OI", Py_None, position);

	return Py_BuildValue("NI", build_object(attached_to), position);
}

/****************************************************************************/

static int prepare_data(PyObject *obj, xorn_obtype_t *type_return,
	const void **data_return)
{
if (PyObject_TypeCheck(obj, &ArcType))
prepare_arc((Arc *)obj, type_return, data_return);
else if (PyObject_TypeCheck(obj, &BoxType))
prepare_box((Box *)obj, type_return, data_return);
else if (PyObject_TypeCheck(obj, &CircleType))
prepare_circle((Circle *)obj, type_return, data_return);
else if (PyObject_TypeCheck(obj, &ComponentType))
prepare_component((Component *)obj, type_return, data_return);
else if (PyObject_TypeCheck(obj, &LineType))
prepare_line((Line *)obj, type_return, data_return);
else if (PyObject_TypeCheck(obj, &NetType))
prepare_net((Net *)obj, type_return, data_return);
else if (PyObject_TypeCheck(obj, &PathType))
prepare_path((Path *)obj, type_return, data_return);
else if (PyObject_TypeCheck(obj, &PictureType))
prepare_picture((Picture *)obj, type_return, data_return);
else if (PyObject_TypeCheck(obj, &TextType))
prepare_text((Text *)obj, type_return, data_return);
else
return -1;  // Unknown type

return 0;  // Success
}

static PyObject *Revision_add_object(
	Revision *self, PyObject *args, PyObject *kwds)
{
	PyObject *data_arg = NULL;
	static char *kwlist[] = { "data", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O:Revision.add_object", kwlist, &data_arg))
		return NULL;

	xorn_obtype_t type = xorn_obtype_none;
	const void *data = NULL;

	if (prepare_data(data_arg, &type, &data) == -1) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ,
			 "Revision.add_object() argument 'data' (pos 1) "
			 "must be of xorn.storage object type, not %.50s",
			 data_arg->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return NULL;
	}

	xorn_error_t err;
	xorn_object_t ob = xorn_add_object(self->rev, type, data, &err);

	if (ob == NULL) {
		switch (err) {
		case xorn_error_revision_not_transient:
			PyErr_SetString(PyExc_ValueError,
			    "revision can only be changed while transient");
			break;
		case xorn_error_invalid_object_data:
			PyErr_SetString(PyExc_ValueError,
			    "invalid object data");
			break;
		case xorn_error_out_of_memory:
			PyErr_NoMemory();
			break;
		case xorn_error_invalid_argument:
			PyErr_SetString(PyExc_SystemError,
			    "error preparing object data");
			break;
		default:
			PyErr_SetString(PyExc_SystemError,
			    "invalid Xorn error code");
		}
		return NULL;
	}

	return build_object(ob);
}

static PyObject *Revision_set_object_data(
	Revision *self, PyObject *args, PyObject *kwds)
{
	PyObject *ob_arg = NULL, *data_arg = NULL;
	static char *kwlist[] = { "ob", "data", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O:Revision.set_object_data", kwlist,
		    &ObjectType, &ob_arg, &data_arg))
		return NULL;

	xorn_obtype_t type = xorn_obtype_none;
	const void *data = NULL;

	if (prepare_data(data_arg, &type, &data) == -1) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ,
			 "Revision.set_object_data() argument 'data' (pos 2) "
			 "must be of xorn.storage object type, not %.50s",
			 data_arg->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return NULL;
	}

	xorn_error_t err;
	if (xorn_set_object_data(self->rev, ((Object *)ob_arg)->ob,
				 type, data, &err) == -1) {
		switch (err) {
		case xorn_error_revision_not_transient:
			PyErr_SetString(PyExc_ValueError,
			    "revision can only be changed while transient");
			break;
		case xorn_error_invalid_object_data:
			PyErr_SetString(PyExc_ValueError,
			    "invalid object data");
			break;
		case xorn_error_invalid_parent:
			PyErr_SetString(PyExc_ValueError,
			    "can't set attached object "
			    "to something other than text");
			break;
		case xorn_error_invalid_existing_child:
			PyErr_SetString(PyExc_ValueError,
			    "can't set object with attached objects "
			    "to something other than net or component");
			break;
		case xorn_error_out_of_memory:
			PyErr_NoMemory();
			break;
		case xorn_error_invalid_argument:
			PyErr_SetString(PyExc_SystemError,
			    "error preparing object data");
			break;
		default:
			PyErr_SetString(PyExc_SystemError,
			    "invalid Xorn error code");
		}
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *Revision_relocate_object(
	Revision *self, PyObject *args, PyObject *kwds)
{
	PyObject *ob_arg = NULL, *attach_to_arg = NULL,
				 *insert_before_arg = NULL;
	static char *kwlist[] = { "ob", "attach_to", "insert_before", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!OO:Revision.relocate_object", kwlist,
		    &ObjectType, &ob_arg, &attach_to_arg, &insert_before_arg))
		return NULL;

	if (attach_to_arg != Py_None &&
	    !PyObject_TypeCheck(attach_to_arg, &ObjectType)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ, "Revision.relocate_object() argument 2 "
				      "must be %.50s or None, not %.50s",
			 ObjectType.tp_name,
			 attach_to_arg->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return NULL;
	}
	if (insert_before_arg != Py_None &&
	    !PyObject_TypeCheck(insert_before_arg, &ObjectType)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ, "Revision.relocate_object() argument 3 "
				      "must be %.50s or None, not %.50s",
			 ObjectType.tp_name,
			 insert_before_arg->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return NULL;
	}

	xorn_error_t err;
	if (xorn_relocate_object(self->rev, ((Object *)ob_arg)->ob,
				 attach_to_arg == Py_None ? NULL :
				     ((Object *)attach_to_arg)->ob,
				 insert_before_arg == Py_None ? NULL :
				     ((Object *)insert_before_arg)->ob,
				 &err) == -1) {
		switch (err) {
		case xorn_error_revision_not_transient:
			PyErr_SetString(PyExc_ValueError,
			    "revision can only be changed while transient");
			break;
		case xorn_error_object_doesnt_exist:
			PyErr_SetString(PyExc_KeyError,
			    "object does not exist");
			break;
		case xorn_error_parent_doesnt_exist:
			PyErr_SetString(PyExc_KeyError,
			    "parent object does not exist");
			break;
		case xorn_error_invalid_parent:
			if (xorn_get_object_type(
				    self->rev, ((Object *)ob_arg)->ob)
				!= xornsch_obtype_text)
				PyErr_SetString(PyExc_ValueError,
				    "only text objects can be attached");
			else
				PyErr_SetString(PyExc_ValueError,
				    "can only attach to net and "
				    "component objects");
			break;
		case xorn_error_successor_doesnt_exist:
			PyErr_SetString(PyExc_KeyError,
			    "reference object does not exist");
			break;
		case xorn_error_successor_not_sibling:
			PyErr_SetString(PyExc_ValueError,
			    "invalid reference object");
			break;
		case xorn_error_out_of_memory:
			PyErr_NoMemory();
			break;
		default:
			PyErr_SetString(PyExc_SystemError,
			    "invalid Xorn error code");
		}
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *Revision_copy_object(
	Revision *self, PyObject *args, PyObject *kwds)
{
	PyObject *rev_arg = NULL, *ob_arg = NULL;
	static char *kwlist[] = { "rev", "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:Revision.copy_object", kwlist,
		    &RevisionType, &rev_arg, &ObjectType, &ob_arg))
		return NULL;

	xorn_error_t err;
	xorn_object_t ob = xorn_copy_object(
	    self->rev, ((Revision *)rev_arg)->rev,
		       ((Object *)ob_arg)->ob, &err);

	if (ob == NULL) {
		switch (err) {
		case xorn_error_revision_not_transient:
			PyErr_SetString(PyExc_ValueError,
			    "revision can only be changed while transient");
			break;
		case xorn_error_object_doesnt_exist:
			PyErr_SetString(PyExc_KeyError,
			    "object does not exist in source revision");
			break;
		case xorn_error_out_of_memory:
			PyErr_NoMemory();
			break;
		default:
			PyErr_SetString(PyExc_SystemError,
			    "invalid Xorn error code");
		}
		return NULL;
	}

	return build_object(ob);
}

static PyObject *Revision_copy_objects(
	Revision *self, PyObject *args, PyObject *kwds)
{
	PyObject *rev_arg = NULL, *sel_arg = NULL;
	static char *kwlist[] = { "rev", "sel", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:Revision.copy_objects", kwlist,
		    &RevisionType, &rev_arg, &SelectionType, &sel_arg))
		return NULL;

	xorn_error_t err;
	xorn_selection_t sel = xorn_copy_objects(
	    self->rev, ((Revision *)rev_arg)->rev,
		       ((Selection *)sel_arg)->sel, &err);

	if (sel == NULL) {
		switch (err) {
		case xorn_error_revision_not_transient:
			PyErr_SetString(PyExc_ValueError,
			    "revision can only be changed while transient");
			break;
		case xorn_error_out_of_memory:
			PyErr_NoMemory();
			break;
		default:
			PyErr_SetString(PyExc_SystemError,
			    "invalid Xorn error code");
		}
		return NULL;
	}

	return build_selection(sel);
}

static PyObject *Revision_delete_object(
	Revision *self, PyObject *args, PyObject *kwds)
{
	PyObject *ob_arg = NULL;
	static char *kwlist[] = { "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!:Revision.delete_object", kwlist,
		    &ObjectType, &ob_arg))
		return NULL;

	xorn_error_t err;
	if (xorn_delete_object(
		    self->rev, ((Object *)ob_arg)->ob, &err) == -1) {
		switch (err) {
		case xorn_error_revision_not_transient:
			PyErr_SetString(PyExc_ValueError,
			    "revision can only be changed while transient");
			break;
		case xorn_error_object_doesnt_exist:
			PyErr_SetString(PyExc_KeyError,
			    "object does not exist");
			break;
		default:
			PyErr_SetString(PyExc_SystemError,
			    "invalid Xorn error code");
		}
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *Revision_delete_objects(
	Revision *self, PyObject *args, PyObject *kwds)
{
	PyObject *sel_arg = NULL;
	static char *kwlist[] = { "sel", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!:Revision.delete_objects", kwlist,
		    &SelectionType, &sel_arg))
		return NULL;

	xorn_error_t err;
	if (xorn_delete_selected_objects(
		    self->rev, ((Selection *)sel_arg)->sel, &err) == -1) {
		switch (err) {
		case xorn_error_revision_not_transient:
			PyErr_SetString(PyExc_ValueError,
			    "revision can only be changed while transient");
			break;
		default:
			PyErr_SetString(PyExc_SystemError,
			    "invalid Xorn error code");
		}
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject *Revision_gettransient(Revision *self, void *closure)
{
	PyObject *result = xorn_revision_is_transient(self->rev) ? Py_True
								 : Py_False;
	Py_INCREF(result);
	return result;
}

static int Revision_settransient(
	Revision *self, PyObject *value, void *closure)
{
	if (value == NULL) {
		PyErr_SetString(PyExc_TypeError,
				"can't delete transient attribute");
		return -1;
	}

	if (value == Py_False) {
		xorn_finalize_revision(self->rev);
		return 0;
	}

	if (value == Py_True) {
		if (xorn_revision_is_transient(self->rev))
			return 0;
		PyErr_SetString(PyExc_ValueError,
				"can't make revision transient again");
		return -1;
	}

	PyErr_SetString(PyExc_TypeError, "transient attribute must be bool");
	return -1;
}




PyMODINIT_FUNC
INITFUNC(void)
{
    PyObject *m;

    if (PyType_Ready(&RevisionType) < 0)
        INITRETURN;
    if (PyType_Ready(&ObjectType) < 0)
        INITRETURN;
    if (PyType_Ready(&SelectionType) < 0)
        INITRETURN;
    if (PyType_Ready(&ArcType) < 0)
        INITRETURN;
    if (PyType_Ready(&BoxType) < 0)
        INITRETURN;
    if (PyType_Ready(&CircleType) < 0)
        INITRETURN;
    if (PyType_Ready(&ComponentType) < 0)
        INITRETURN;
    if (PyType_Ready(&LineType) < 0)
        INITRETURN;
    if (PyType_Ready(&NetType) < 0)
        INITRETURN;
    if (PyType_Ready(&PathType) < 0)
        INITRETURN;
    if (PyType_Ready(&PictureType) < 0)
        INITRETURN;
    if (PyType_Ready(&TextType) < 0)
        INITRETURN;
	m = PyModule_Create(&moduledef);
    if (m == NULL)
        INITRETURN;
    if (add_revision_types(m) < 0)
        INITRETURN;
    if (add_object_types(m) < 0)
        INITRETURN;
    if (add_selection_types(m) < 0)
        INITRETURN;
    if (add_data_types(m) < 0)
        INITRETURN;

    xorn_error = PyErr_NewException("xorn.storage.Error", NULL, NULL);
    if (xorn_error == NULL)
        INITRETURN;
    Py_INCREF(xorn_error);
    PyModule_AddObject(m, "Error", xorn_error);

    INITRETURNVAL(m);
}

