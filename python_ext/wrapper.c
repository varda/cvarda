#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // NULL, EXIT_*

#include "../include/varda.h"   // vrd_*, VRD_*


#include "CoverageTable.c"  // CoverageTable
#include "MNVTable.c"       // MNVTable
#include "SNVTable.c"       // SNVTable


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
    if (0 > PyType_Ready(&CoverageTable))
    {
        return NULL;
    } // if

    if (0 > PyType_Ready(&MNVTable))
    {
        return NULL;
    } // if

    if (0 > PyType_Ready(&SNVTable))
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

    Py_INCREF(&CoverageTable);
    if (0 > PyModule_AddObject(mod, "CoverageTable", (PyObject*) &CoverageTable))
    {
        return NULL;
    } // if

    Py_INCREF(&MNVTable);
    if (0 > PyModule_AddObject(mod, "MNVTable", (PyObject*) &MNVTable))
    {
        return NULL;
    } // if

    Py_INCREF(&SNVTable);
    if (0 > PyModule_AddObject(mod, "SNVTable", (PyObject*) &SNVTable))
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
