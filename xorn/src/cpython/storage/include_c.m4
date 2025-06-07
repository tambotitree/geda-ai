m4_divert(`-1')
# Copyleft 2025 John Ryan, maintainer geda-ai
# Modern include_c.m4 for Python 3.x only
# This macro defines a full PyTypeObject initialization block

m4_define(`include_c', `
PyTypeObject Class`'Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "xorn.storage.Class",
    .tp_basicsize = sizeof(Class),
    .tp_itemsize = 0,

    .tp_dealloc = (destructor)Class(`dealloc'),
    .tp_vectorcall_offset = 0,
    .tp_getattr = NULL,
    .tp_setattr = NULL,
    .tp_as_async = NULL,
    .tp_repr = NULL,

    .tp_as_number = NULL,
    .tp_as_sequence = NULL,
    .tp_as_mapping = NULL,

    .tp_hash = NULL,
    .tp_call = NULL,
    .tp_str = NULL,
    .tp_getattro = NULL,
    .tp_setattro = NULL,

    .tp_as_buffer = NULL,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
    .tp_doc = PyDoc_STR("docstring"),

    .tp_traverse = (traverseproc)Class(`traverse'),
    .tp_clear = (inquiry)Class(`clear'),

    .tp_richcompare = NULL,
    .tp_weaklistoffset = 0,

    .tp_iter = NULL,
    .tp_iternext = NULL,

    .tp_methods = NULL,
    .tp_members = Class(`members'),
    .tp_getset = Class(`getset'),
    .tp_base = NULL,
    .tp_dict = NULL,
    .tp_descr_get = NULL,
    .tp_descr_set = NULL,
    .tp_dictoffset = 0,
    .tp_init = (initproc)Class(`init'),
    .tp_alloc = NULL,
    .tp_new = Class(`new'),
    .tp_free = NULL,  /* Can override with PyObject_GC_Del if needed */
    .tp_is_gc = NULL, /* Optional GC flag function */
    .tp_bases = NULL,
    .tp_mro = NULL,
    .tp_cache = NULL,
    .tp_subclasses = NULL,
    .tp_weaklist = NULL,
    .tp_del = NULL,
    .tp_version_tag = 0,
    .tp_finalize = NULL,
    .tp_vectorcall = NULL
};
')
