#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS

#include <stdio.h>      // fprintf
#include <stdlib.h>     // NULL, EXIT_*


static PyObject*
init(PyObject* const restrict self, PyObject* const restrict args)
{
    (void) self;
    (void) args;

    return Py_BuildValue("i", 0);
} // init


static PyMethodDef methods[] =
{
    {"init", init, METH_VARARGS,
     "Docstring...\n"},

    {NULL, NULL, 0, NULL}  // sentinel
}; // methods


static struct PyModuleDef module =
{
    PyModuleDef_HEAD_INIT,
    "cvarda",
    "Variant frequency database\n",
    -1,
    methods,
    NULL,
    NULL,
    NULL,
    NULL
}; // module


PyMODINIT_FUNC
PyInit_cvarda(void)
{
    return PyModule_Create(&module);
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
