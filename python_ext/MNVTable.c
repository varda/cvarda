#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t

#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "helpers.h"    // CFG_*

#include "SequenceTable.c"  // SequenceTable, SequenceTableObject


typedef struct
{
    PyObject_HEAD
    vrd_MNV_Table* table;
} MNVTableObject;


static PyObject*
MNVTable_new(PyTypeObject* const restrict type,
             PyObject* const restrict args,
             PyObject* const restrict kwds)
{
    (void) kwds;

    size_t ref_capacity = CFG_REF_CAPACITY;
    size_t tree_capacity = CFG_TREE_CAPACITY;

    if (!PyArg_ParseTuple(args, "|nn:MNVTable", &ref_capacity, &tree_capacity))
    {
        return NULL;
    } // if

    MNVTableObject* const restrict self = (MNVTableObject*) type->tp_alloc(type, 0);

    self->table = vrd_mnv_table_init(ref_capacity, tree_capacity);
    if (NULL == self->table)
    {
        Py_TYPE(self)->tp_free((PyObject*) self);
        PyErr_SetString(PyExc_RuntimeError, "MNVTable: vrd_mnv_table_init() failed");
        return NULL;
    } // if

    return (PyObject*) self;
} // MNVTable_new


static void
MNVTable_dealloc(MNVTableObject* const self)
{
    vrd_mnv_table_destroy(&self->table);
    Py_TYPE(self)->tp_free((PyObject*) self);
} // MNVTable_dealloc


static PyObject*
MNVTable_insert(MNVTableObject* const restrict self,
                PyObject* const restrict args)
{
    char const* restrict reference = NULL;
    size_t len = 0;
    int start = 0;
    int end = 0;
    int sample_id = 0;
    int inserted = 0;
    int phase = 0;

    if (!PyArg_ParseTuple(args, "s#iii|ii:MNVTable.insert", &reference, &len, &start, &end, &sample_id, &inserted, &phase))
    {
        return NULL;
    } // if

    if (-1 == vrd_mnv_table_insert(self->table, len, reference, start, end, sample_id, phase, inserted))
    {
        PyErr_SetString(PyExc_RuntimeError, "MNVTable.insert: vrd_mnv_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // MNVTable_insert


static PyObject*
MNVTable_query(MNVTableObject* const restrict self,
               PyObject* const restrict args)
{
    char const* restrict reference = NULL;
    size_t len = 0;
    int start = 0;
    int end = 0;
    int inserted = 0;
    PyObject* restrict list = NULL;

    if (!PyArg_ParseTuple(args, "s#ii|iO!:MNVTable.insert", &reference, &len, &start, &end, &inserted, &PyList_Type, &list))
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
    result = vrd_mnv_table_query(self->table, len, reference, start, end, inserted, subset);
    vrd_avl_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // MNVTable_query


static PyObject*
MNVTable_remove(MNVTableObject* const restrict self,
                PyObject* const restrict args)
{
    SequenceTableObject* restrict seq = NULL;
    PyObject* restrict list = NULL;

    if (!PyArg_ParseTuple(args, "O!O!:MNVTable.remove", &SequenceTable, &seq, &PyList_Type, &list))
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
    result = vrd_mnv_table_remove(self->table, subset, seq->table);
    vrd_avl_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // MNVTable_remove


static PyMethodDef MNVTable_methods[] =
{
    {"insert", (PyCFunction) MNVTable_insert, METH_VARARGS,
     "insert(reference, start, end, sample_id, type [, phase])\n"
     "Insert a region in the :py:class:`MNVTable`\n\n"
     ":param string reference: The reference sequence ID\n"
     ":param integer start: The start position of the deleted part of the MNV\n"
     ":param integer end: The end position of the deleted part of the MNV\n"
     ":param integer sample_id: The sample ID\n"
     ":param integer inserted: A reference to an object stored in :py:class:`SequenceTable`\n"
     ":param phase: The phase group (position based)\n"
     ":type phase: integer, optional\n"},

    {"query", (PyCFunction) MNVTable_query, METH_VARARGS,
     "query(reference, start, end, inserted[, subset])\n"
     "Query for MNVs in the :py:class:`MNVTable`\n\n"
     ":param string reference: The reference sequence ID\n"
     ":param integer start: The start position of the deleted part of the MNV\n"
     ":param integer end: The end position of the deleted part of the MNV\n"
     ":param integer inserted: A reference to an object stored in :py:class:`SequenceTable`\n"
     ":param subset: A list of sample IDs (`integer`), defaults to `None`\n"
     ":type subset: list, optional\n"
     ":return: The number of contained MNVs\n"
     ":rtype: integer\n"},

    {"remove", (PyCFunction) MNVTable_remove, METH_VARARGS,
     "remove(subset)\n"
     "Remove for MNVs in the :py:class:`MNVTable`\n\n"
     ":param subset: A list of sample IDs (`integer`)\n"
     ":type subset: list\n"
     ":return: The number of removed MNVs\n"
     ":rtype: integer\n"},

    {NULL, NULL, 0, NULL}  // sentinel
}; // MNVTable_methods


static PyTypeObject MNVTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.MNVTable",
    .tp_doc = "MNVTable([ref_capacity[, ref_size_capacity[, tree_capacity]]])\n"
              "This class is a conceptual representation of a database"
              "table containing multi nucleotide variants (MNV).\n\n"
              ":param ref_capacity:  defaults to :c:data:`CFG_REF_CAPACITY`\n"
              ":type ref_capacity: integer, optional\n"
              ":param tree_capacity:  defaults to :c:data:`CFG_REF_TREE_CAPACITY`\n"
              ":type tree_capacity: integer, optional\n",
    .tp_basicsize = sizeof(MNVTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = MNVTable_new,
    .tp_dealloc = (destructor) MNVTable_dealloc,
    .tp_methods = MNVTable_methods
}; // MNVTable
