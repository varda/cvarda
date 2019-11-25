#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t

#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_AVL_tree_*
#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_Cov_table_*
#include "utils.h"          // CFG_*, sample_set
#include "CoverageTable.h"  // CoverageTable*


#define VRD_TYPENAME Cov
#define VRD_OBJNAME CoverageTable


#include "template_table.inc"   // CoverageTable_*


#undef VRD_TYPENAME
#undef VRD_OBJNAME


static PyObject*
CoverageTable_insert(CoverageTableObject* const self,
                     PyObject* const args)
{
    char const* reference = NULL;
    size_t len = 0;
    int start = 0;
    int end = 0;
    int sample_id = 0;

    if (!PyArg_ParseTuple(args, "s#iii:CoverageTable.insert", &reference, &len, &start, &end, &sample_id))
    {
        return NULL;
    } // if

    int ret = 0;
    Py_BEGIN_ALLOW_THREADS
    ret = vrd_Cov_table_insert(self->table, len, reference, start, end, sample_id);
    Py_END_ALLOW_THREADS

    if (0 != ret)
    {
        PyErr_SetString(PyExc_RuntimeError, "CoverageTable.insert: vrd_Cov_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // CoverageTable_insert


static PyObject*
CoverageTable_query(CoverageTableObject* const self,
                    PyObject* const args)
{
    char const* reference = NULL;
    size_t len = 0;
    int start = 0;
    int end = 0;
    PyObject* list = NULL;

    if (!PyArg_ParseTuple(args, "s#ii|O!:CoverageTable.query", &reference, &len, &start, &end, &PyList_Type, &list))
    {
        return NULL;
    } // if

    vrd_AVL_Tree* subset = NULL;
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
    result = vrd_Cov_table_query_stab(self->table, len, reference, start, end, subset);
    vrd_AVL_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // CoverageTable_query


static PyObject*
CoverageTable_remove(CoverageTableObject* const self,
                     PyObject* const args)
{
    PyObject* list = NULL;

    if (!PyArg_ParseTuple(args, "O!:CoverageTable.remove", &PyList_Type, &list))
    {
        return NULL;
    } // if

    vrd_AVL_Tree* subset = sample_set(list);
    if (NULL == subset)
    {
        return NULL;
    } // if

    int result = 0;
    Py_BEGIN_ALLOW_THREADS
    result = vrd_Cov_table_remove(self->table, subset);
    vrd_AVL_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // CoverageTable_remove


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

    {"diagnostics", (PyCFunction) CoverageTable_diagnostics, METH_NOARGS,
     "diagnostics()\n"
     "Gives diagnostic information about the structures in the :py:class:`CoverageTable`\n\n"
     "return: Diagnostics information\n"
     "rtype: dictionary\n"},

    {NULL, NULL, 0, NULL}  // sentinel
}; // CoverageTable_methods


PyTypeObject CoverageTable =
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
