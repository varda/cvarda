#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, fopen fclose

#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_AVL_tree_*
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_MNV_table_*
#include "../include/seq_table.h"   // vrd_Seq_table_key
#include "utils.h"          // CFG_*, sample_set
#include "MNVTable.h"       // MNVTable*
#include "SequenceTable.h"  // SequenceTable*


#define VRD_TYPENAME MNV
#define VRD_OBJNAME MNVTable


#include "template_table.inc"   // MNVTable_*
#include "../src/mnv_tree.h"    // vrd_MNV_unpack


#undef VRD_TYPENAME
#undef VRD_OBJNAME


static PyObject*
MNVTable_insert(MNVTableObject* const self, PyObject* const args)
{
    char const* reference = NULL;
    size_t len = 0;
    size_t start = 0;
    size_t end = 0;
    size_t allele_count = 0;
    size_t sample_id = 0;
    size_t inserted = 0;
    size_t phase = 0;

    if (!PyArg_ParseTuple(args, "s#nnnn|nn:MNVTable.insert", &reference, &len, &start, &end, &allele_count, &sample_id, &inserted, &phase))
    {
        return NULL;
    } // if

    if (0 != vrd_MNV_table_insert(self->table, len + 1, reference, start, end, allele_count, sample_id, phase, inserted))
    {
        PyErr_SetString(PyExc_RuntimeError, "MNVTable.insert: vrd_MNV_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // MNVTable_insert


static PyObject*
MNVTable_query(MNVTableObject* const self, PyObject* const args)
{
    char const* reference = NULL;
    size_t len = 0;
    size_t start = 0;
    size_t end = 0;
    size_t inserted = 0;
    PyObject* list = NULL;

    if (!PyArg_ParseTuple(args, "s#nn|nO!:MNVTable.query", &reference, &len, &start, &end, &inserted, &PyList_Type, &list))
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
    result = vrd_MNV_table_query(self->table, len + 1, reference, start, end, inserted, subset);
    vrd_AVL_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // MNVTable_query


static PyObject*
MNVTable_remove(MNVTableObject* const self, PyObject* const args)
{
    PyObject* list = NULL;
    SequenceTableObject* seq = NULL;

    if (!PyArg_ParseTuple(args, "O!O!:MNVTable.remove", &PyList_Type, &list, &SequenceTable, &seq))
    {
        return NULL;
    } // if

    vrd_AVL_Tree* subset = sample_set(list);
    if (NULL == subset)
    {
        return NULL;
    } // if

    size_t result = 0;
    Py_BEGIN_ALLOW_THREADS
    result = vrd_MNV_table_remove_seq(self->table, subset, seq->table);
    vrd_AVL_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // MNVTable_remove


static PyObject*
MNVTable_query_region(MNVTableObject* const self, PyObject* const args)
{
    char const* reference = NULL;
    size_t len = 0;
    size_t start = 0;
    size_t end = 0;
    size_t size = 0;
    SequenceTableObject* seq = NULL;
    PyObject* list = NULL;

    if (!PyArg_ParseTuple(args, "s#nnnO!|O!:MNVTable.query_region", &reference, &len, &start, &end, &size, &SequenceTable, &seq, &PyList_Type, &list))
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

    // FIXME: overflow
    void** const variant = malloc(size * sizeof(*variant));
    if (NULL == variant)
    {
        vrd_AVL_tree_destroy(&subset);
        return PyErr_NoMemory();
    } // if

    size_t count = 0;
    Py_BEGIN_ALLOW_THREADS
    count = vrd_MNV_table_query_region(self->table, len + 1, reference, start, end, subset, size, variant);
    Py_END_ALLOW_THREADS

    vrd_AVL_tree_destroy(&subset);

    PyObject* const result = PyList_New(count);
    if (NULL == result)
    {
        free(variant);
        return PyErr_NoMemory();
    } // if

    for (size_t i = 0; i < count; ++i)
    {
        size_t v_start = 0;
        size_t v_end = 0;
        size_t allele_count = 0;
        size_t sample_id = 0;
        size_t phase = 0;
        size_t inserted = 0;

        vrd_MNV_unpack(variant[i], &v_start, &v_end, &allele_count, &sample_id, &phase, &inserted);
        char* seq_inserted = NULL;
        size_t const len = vrd_Seq_table_key(seq->table, inserted, &seq_inserted);
        PyObject* const item = Py_BuildValue("{s:i,s:i,s:i,s:i,s:i,s:s}", "start", v_start,
                                                                          "end", v_end,
                                                                          "allele_count", allele_count,
                                                                          "sample_id", sample_id,
                                                                          "phase", phase,
                                                                          "inserted", len == 1 ? "." : seq_inserted);
        free(seq_inserted);
        if (NULL == item)
        {
            Py_DECREF(result);
            free(variant);
            return PyErr_NoMemory();
        } // if

        if (0 != PyList_SetItem(result, i, item))
        {
            Py_DECREF(item);
            Py_DECREF(result);
            free(variant);
            return PyErr_NoMemory();
        } // if
    } // for

    free(variant);

    return result;
} // MNVTable_query_region


static PyObject*
MNVTable_export(MNVTableObject* const self, PyObject* const args)
{
    char const* path = NULL;
    SequenceTableObject* seq = NULL;

    if (!PyArg_ParseTuple(args, "sO!:MNVTable.export", &path, &SequenceTable, &seq))
    {
        return NULL;
    } // if

    FILE* stream = fopen(path, "w");
    if (NULL == stream)
    {
        PyErr_SetFromErrno(PyExc_OSError);
        return NULL;
    } // if

    size_t result = 0;
    Py_BEGIN_ALLOW_THREADS
    vrd_MNV_table_export(self->table, stream, seq->table);
    Py_END_ALLOW_THREADS

    if (0 != fclose(stream))
    {
        PyErr_SetFromErrno(PyExc_OSError);
        return NULL;
    } // if

    return Py_BuildValue("i", result);
} // MNVTable_export


static PyMethodDef MNVTable_methods[] =
{
    {"insert", (PyCFunction) MNVTable_insert, METH_VARARGS,
     "insert(reference, start, end, allele_count, sample_id, inserted[, phase])\n"
     "Insert a region in the :py:class:`MNVTable`\n\n"
     ":param string reference: The reference sequence ID\n"
     ":param integer start: The start position of the deleted part of the MNV\n"
     ":param integer end: The end position of the deleted part of the MNV\n"
     ":param integer allele_count: The allele count of the MNV\n"
     ":param integer sample_id: The sample ID\n"
     ":param integer inserted: The index for a sequence stored in :py:class:`SequenceTable`\n"
     ":param phase: The phase group (position based)\n"
     ":type phase: integer, optional\n"},

    {"query", (PyCFunction) MNVTable_query, METH_VARARGS,
     "query(reference, start, end, inserted[, subset])\n"
     "Query for MNVs in the :py:class:`MNVTable`\n\n"
     ":param string reference: The reference sequence ID\n"
     ":param integer start: The start position of the deleted part of the MNV\n"
     ":param integer end: The end position of the deleted part of the MNV\n"
     ":param integer inserted: The index for a sequence stored in :py:class:`SequenceTable`\n"
     ":param subset: A list of sample IDs (`integer`), defaults to `None`\n"
     ":type subset: list, optional\n"
     ":return: The number of contained MNVs\n"
     ":rtype: integer\n"},

    {"query_region", (PyCFunction) MNVTable_query_region, METH_VARARGS,
     "query_region(reference, start, end, size, seq_table[, subset])\n"
     "Query for MNVs in a region [start, end) in the :py:class:`MNVTable`\n\n"
     ":param string reference: The reference sequence ID\n"
     ":param integer start: The start of the region\n"
     ":param integer end: The end of the region\n"
     ":param integer size: The maximum size of the result vector\n"
     ":param seq_table: The sequence table\n"
     ":type seq_table: :py:class:`SequenceTable`\n"
     ":param subset: A list of sample IDs (`integer`), defaults to `None`\n"
     ":type subset: list, optional\n"
     ":return: A list of MNVs containted in the query interval\n"
     ":rtype: list of dictionaries\n"},

    {"remove", (PyCFunction) MNVTable_remove, METH_VARARGS,
     "remove(subset, seq_table)\n"
     "Remove for MNVs in the :py:class:`MNVTable`\n\n"
     ":param subset: A list of sample IDs (`integer`)\n"
     ":type subset: list\n"
     ":param seq_table: The sequence table\n"
     ":type seq_table: :py:class:`SequenceTable`\n"
     ":return: The number of removed MNVs\n"
     ":rtype: integer\n"},

    {"reorder", (PyCFunction) MNVTable_reorder, METH_NOARGS,
     "reorder()\n"
     "Reorders all structures in the :py:class:`MNVTable`\n\n"},

    {"read", (PyCFunction) MNVTable_read, METH_VARARGS,
     "read(path)\n"
     "Read a :py:class:`MNVTable` from files\n\n"
     ":param string path: A path including a prefix that identifies the files\n"},

    {"write", (PyCFunction) MNVTable_write, METH_VARARGS,
     "write(path)\n"
     "Write a :py:class:`MNVTable` to files\n\n"
     ":param string path: A path including a prefix that identifies the files\n"},

    {"export", (PyCFunction) MNVTable_export, METH_VARARGS,
     "export(path, seq_table)\n"
     "Export a :py:class:`MNVTable`\n\n"
     ":param string path: A path including a prefix that identifies the file\n"
     ":param seq_table: The sequence table\n"
     ":type seq_table: :py:class:`SequenceTable`\n"
     ":return: The number of exported MNVs\n"
     ":rtype: integer\n"},

    {"diagnostics", (PyCFunction) MNVTable_diagnostics, METH_NOARGS,
     "diagnostics()\n"
     "Gives diagnostic information about the structures in the :py:class:`MNVTable`\n\n"
     ":return: Diagnostics information\n"
     ":rtype: dictionary\n"},

    {NULL, NULL, 0, NULL}  // sentinel
}; // MNVTable_methods


PyTypeObject MNVTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.ext.MNVTable",
    .tp_doc = "MNVTable([ref_capacity[, tree_capacity]]])\n"
              "Table containing multi nucleotide variants (MNV).\n\n"
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
