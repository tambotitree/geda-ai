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

   #ifdef HAVE_CONFIG_H
   #include "config.h"
   #endif
   #include "module.h"
   
   
   SCM scm_procedure_or_name(SCM proc)
   {
   #ifdef HAVE_SCM_PROCEDURE_NAME
	   return scm_procedure_name(proc);
   #else
	   return proc;
   #endif
   }
   
   SCM scm_frame_procedure_or_name(SCM frame)
   {
   #ifdef HAVE_SCM_FRAME_PROCEDURE_NAME
	   return scm_frame_procedure_name(frame);
   #else
	   return scm_frame_procedure(frame);
   #endif
   }
   
   
   struct call_data {
	   SCM proc;
	   PyObject *args;
   };
   
   static PyObject *call_wrapper(struct call_data *data)
   {
	   return scm2py(scm_apply(data->proc, py2scm(data->args), SCM_EOL));
   }
   
   static PyObject *Procedure_call(
	   Procedure *self, PyObject *args, PyObject *kwds)
   {
	   if (kwds != NULL && PyDict_Size(kwds) != 0) {
		   PyErr_SetString(
			   PyExc_ValueError,
			   "can't pass keyword arguments to Guile procedure");
		   return NULL;
	   }
   
	   struct call_data data = { self->proc, args };
	   PyObject *result = scm_with_guile(
		   (void *(*)(void *))call_wrapper, &data);
	   if (result == NULL && !PyErr_Occurred())
		   PyErr_SetNone(guile_error);
	   return result;
   }
   
   static PyObject *Procedure_repr(Procedure *self)
   {
	   char *s = scm_to_utf8_stringn(
		   scm_simple_format(SCM_BOOL_F,
					 scm_from_utf8_string("~S"),
					 scm_list_1(self->proc)), NULL);
	   if (s[0] != '#' || s[1] != '<') {
		   PyErr_SetString(
			   PyExc_SystemError,
			   "Invalid procedure representation returned by Guile");
		   return NULL;
	   }
	   PyObject *result = PyUnicode_FromFormat("<Guile %s", s + 2);
	   free(s);
	   return result;
   }
   
   static PyObject *Procedure_richcompare(Procedure *a, Procedure *b, int op)
   {
	   switch (op) {
	   case Py_EQ:
		   return PyBool_FromLong(scm_is_eq(a->proc, b->proc));
	   case Py_NE:
		   return PyBool_FromLong(!scm_is_eq(a->proc, b->proc));
	   default:
		   PyErr_SetString(
			   PyExc_TypeError,
			   "xorn.guile.Procedure only implements (non-)equality");
		   return NULL;
	   }
   }
   
   static void Procedure_dealloc(Procedure *self)
   {
	   Py_TYPE(self)->tp_free((PyObject *)self);
   }
   
   PyTypeObject ProcedureType = {
	   PyVarObject_HEAD_INIT(NULL, 0)
	   .tp_name = "xorn.guile.Procedure",
	   .tp_basicsize = sizeof(Procedure),
	   .tp_itemsize = 0,
	   .tp_dealloc = (destructor)Procedure_dealloc,
	   .tp_repr = (reprfunc)Procedure_repr,
	   .tp_call = (ternaryfunc)Procedure_call,
	   .tp_richcompare = (richcmpfunc)Procedure_richcompare,
	   .tp_flags = Py_TPFLAGS_DEFAULT,
	   .tp_doc = "Guile procedure."
   };
   