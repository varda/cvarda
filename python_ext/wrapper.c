#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include "../include/varda.h"

#include <stddef.h>     // size_t
#include <stdio.h>      // fprintf
#include <stdlib.h>     // NULL, EXIT_*, malloc


typedef struct
{
    PyObject_HEAD
    char* restrict data;
} RegionTableObject;


static PyObject*
RegionTable_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    (void) args;
    (void) kwds;

    RegionTableObject* const restrict self = (RegionTableObject*) type->tp_alloc(type, 0);
    size_t const size = 1ULL << 30;
    self->data = malloc(size);
    if (NULL == self->data)
    {
        Py_TYPE(self)->tp_free((PyObject*) self);
        return PyErr_NoMemory();
    } // if

    for (size_t i = 0; i < size; ++i)
    {
        self->data[i] = i % 256;
    } // for

    return (PyObject*) self;
} // RegionTable_new


static void
RegionTable_dealloc(RegionTableObject* self)
{
    free(self->data);
    Py_TYPE(self)->tp_free((PyObject *) self);
} // RegionTable_dealloc


static PyTypeObject RegionTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.RegionTable",
    .tp_doc = "Docstring...",
    .tp_basicsize = sizeof(RegionTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = RegionTable_new,
    .tp_dealloc = (destructor) RegionTable_dealloc
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
