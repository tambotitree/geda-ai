/* module.c - Python C API bindings for gEDA storage objects
 *
 * Originally written for gEDA/gaf
 * Modernized for Python 3 and extended by geda-ai
 *
 * Copyright (C) [original year] gEDA Contributors
 * Copyleft 2025 John Ryan, maintainer geda-ai
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "module.h"
#include "data.h"


// Forward declarations for type objects
extern PyTypeObject RevisionType;
extern PyTypeObject ObjectType;
extern PyTypeObject SelectionType;
extern PyTypeObject ArcType;
extern PyTypeObject BoxType;
extern PyTypeObject CircleType;
extern PyTypeObject ComponentType;
extern PyTypeObject LineType;
extern PyTypeObject NetType;
extern PyTypeObject PathType;
extern PyTypeObject PictureType;
extern PyTypeObject TextType;
extern PyTypeObject LineAttrType;
extern PyTypeObject FillAttrType;


static PyObject *to_python_list(xorn_object_t *objects, size_t count)
{
	PyObject *list;
	size_t i;

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

static PyObject *get_objects_attached_to(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *rev_arg = NULL, *ob_arg = NULL;
	static char *kwlist[] = { "rev", "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O:get_objects_attached_to", kwlist,
		    &RevisionType, &rev_arg, &ob_arg))
		return NULL;

	if (ob_arg != Py_None &&
	    !PyObject_TypeCheck(ob_arg, &ObjectType)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ, "get_objects_attached_to() argument 2 "
				      "must be %.50s or None, not %.50s",
			 ObjectType.tp_name,
			 ob_arg->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return NULL;
	}

	if (ob_arg != Py_None &&
	    !xorn_object_exists_in_revision(((Revision *)rev_arg)->rev,
					    ((Object *)ob_arg)->ob)) {
		PyErr_SetString(PyExc_KeyError, "object does not exist");
		return NULL;
	}

	xorn_object_t *objects;
	size_t count;

	if (xorn_get_objects_attached_to(((Revision *)rev_arg)->rev,
					 ob_arg == Py_None ? NULL :
					     ((Object *)ob_arg)->ob,
					 &objects, &count) == -1)
		return PyErr_NoMemory();

	return to_python_list(objects, count);
}

static PyObject *get_selected_objects(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *rev_arg = NULL, *sel_arg = NULL;
	static char *kwlist[] = { "rev", "sel", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:get_selected_objects", kwlist,
		    &RevisionType, &rev_arg, &SelectionType, &sel_arg))
		return NULL;

	xorn_object_t *objects;
	size_t count;

	if (xorn_get_selected_objects(((Revision *)rev_arg)->rev,
				      ((Selection *)sel_arg)->sel,
				      &objects, &count) == -1)
		return PyErr_NoMemory();

	return to_python_list(objects, count);
}

static PyObject *get_added_objects(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *from_arg = NULL, *to_arg = NULL;
	static char *kwlist[] = { "from", "to", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:get_added_objects", kwlist,
		    &RevisionType, &from_arg, &RevisionType, &to_arg))
		return NULL;

	xorn_object_t *objects;
	size_t count;

	if (xorn_get_added_objects(((Revision *)from_arg)->rev,
				   ((Revision *)to_arg)->rev,
				   &objects, &count) == -1)
		return PyErr_NoMemory();

	return to_python_list(objects, count);
}

static PyObject *get_removed_objects(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *from_arg = NULL, *to_arg = NULL;
	static char *kwlist[] = { "from", "to", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:get_removed_objects", kwlist,
		    &RevisionType, &from_arg, &RevisionType, &to_arg))
		return NULL;

	xorn_object_t *objects;
	size_t count;

	if (xorn_get_removed_objects(((Revision *)from_arg)->rev,
				     ((Revision *)to_arg)->rev,
				     &objects, &count) == -1)
		return PyErr_NoMemory();

	return to_python_list(objects, count);
}

static PyObject *get_modified_objects(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *from_arg = NULL, *to_arg = NULL;
	static char *kwlist[] = { "from", "to", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:get_modified_objects", kwlist,
		    &RevisionType, &from_arg, &RevisionType, &to_arg))
		return NULL;

	xorn_object_t *objects;
	size_t count;

	if (xorn_get_modified_objects(((Revision *)from_arg)->rev,
				      ((Revision *)to_arg)->rev,
				      &objects, &count) == -1)
		return PyErr_NoMemory();

	return to_python_list(objects, count);
}

/****************************************************************************/

static PyObject *select_none(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	xorn_selection_t sel = xorn_select_none();
	return sel ? build_selection(sel) : PyErr_NoMemory();
}

static PyObject *select_object(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *ob_arg = NULL;
	static char *kwlist[] = { "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!:select_object", kwlist,
		    &ObjectType, &ob_arg))
		return NULL;

	xorn_selection_t sel = xorn_select_object(((Object *)ob_arg)->ob);
	return sel ? build_selection(sel) : PyErr_NoMemory();
}

static PyObject *select_attached_to(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *rev_arg = NULL, *ob_arg = NULL;
	static char *kwlist[] = { "rev", "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O:select_attached_to", kwlist,
		    &RevisionType, &rev_arg, &ob_arg))
		return NULL;

	if (ob_arg != Py_None &&
	    !PyObject_TypeCheck(ob_arg, &ObjectType)) {
		char buf[BUFSIZ];
		snprintf(buf, BUFSIZ, "select_attached_to() argument 2 "
				      "must be %.50s or None, not %.50s",
			 ObjectType.tp_name,
			 ob_arg->ob_type->tp_name);
		PyErr_SetString(PyExc_TypeError, buf);
		return NULL;
	}

	if (ob_arg != Py_None &&
	    !xorn_object_exists_in_revision(((Revision *)rev_arg)->rev,
					    ((Object *)ob_arg)->ob)) {
		PyErr_SetString(PyExc_KeyError, "object does not exist");
		return NULL;
	}

	xorn_selection_t sel = xorn_select_attached_to(
		((Revision *)rev_arg)->rev,
		ob_arg == Py_None ? NULL : ((Object *)ob_arg)->ob);
	return sel ? build_selection(sel) : PyErr_NoMemory();
}

static PyObject *select_all(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *rev_arg = NULL;
	static char *kwlist[] = { "rev", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!:select_all", kwlist,
		    &RevisionType, &rev_arg))
		return NULL;

	xorn_selection_t sel = xorn_select_all(((Revision *)rev_arg)->rev);
	return sel ? build_selection(sel) : PyErr_NoMemory();
}

static PyObject *select_all_except(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *rev_arg = NULL, *sel_arg = NULL;
	static char *kwlist[] = { "rev", "sel", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:select_all_except", kwlist,
		    &RevisionType, &rev_arg, &SelectionType, &sel_arg))
		return NULL;

	xorn_selection_t sel = xorn_select_all_except(
		((Revision *)rev_arg)->rev, ((Selection *)sel_arg)->sel);
	return sel ? build_selection(sel) : PyErr_NoMemory();
}

static PyObject *select_including(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *sel_arg = NULL, *ob_arg = NULL;
	static char *kwlist[] = { "sel", "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:select_including", kwlist,
		    &SelectionType, &sel_arg, &ObjectType, &ob_arg))
		return NULL;

	xorn_selection_t sel = xorn_select_including(
		((Selection *)sel_arg)->sel, ((Object *)ob_arg)->ob);
	return sel ? build_selection(sel) : PyErr_NoMemory();
}

static PyObject *select_excluding(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *sel_arg = NULL, *ob_arg = NULL;
	static char *kwlist[] = { "sel", "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:select_excluding", kwlist,
		    &SelectionType, &sel_arg, &ObjectType, &ob_arg))
		return NULL;

	xorn_selection_t sel = xorn_select_excluding(
		((Selection *)sel_arg)->sel, ((Object *)ob_arg)->ob);
	return sel ? build_selection(sel) : PyErr_NoMemory();
}

static PyObject *select_union(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *sel0_arg = NULL, *sel1_arg = NULL;
	static char *kwlist[] = { "sel0", "sel1", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:select_union", kwlist,
		    &SelectionType, &sel0_arg, &SelectionType, &sel1_arg))
		return NULL;

	xorn_selection_t sel = xorn_select_union(((Selection *)sel0_arg)->sel,
						 ((Selection *)sel1_arg)->sel);
	return sel ? build_selection(sel) : PyErr_NoMemory();
}

static PyObject *select_intersection(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *sel0_arg = NULL, *sel1_arg = NULL;
	static char *kwlist[] = { "sel0", "sel1", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:select_intersection", kwlist,
		    &SelectionType, &sel0_arg, &SelectionType, &sel1_arg))
		return NULL;

	xorn_selection_t sel = xorn_select_intersection(
		((Selection *)sel0_arg)->sel, ((Selection *)sel1_arg)->sel);
	return sel ? build_selection(sel) : PyErr_NoMemory();
}

static PyObject *select_difference(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *sel0_arg = NULL, *sel1_arg = NULL;
	static char *kwlist[] = { "sel0", "sel1", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:select_difference", kwlist,
		    &SelectionType, &sel0_arg, &SelectionType, &sel1_arg))
		return NULL;

	xorn_selection_t sel = xorn_select_difference(
		((Selection *)sel0_arg)->sel, ((Selection *)sel1_arg)->sel);
	return sel ? build_selection(sel) : PyErr_NoMemory();
}

static PyObject *selection_is_empty(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *rev_arg = NULL, *sel_arg = NULL;
	static char *kwlist[] = { "rev", "sel", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!:selection_is_empty", kwlist,
		    &RevisionType, &rev_arg, &SelectionType, &sel_arg))
		return NULL;

	PyObject *result = xorn_selection_is_empty(
		((Revision *)rev_arg)->rev,
		((Selection *)sel_arg)->sel) ? Py_True : Py_False;
	Py_INCREF(result);
	return result;
}

static PyObject *object_is_selected(
	PyObject *self, PyObject *args, PyObject *kwds)
{
	PyObject *rev_arg = NULL, *sel_arg = NULL, *ob_arg = NULL;
	static char *kwlist[] = { "rev", "sel", "ob", NULL };

	if (!PyArg_ParseTupleAndKeywords(
		    args, kwds, "O!O!O!:object_is_selected", kwlist,
		    &RevisionType, &rev_arg, &SelectionType, &sel_arg,
		    &ObjectType, &ob_arg))
		return NULL;

	PyObject *result = xorn_object_is_selected(
		((Revision *)rev_arg)->rev,
		((Selection *)sel_arg)->sel,
		((Object *)ob_arg)->ob) ? Py_True : Py_False;
	Py_INCREF(result);
	return result;
}

static PyMethodDef storage_methods[] = {
    {"get_objects_attached_to", (PyCFunction)get_objects_attached_to, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("get_objects_attached_to(rev, ob) -> list[Object]\n"
              "Return objects in a revision attached to a given object.")},
    {"get_selected_objects", (PyCFunction)get_selected_objects, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("get_selected_objects(rev, sel) -> list[Object]\n"
              "Return objects in both the revision and the selection.")},
    {"get_added_objects", (PyCFunction)get_added_objects, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("get_added_objects(from, to) -> list[Object]\n"
              "Return objects present in 'to' but not in 'from'.")},
    {"get_removed_objects", (PyCFunction)get_removed_objects, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("get_removed_objects(from, to) -> list[Object]\n"
              "Return objects present in 'from' but not in 'to'.")},
    {"get_modified_objects", (PyCFunction)get_modified_objects, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("get_modified_objects(from, to) -> list[Object]\n"
              "Return objects present in both revisions but with modified content.")},
    {"select_none", (PyCFunction)select_none, METH_NOARGS,
     PyDoc_STR("select_none() -> Selection\nReturn an empty selection.")},
    {"select_object", (PyCFunction)select_object, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("select_object(ob) -> Selection\nReturn a selection containing only 'ob'.")},
    {"select_attached_to", (PyCFunction)select_attached_to, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("select_attached_to(rev, ob) -> Selection\nReturn selection of objects attached to 'ob'.")},
    {"select_all", (PyCFunction)select_all, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("select_all(rev) -> Selection\nReturn all objects in the revision.")},
    {"select_all_except", (PyCFunction)select_all_except, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("select_all_except(rev, sel) -> Selection\nReturn all objects in revision excluding selection.")},
    {"select_including", (PyCFunction)select_including, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("select_including(sel, ob) -> Selection\nReturn selection including 'ob'.")},
    {"select_excluding", (PyCFunction)select_excluding, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("select_excluding(sel, ob) -> Selection\nReturn selection excluding 'ob'.")},
    {"select_union", (PyCFunction)select_union, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("select_union(sel0, sel1) -> Selection\nReturn union of both selections.")},
    {"select_intersection", (PyCFunction)select_intersection, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("select_intersection(sel0, sel1) -> Selection\nReturn intersection of both selections.")},
    {"select_difference", (PyCFunction)select_difference, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("select_difference(sel0, sel1) -> Selection\nReturn objects in sel0 not in sel1.")},
    {"selection_is_empty", (PyCFunction)selection_is_empty, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("selection_is_empty(rev, sel) -> bool\nReturn True if selection is empty.")},
    {"object_is_selected", (PyCFunction)object_is_selected, METH_VARARGS | METH_KEYWORDS,
     PyDoc_STR("object_is_selected(rev, sel, ob) -> bool\nReturn True if object is selected.")},
    {NULL, NULL, 0, NULL}  // Sentinel
};

static struct PyModuleDef storagemodule = {
    PyModuleDef_HEAD_INIT,
    "storage",
    "Xorn storage backend bindings (Python 3, geda-ai compatible)",
    -1,
    storage_methods
};

static int add_type(PyObject *module, const char *name, PyTypeObject *value)
{
	Py_INCREF(value);
	return PyModule_AddObject(module, name, (PyObject *)value);
}

PyMODINIT_FUNC PyInit_storage(void)
{
    PyObject *m;

    if (PyType_Ready(&RevisionType) < 0) return NULL;
    if (PyType_Ready(&ObjectType) < 0) return NULL;
    if (PyType_Ready(&SelectionType) < 0) return NULL;
    if (PyType_Ready(&ArcType) < 0) return NULL;
    if (PyType_Ready(&BoxType) < 0) return NULL;
    if (PyType_Ready(&CircleType) < 0) return NULL;
    if (PyType_Ready(&ComponentType) < 0) return NULL;
    if (PyType_Ready(&LineType) < 0) return NULL;
    if (PyType_Ready(&NetType) < 0) return NULL;
    if (PyType_Ready(&PathType) < 0) return NULL;
    if (PyType_Ready(&PictureType) < 0) return NULL;
    if (PyType_Ready(&TextType) < 0) return NULL;
    if (PyType_Ready(&LineAttrType) < 0) return NULL;
    if (PyType_Ready(&FillAttrType) < 0) return NULL;

    m = PyModule_Create(&storagemodule);
    if (!m) return NULL;

    PyModule_AddObject(m, "Revision", (PyObject *)&RevisionType);
    PyModule_AddObject(m, "Object", (PyObject *)&ObjectType);
    PyModule_AddObject(m, "Selection", (PyObject *)&SelectionType);
    // Optional: add remaining types if needed

    return m;
}
