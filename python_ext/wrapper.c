#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*

#include <errno.h>      // errno
#include <stddef.h>     // NULL
#include <stdio.h>      // FILE, fclose, fopen, fprintf, stderr
#include <stdlib.h>     // EXIT_*, calloc, free

#include "../include/varda.h"   // vrd_*

#include "CoverageTable.h"  // CoverageTable*
#include "MNVTable.h"       // MNVTable*
#include "SequenceTable.h"  // SequenceTable*
#include "SNVTable.h"       // SNVTable*
#include "utils.h"          // sample_set


static PyObject*
coverage_from_file(PyObject* const self, PyObject* const args)
{
    (void) self;

    char const* path = NULL;
    int sample_id = 0;
    CoverageTableObject* cov = NULL;

    if (!PyArg_ParseTuple(args, "siO!:coverage_from_file", &path, &sample_id, &CoverageTable, &cov))
    {
        return NULL;
    } // if

    errno = 0;
    FILE* stream = fopen(path, "r");
    if (NULL == stream)
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    size_t count = 0;
    Py_BEGIN_ALLOW_THREADS
    count = vrd_coverage_from_file(stream, cov->table, sample_id);
    Py_END_ALLOW_THREADS

    errno = 0;
    if (0 != fclose(stream))
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    return Py_BuildValue("i", count);
} // coverage_from_file


static PyObject*
variants_from_file(PyObject* const self, PyObject* const args)
{
    (void) self;

    char const* path = NULL;
    int sample_id = 0;
    SNVTableObject* snv = NULL;
    MNVTableObject* mnv = NULL;
    SequenceTableObject* seq = NULL;

    if (!PyArg_ParseTuple(args, "siO!O!O!:variants_from_file", &path, &sample_id, &SNVTable, &snv, &MNVTable, &mnv, &SequenceTable, &seq))
    {
        return NULL;
    } // if

    errno = 0;
    FILE* stream = fopen(path, "r");
    if (NULL == stream)
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    size_t count = 0;
    Py_BEGIN_ALLOW_THREADS
    count = vrd_variants_from_file(stream, snv->table, mnv->table, seq->table, sample_id);
    Py_END_ALLOW_THREADS

    errno = 0;
    if (0 != fclose(stream))
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    return Py_BuildValue("i", count);
} // variants_from_file


static PyObject*
annotate_from_file(PyObject* const self, PyObject* const args)
{
    (void) self;

    char const* in_path = NULL;
    char const* out_path = NULL;
    CoverageTableObject* cov = NULL;
    SNVTableObject* snv = NULL;
    MNVTableObject* mnv = NULL;
    SequenceTableObject* seq = NULL;
    PyObject* list = NULL;

    if (!PyArg_ParseTuple(args, "ssO!O!O!O!|O!:annotate_from_file", &out_path, &in_path, &CoverageTable, &cov, &SNVTable, &snv, &MNVTable, &mnv, &SequenceTable, &seq, &PyList_Type, &list))
    {
        return NULL;
    } // if

    errno = 0;
    FILE* istream = fopen(in_path, "r");
    if (NULL == istream)
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    errno = 0;
    FILE* ostream = fopen(out_path, "w");
    if (NULL == ostream)
    {
        fclose(istream);
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    vrd_AVL_Tree* subset = NULL;
    if (NULL != list)
    {
        subset = sample_set(list);
        if (NULL == subset)
        {
            fclose(istream);
            fclose(ostream);
            return NULL;
        } // if
    } // if

    size_t count = 0;
    Py_BEGIN_ALLOW_THREADS
    count = vrd_annotate_from_file(ostream, istream, cov->table, snv->table, mnv->table, seq->table, subset);
    Py_END_ALLOW_THREADS

    errno = 0;
    if (0 != fclose(istream))
    {
        fclose(ostream);
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    errno = 0;
    if (0 != fclose(ostream))
    {
        return PyErr_SetFromErrno(PyExc_OSError);
    } // if

    return Py_BuildValue("i", count);
} // annotate_from_file


static PyObject*
sample_count(PyObject* const self, PyObject* const args)
{
    (void) self;

    CoverageTableObject* cov = NULL;
    SNVTableObject* snv = NULL;
    MNVTableObject* mnv = NULL;

    if (!PyArg_ParseTuple(args, "O!O!O!:sample_count", &CoverageTable, &cov, &SNVTable, &snv, &MNVTable, &mnv))
    {
        return NULL;
    } // if

    size_t* const count = calloc(VRD_MAX_SAMPLE_ID, sizeof(*count));
    if (NULL == count)
    {
        return PyErr_NoMemory();
    } // if

    size_t max_sample_id = 0;
    {
        size_t const table_max_sample_id = vrd_Cov_table_sample_count(cov->table, count);
        if (table_max_sample_id > max_sample_id)
        {
            max_sample_id = table_max_sample_id;
        } // if
    }
    {
        size_t const table_max_sample_id = vrd_SNV_table_sample_count(snv->table, count);
        if (table_max_sample_id > max_sample_id)
        {
            max_sample_id = table_max_sample_id;
        } // if
    }
    {
        size_t const table_max_sample_id = vrd_MNV_table_sample_count(mnv->table, count);
        if (table_max_sample_id > max_sample_id)
        {
            max_sample_id = table_max_sample_id;
        } // if
    }

    PyObject* const result = PyList_New(max_sample_id + 1);
    if (NULL == result)
    {
        free(count);
        return PyErr_NoMemory();
    } // if

    for (size_t i = 0; i <= max_sample_id; ++i)
    {
        PyObject* const item = Py_BuildValue("i", count[i]);
        if (NULL == item)
        {
            Py_DECREF(result);
            free(count);
            return PyErr_NoMemory();
        } // if

        if (0 != PyList_SetItem(result, i, item))
        {
            Py_DECREF(item);
            Py_DECREF(result);
            free(count);
            return PyErr_NoMemory();
        } // if
    } // for

    free(count);
    return result;
} // sample_count


static PyMethodDef methods[] =
{
    {"coverage_from_file", (PyCFunction) coverage_from_file, METH_VARARGS,
     "coverage_from_file(path, sample_id, cov_table)\n"
     "Import covered regions for a given sample from a file\n\n"
     ":param string path: The file path\n"
     ":param int sample_id: The sample ID\n"
     ":param cov_table: The coverage table\n"
     ":type cov_table: :py:class:`CoverageTable`\n"
     ":return: The number of inserted covered regions\n"
     ":rtype: integer\n"},

    {"variants_from_file", (PyCFunction) variants_from_file, METH_VARARGS,
     "variants_from_file(path, sample_id, snv_table, mnv_table, seq_table)\n"
     "Import variants for a given sample from a file\n\n"
     ":param string path: The file path\n"
     ":param int sample_id: The sample ID\n"
     ":param snv_table: The SNV table\n"
     ":type snv_table: :py:class:`SNVTable`\n"
     ":param mnv_table: The MNV table\n"
     ":type mnv_table: :py:class:`MNVTable`\n"
     ":param seq_table: The Sequence table\n"
     ":type seq_table: :py:class:`SequenceTable`\n"
     ":return: The number of inserted variants\n"
     ":rtype: integer\n"},

    {"annotate_from_file", (PyCFunction) annotate_from_file, METH_VARARGS,
     "annotate_from_file(out_path, in_path, cov_table, snv_table, mnv_table, seq_table[, subset])\n"
     "Annotate variants in the input file against (a subset) of the database\n\n"
     ":param string out_path: The file path for the annotation (output)\n"
     ":param string in_path: The file path for the variants (input)\n"
     ":param cov_table: The coverage table\n"
     ":type cov_table: :py:class:`CoverageTable`\n"
     ":param snv_table: The SNV table\n"
     ":type snv_table: :py:class:`SNVTable`\n"
     ":param mnv_table: The MNV table\n"
     ":type mnv_table: :py:class:`MNVTable`\n"
     ":param seq_table: The Sequence table\n"
     ":type seq_table: :py:class:`SequenceTable`\n"
     ":param subset: A list of sample IDs (`integer`), defaults to `None`\n"
     ":type subset: list, optional\n"
     ":return: The number of annotated variants\n"
     ":rtype: integer\n"},

    {"sample_count", (PyCFunction) sample_count, METH_VARARGS,
     "sample_count(cov_table, snv_table, mnv_table)\n"
     "Count the number of entries (coverage regions and variants) for each sample ID in the database\n\n"
     ":param cov_table: The coverage table\n"
     ":type cov_table: :py:class:`CoverageTable`\n"
     ":param snv_table: The SNV table\n"
     ":type snv_table: :py:class:`SNVTable`\n"
     ":param mnv_table: The MNV table\n"
     ":type mnv_table: :py:class:`MNVTable`\n"
     ":return: A list of entry counts per sample ID\n"
     ":rtype: list of integers\n"},

    {NULL, NULL, 0, NULL}  // sentinel
}; // methods


static struct PyModuleDef module =
{
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "cvarda",
    .m_doc = "Python module around the Varda2 C library",
    .m_size = -1,
    .m_methods = methods
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

    if (0 > PyType_Ready(&SequenceTable))
    {
        return NULL;
    } // if

    if (0 > PyType_Ready(&SNVTable))
    {
        return NULL;
    } // if

    PyObject* const mod = PyModule_Create(&module);
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

    if (0 > PyModule_AddIntConstant(mod, "MAX_POSITION", VRD_MAX_POSITION))
    {
        return NULL;
    } // if
    if (0 > PyModule_AddIntConstant(mod, "MAX_ALLELE_COUNT", VRD_MAX_ALLELE_COUNT))
    {
        return NULL;
    } // if
    if (0 > PyModule_AddIntConstant(mod, "MAX_SAMPLE_ID", VRD_MAX_SAMPLE_ID))
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

    Py_INCREF(&SequenceTable);
    if (0 > PyModule_AddObject(mod, "SequenceTable", (PyObject*) &SequenceTable))
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

    wchar_t* const program = Py_DecodeLocale(argv[0], NULL);
    if (NULL == program)
    {
        fprintf(stderr, "Py_DecodeLocale(): cannot decode argv[0]\n");
        return EXIT_FAILURE;
    } // if

    if (-1 == PyImport_AppendInittab("cvarda", PyInit_cvarda))
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
