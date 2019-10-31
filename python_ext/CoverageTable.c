#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t

#include "../include/avl_tree.h"    // vrd_avl_tree_destroy
#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_cov_table_*
#include "helpers.h"    // CFG_*, sample_set


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
    (void) kwds;

    size_t ref_capacity = CFG_REF_CAPACITY;
    size_t tree_capacity = CFG_TREE_CAPACITY;

    if (!PyArg_ParseTuple(args, "|nn:CoverageTable", &ref_capacity, &tree_capacity))
    {
        return NULL;
    } // if

    CoverageTableObject* const restrict self = (CoverageTableObject*) type->tp_alloc(type, 0);

    self->table = vrd_cov_table_init(ref_capacity, tree_capacity);
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


static PyObject*
CoverageTable_remove(CoverageTableObject* const restrict self,
                     PyObject* const restrict args)
{
    PyObject* restrict list = NULL;

    if (!PyArg_ParseTuple(args, "O!:CoverageTable.remove", &PyList_Type, &list))
    {
        return NULL;
    } // if

    vrd_AVL_Tree* restrict subset = sample_set(list);
    if (NULL == subset)
    {
        return NULL;
    } // if

    size_t result = 0;
    Py_BEGIN_ALLOW_THREADS
    result = vrd_cov_table_remove(self->table, subset);
    vrd_avl_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // CoverageTable_remove


static PyObject*
CoverageTable_reorder(CoverageTableObject* const restrict self,
                      PyObject* const restrict args)
{
    (void) args;

    if (0 != vrd_cov_table_reorder(self->table))
    {
        PyErr_SetString(PyExc_RuntimeError, "CoverageTable.reorder: vrd_cov_table_reorder() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // CoverageTable_reorder


static PyObject*
CoverageTable_read(CoverageTableObject* const restrict self,
                   PyObject* const restrict args)
{
    char const* restrict path = NULL;

    if (!PyArg_ParseTuple(args, "s:CoverageTable.read", &path))
    {
        return NULL;
    } // if

    if (0 != vrd_cov_table_read(self->table, path))
    {
        PyErr_SetString(PyExc_RuntimeError, "CoverageTable.read: vrd_cov_table_read() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // CoverageTable_read


static PyObject*
CoverageTable_write(CoverageTableObject* const restrict self,
                    PyObject* const restrict args)
{
    char const* restrict path = NULL;

    if (!PyArg_ParseTuple(args, "s:CoverageTable.write", &path))
    {
        return NULL;
    } // if

    if (0 != vrd_cov_table_write(self->table, path))
    {
        PyErr_SetString(PyExc_RuntimeError, "CoverageTable.write: vrd_cov_table_write() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // CoverageTable_write


static PyMethodDef CoverageTable_methods[] =
{
    {"insert", (PyCFunction) CoverageTable_insert, METH_VARARGS,
     "insert(reference, start, end, sample_id)\n"
     "Insert a region in the :py:class:`CoverageTable`\n\n"
     ":param string reference: The reference sequence ID\n"
     ":param integer start: The start position of the region (included)\n"
     ":param integer end: The end position of the region (excluded)\n"
     ":param integer sample_id: The sample ID\n"},

    {"query", (PyCFunction) CoverageTable_query, METH_VARARGS,
     "query(reference, start, end [, subset])\n"
     "Query for covered regions in the :py:class:`CoverageTable`\n\n"
     ":param string reference: The reference sequence ID\n"
     ":param integer start: The start position of the region (included)\n"
     ":param integer end: The end position of the region (excluded)\n"
     ":param subset: A list of sample IDs (`integer`), defaults to `None`\n"
     ":type subset: list, optional\n"
     ":return: The number of contained covered regions\n"
     ":rtype: integer\n"},

    {"remove", (PyCFunction) CoverageTable_remove, METH_VARARGS,
     "remove(subset)\n"
     "Remove for covered regions in the :py:class:`CoverageTable`\n\n"
     ":param subset: A list of sample IDs (`integer`)\n"
     ":type subset: list\n"
     ":return: The number of removed covered regions\n"
     ":rtype: integer\n"},

    {"reorder", (PyCFunction) CoverageTable_reorder, METH_NOARGS,
     "reorder()\n"
     "Reorders all structures in the :py:class:`CoverageTable`\n\n"},

    {"read", (PyCFunction) CoverageTable_read, METH_VARARGS,
     "read(path)\n"
     "Read a :py:class:`CoverageTable` from files\n\n"
     ":param string path: A path including a prefix that identifies the files.\n"},

    {"write", (PyCFunction) CoverageTable_write, METH_VARARGS,
     "write(path)\n"
     "Write a :py:class:`CoverageTable` to files\n\n"
     ":param string path: A path including a prefix that identifies the files.\n"},

    {NULL, NULL, 0, NULL}  // sentinel
}; // CoverageTable_methods


static PyTypeObject CoverageTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.CoverageTable",
    .tp_doc = "CoverageTable([ref_capacity[, ref_size_capacity[, tree_capacity]]])\n"
              "This class is a conceptual representation of a database"
              "table containing covered regions on a reference sequence.\n\n"
              ":param ref_capacity:  defaults to :c:data:`CFG_REF_CAPACITY`\n"
              ":type ref_capacity: integer, optional\n"
              ":param tree_capacity:  defaults to :c:data:`CFG_REF_TREE_CAPACITY`\n"
              ":type tree_capacity: integer, optional\n",
    .tp_basicsize = sizeof(CoverageTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = CoverageTable_new,
    .tp_dealloc = (destructor) CoverageTable_dealloc,
    .tp_methods = CoverageTable_methods
}; // CoverageTable
