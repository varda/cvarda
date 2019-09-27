#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include "../include/varda.h"   // vrd_*, VRD_*

#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // NULL, EXIT_*


typedef struct
{
    PyObject_HEAD
    vrd_Region_Table* restrict table;
} RegionTableObject;


static PyObject*
RegionTable_new(PyTypeObject* const restrict type,
                PyObject* const restrict args,
                PyObject* const restrict kwds)
{
    (void) args;
    (void) kwds;

    RegionTableObject* const restrict self = (RegionTableObject*) type->tp_alloc(type, 0);

    self->table = vrd_region_table_init();
    if (NULL == self->table)
    {
        Py_TYPE(self)->tp_free((PyObject*) self);
        PyErr_SetString(PyExc_RuntimeError, "RegionTable: vrd_region_table_init() failed");
        return NULL;
    } // if

    return (PyObject*) self;
} // RegionTable_new


static void
RegionTable_dealloc(RegionTableObject* const restrict self)
{
    vrd_region_table_destroy(&self->table);
    Py_TYPE(self)->tp_free((PyObject*) self);
} // RegionTable_dealloc


static PyObject*
RegionTable_insert(RegionTableObject* const restrict self,
                   PyObject* const restrict args)
{
    char const* restrict reference = NULL;
    size_t len = 0;
    uint32_t start = 0;
    uint32_t end = 0;
    uint32_t sample_id = 0;
    uint32_t phase = 0;

    if (!PyArg_ParseTuple(args, "s#IIII", &reference, &len, &start, &end, &sample_id, &phase))
    {
        return NULL;
    } // if

    if (-1 == vrd_region_table_insert(self->table, len, reference, start, end, sample_id, phase))
    {
        PyErr_SetString(PyExc_RuntimeError, "RegionTable.insert(): vrd_region_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // RegionTable_insert


static PyMethodDef RegionTable_methods[] =
{
    {"insert", (PyCFunction) RegionTable_insert, METH_VARARGS,
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
