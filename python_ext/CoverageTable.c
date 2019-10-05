#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t

#include "../include/varda.h"   // vrd_*

#include "helpers.h"    // sample_set


typedef struct
{
    PyObject_HEAD
    vrd_Cov_Table* table;
} CoverageTableObject;


static PyObject*
CoverageTable_new(PyTypeObject* const restrict type,
                  PyObject* const restrict args,
                  PyObject* const restrict kwds)
{
    (void) args;
    (void) kwds;

    CoverageTableObject* const restrict self = (CoverageTableObject*) type->tp_alloc(type, 0);

    self->table = vrd_cov_table_init();
    if (NULL == self->table)
    {
        Py_TYPE(self)->tp_free((PyObject*) self);
        PyErr_SetString(PyExc_RuntimeError, "CoverageTable: vrd_cov_table_init() failed");
        return NULL;
    } // if

    return (PyObject*) self;
} // CoverageTable_new


static void
CoverageTable_dealloc(CoverageTableObject* const self)
{
    vrd_cov_table_destroy(&self->table);
    Py_TYPE(self)->tp_free((PyObject*) self);
} // CoverageTable_dealloc


static PyObject*
CoverageTable_insert(CoverageTableObject* const restrict self,
                     PyObject* const restrict args)
{
    char const* restrict reference = NULL;
    size_t len = 0;
    int start = 0;
    int end = 0;
    int sample_id = 0;

    if (!PyArg_ParseTuple(args, "s#iii:CoverageTable.insert", &reference, &len, &start, &end, &sample_id))
    {
        return NULL;
    } // if

    if (-1 == vrd_cov_table_insert(self->table, len, reference, start, end, sample_id))
    {
        PyErr_SetString(PyExc_RuntimeError, "CoverageTable.insert: vrd_cov_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // CoverageTable_insert


static PyObject*
CoverageTable_query(CoverageTableObject* const restrict self,
                    PyObject* const restrict args)
{
    char const* restrict reference = NULL;
    size_t len = 0;
    int start = 0;
    int end = 0;
    PyObject* restrict list = NULL;

    if (!PyArg_ParseTuple(args, "s#ii|O!:CoverageTable.query", &reference, &len, &start, &end, &PyList_Type, &list))
    {
        return NULL;
    } // if

    vrd_AVL_Tree* restrict subset = NULL;
    if (NULL != list)
    {
        subset = sample_set(list);
        if (NULL == subset)
        {
            return NULL;
        } // if
    } // if

    size_t result = 0;
    Py_BEGIN_ALLOW_THREADS
    result = vrd_cov_table_query(self->table, len, reference, start, end, subset);
    vrd_avl_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // CoverageTable_query


static PyMethodDef CoverageTable_methods[] =
{
    {"insert", (PyCFunction) CoverageTable_insert, METH_VARARGS,
     "Insert a region in the :py:class:`CoverageTable`\n\n"
     ":param str reference: The reference sequence ID\n"
     ":param int start: The start position of the region (included)\n"
     ":param int end: The end position of the region (excluded)\n"
     ":param int sample_id: The sample ID\n"},

    {"query", (PyCFunction) CoverageTable_query, METH_VARARGS,
     "Query for covered regions in the :py:class:`CoverageTable`\n\n"
     ":param str reference: The reference sequence ID\n"
     ":param int start: The start position of the region (included)\n"
     ":param int end: The end position of the region (excluded)\n"
     ":param subset: A list of sample IDs, defaults to None\n"
     ":type subset: list, optional\n"
     ":return: The number of contained covered regions\n"
     ":rtype: integer\n"},

    {NULL}  // sentinel
}; // CoverageTable_methods


static PyTypeObject CoverageTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.CoverageTable",
    .tp_doc = "This class is a conceptual representation of a database"
              "table containing covered regions on a reference sequence.",
    .tp_basicsize = sizeof(CoverageTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = CoverageTable_new,
    .tp_dealloc = (destructor) CoverageTable_dealloc,
    .tp_methods = CoverageTable_methods
}; // CoverageTable
