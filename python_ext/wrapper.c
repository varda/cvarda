#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS

#include <stdio.h>      // fprintf
#include <stdlib.h>     // NULL, EXIT_*


static PyObject*
insert_region(PyObject* const restrict self,
              PyObject* const restrict args)
{
    (void) self;

    Py_ssize_t sample_id = 0;
    char const* restrict reference = NULL;
    Py_ssize_t len = 0;
    Py_ssize_t start = 0;
    Py_ssize_t end = 0;
    Py_ssize_t phase = 0;

    if (!PyArg_ParseTuple(args, "ns#nnn", &sample_id,
                                          &reference,
                                          &len,
                                          &start,
                                          &end,
                                          &phase))
    {
        return NULL;
    } // if

    (void) fprintf(stderr, "ok\n");


    return Py_BuildValue("n", 0);
} // insert_region


static PyMethodDef methods[] =
{
    {"insert_region", insert_region, METH_VARARGS,
     "insert_region(sample_id, reference, start, end, phase)\n\n"
     "Inserts a new region [start, end) for sample sample_id on the "
     "reference sequence. Phasing information."},

    {NULL, NULL, 0, NULL}  // sentinel
}; // methods


static struct PyModuleDef module =
{
    PyModuleDef_HEAD_INIT,
    "cvarda",
    "Varda2 Variant frequency database C library\n",
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
