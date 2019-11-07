#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t

#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_AVL_tree_*
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_MNV_table_*
#include "utils.h"          // CFG_*, sample_set
#include "MNVTable.h"       // MNVTable*
#include "SequenceTable.h"  // SequenceTable*


#define VRD_TYPENAME MNV
#define VRD_OBJNAME MNVTable


#include "template_table.inc"   // MNVTable_*


#undef VRD_TYPENAME
#undef VRD_OBJNAME


static PyObject*
MNVTable_insert(MNVTableObject* const self,
                PyObject* const args)
{
    char const* reference = NULL;
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

    if (0 != vrd_MNV_table_insert(self->table, len, reference, start, end, sample_id, phase, inserted))
    {
        PyErr_SetString(PyExc_RuntimeError, "MNVTable.insert: vrd_MNV_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // MNVTable_insert


static PyObject*
MNVTable_query(MNVTableObject* const self,
               PyObject* const args)
{
    char const* reference = NULL;
    size_t len = 0;
    int start = 0;
    int end = 0;
    int inserted = 0;
    PyObject* list = NULL;

    if (!PyArg_ParseTuple(args, "s#ii|iO!:MNVTable.query", &reference, &len, &start, &end, &inserted, &PyList_Type, &list))
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
    result = vrd_MNV_table_query_stab(self->table, len, reference, start, end, inserted, subset);
    vrd_AVL_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // MNVTable_query


static PyObject*
MNVTable_remove(MNVTableObject* const self,
                PyObject* const args)
{
    SequenceTableObject* seq = NULL;
    PyObject* list = NULL;

    if (!PyArg_ParseTuple(args, "O!O!:MNVTable.remove", &SequenceTable, &seq, &PyList_Type, &list))
    {
        return NULL;
    } // if

    vrd_AVL_Tree* subset = sample_set(list);
    if (NULL == subset)
    {
        return NULL;
    } // if

    int ret = 0;
    Py_BEGIN_ALLOW_THREADS
    ret = vrd_MNV_table_remove_seq(self->table, subset, seq->table);
    vrd_AVL_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    if (0 != ret)
    {
        PyErr_SetString(PyExc_RuntimeError, "MNVTable.remove: vrd_MNV_table_remove() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
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
     ":type subset: list\n"},

    {"reorder", (PyCFunction) MNVTable_reorder, METH_NOARGS,
     "reorder()\n"
     "Reorders all structures in the :py:class:`MNVTable`\n\n"},

    {"read", (PyCFunction) MNVTable_read, METH_VARARGS,
     "read(path)\n"
     "Read a :py:class:`MNVTable` from files\n\n"
     ":param string path: A path including a prefix that identifies the files.\n"},

    {"write", (PyCFunction) MNVTable_write, METH_VARARGS,
     "write(path)\n"
     "Write a :py:class:`MNVTable` to files\n\n"
     ":param string path: A path including a prefix that identifies the files.\n"},

    {NULL, NULL, 0, NULL}  // sentinel
}; // MNVTable_methods


PyTypeObject MNVTable =
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
