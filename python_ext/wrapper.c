#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include "../include/varda.h"   // vrd_*

#include <stddef.h>     // size_t
#include <stdio.h>      // fprintf
#include <stdlib.h>     // NULL, EXIT_*, malloc


enum
{
    ASCII_SIZE = 95
}; // constants


static inline size_t
ascii_to_idx(char const ch)
{
    if (isprint(ch))
    {
        return ch - ' ';
    } // if
    return 256;
} // ascii_to_idx


typedef struct
{
    PyObject_HEAD
    vrd_Trie* restrict trie;
} RegionTableObject;


static PyObject*
RegionTable_new(PyTypeObject* const restrict type,
                PyObject* const restrict args,
                PyObject* const restrict kwds)
{
    (void) args;
    (void) kwds;

    RegionTableObject* const restrict self = (RegionTableObject*) type->tp_alloc(type, 0);

    self->trie = vrd_trie_init(ASCII_SIZE, ascii_to_idx);
    if (NULL == self->trie)
    {
        Py_TYPE(self)->tp_free((PyObject*) self);
        PyErr_SetString(PyExc_RuntimeError, "RegionTable: vrd_trie_init() failed");
        return NULL;
    } // if

    return (PyObject*) self;
} // RegionTable_new


static void
RegionTable_dealloc(RegionTableObject* const restrict self)
{
    vrd_trie_destroy(&self->trie);
    Py_TYPE(self)->tp_free((PyObject *) self);
} // RegionTable_dealloc


static PyObject*
RegionTable_load(RegionTableObject* const restrict self,
                 PyObject* const restrict args)
{
    char const* restrict reference_id = NULL;
    char const* restrict path = NULL;
    size_t len = 0;

    if (!PyArg_ParseTuple(args, "s#s", &reference_id, &len, &path))
    {
        return NULL;
    } // if

    // TODO: load an index from path
    void* const restrict index = NULL;

    void* const restrict result = vrd_trie_insert(self->trie, len, reference_id, index);

    if (0 == result)
    {
        PyErr_SetString(PyExc_RuntimeError, "RegionTable.load(): vrd_trie_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // RegionTable_load


static PyMethodDef RegionTable_methods[] =
{
    {"load", (PyCFunction) RegionTable_load, METH_VARARGS,
     "Docstring..."},

    {NULL}  // sentinel
}; // RegionTable_methods


static PyTypeObject RegionTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.RegionTable",
    .tp_doc = "Docstring...",
    .tp_basicsize = sizeof(RegionTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = RegionTable_new,
    .tp_dealloc = (destructor) RegionTable_dealloc,
    .tp_methods = RegionTable_methods
}; // RegionTable


static struct PyModuleDef module =
{
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "cvarda",
    .m_doc = "Varda2 Variant frequency database C library",
    .m_size = -1
}; // module


PyMODINIT_FUNC
PyInit_cvarda(void)
{
    if (0 > PyType_Ready(&RegionTable))
    {
        return NULL;
    } // if

    PyObject* const restrict mod = PyModule_Create(&module);
    if (NULL == mod)
    {
        return NULL;
    } // if

    int major = 0;
    int minor = 0;
    int patch = 0;
    vrd_version(&major, &minor, &patch);

    if (0 > PyModule_AddIntConstant(mod, "VERSION_MAJOR", major))
    {
        return NULL;
    } // if
    if (0 > PyModule_AddIntConstant(mod, "VERSION_MINOR", minor))
    {
        return NULL;
    } // if
    if (0 > PyModule_AddIntConstant(mod, "VERSION_PATCH", patch))
    {
        return NULL;
    } // if

    Py_INCREF(&RegionTable);
    if (0 > PyModule_AddObject(mod, "RegionTable", (PyObject*) &RegionTable))
    {
        return NULL;
    } // if

    return mod;
} // PyInit_cvarda


int
main(int argc, char* argv[])
{
    (void) argc;

    wchar_t* const restrict program = Py_DecodeLocale(argv[0], NULL);
    if (NULL == program)
    {
        fprintf(stderr, "Py_DecodeLocale(): cannot decode argv[0]\n");
        return EXIT_FAILURE;
    } // if

    int const ret = PyImport_AppendInittab("cvarda", PyInit_cvarda);
    if (-1 == ret)
    {
        PyMem_RawFree(program);
        fprintf(stderr, "PyImport_AppendInittab() failed\n");
        return EXIT_FAILURE;
    } // if

    Py_SetProgramName(program);
    Py_Initialize();

    PyMem_RawFree(program);

    return EXIT_SUCCESS;
} // main
