#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, flcose, fopen
#include <stdlib.h>     // calloc, free

#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_AVL_tree_*
#include "../include/iupac.h"       // vrd_iuapc_to_idx
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_SNV_table_*
#include "utils.h"      // CFG_*, sample_set
#include "SNVTable.h"   // SNVTable*


#define VRD_TYPENAME SNV
#define VRD_OBJNAME SNVTable

#include "template_table.inc"   // SNVTable_*
#include "../src/snv_tree.h"    // vrd_SNV_unpack

#undef VRD_TYPENAME
#undef VRD_OBJNAME


static PyObject*
SNVTable_insert(SNVTableObject* const self, PyObject* const args)
{
    char const* reference = NULL;
    size_t len = 0;
    size_t position = 0;
    size_t allele_count = 0;
    size_t sample_id = 0;
    char const* inserted = NULL;
    size_t len_inserted = 0;
    size_t phase = 0;

    if (!PyArg_ParseTuple(args, "s#nnns#|n:SNVTable.insert", &reference, &len, &position, &allele_count, &sample_id, &inserted, &len_inserted, &phase))
    {
        return NULL;
    } // if

    if (1 != len_inserted)
    {
        PyErr_SetString(PyExc_ValueError, "SNVTable.insert: expected one inserted nucleotide");
        return NULL;
    } // if

    if (0 != vrd_SNV_table_insert(self->table, len + 1, reference, position, allele_count, sample_id, phase, vrd_iupac_to_idx(inserted[0])))
    {
        PyErr_SetString(PyExc_RuntimeError, "SNVTable.insert: vrd_SNV_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // SNVTable_insert


static PyObject*
SNVTable_query(SNVTableObject* const self, PyObject* const args)
{
    char const* reference = NULL;
    size_t len = 0;
    size_t position = 0;
    char const* inserted = NULL;
    size_t len_inserted = 0;
    int homozygous = 0;
    size_t size = 0;
    PyObject* list = NULL;

    if (!PyArg_ParseTuple(args, "s#ns#|pnO!:SNVTable.query", &reference, &len, &position, &inserted, &len_inserted, &homozygous, &size, &PyList_Type, &list))
    {
        return NULL;
    } // if

    if (1 != len_inserted)
    {
        PyErr_SetString(PyExc_ValueError, "SNVTable.query: expected one inserted nucleotide");
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

    void** variant = NULL;
    if (size > 0)
    {
        variant = calloc(size, sizeof(*variant));
        if (NULL == variant)
        {
            size = 0;
        } // if
    } // if

    size_t count = 0;
    Py_BEGIN_ALLOW_THREADS
    count = vrd_SNV_table_query(self->table, len + 1, reference, position, vrd_iupac_to_idx(inserted[0]), homozygous != 0, subset, size, variant);
    Py_END_ALLOW_THREADS

    vrd_AVL_tree_destroy(&subset);

    if ((size_t) -1 == count)
    {
        free(variant);
        PyErr_SetString(PyExc_ValueError, "SNVTable.query: reference not found");
        return NULL;
    } // if

    PyObject* const result = PyList_New(0);
    if (NULL == result)
    {
        free(variant);
        return PyErr_NoMemory();
    } // if

    if (NULL != variant)
    {
        for (size_t i = 0; i < size && NULL != variant[i]; ++i)
        {
            size_t sample_id = 0;
            vrd_SNV_unpack(variant[i], &(size_t){0}, &(size_t){0}, &sample_id, &(size_t){0}, &(char){'\0'});
            PyObject* const item = Py_BuildValue("i", sample_id);
            if (NULL == item)
            {
                Py_DECREF(result);
                free(variant);
                return PyErr_NoMemory();
            } // if

            if (0 != PyList_Append(result, item))
            {
                Py_DECREF(item);
                Py_DECREF(result);
                free(variant);
                return PyErr_NoMemory();
            } // if

            Py_DECREF(item);
        } // for

        free(variant);

        return result;
    } // if

    return Py_BuildValue("i", count);
} // SNVTable_query


static PyObject*
SNVTable_query_region(SNVTableObject* const self, PyObject* const args)
{
    char const* reference = NULL;
    size_t len = 0;
    size_t start = 0;
    size_t end = 0;
    size_t size = 0;
    PyObject* list = NULL;

    if (!PyArg_ParseTuple(args, "s#nnn|O!:SNVTable.query_region", &reference, &len, &start, &end, &size, &PyList_Type, &list))
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

    void** const variant = calloc(size, sizeof(*variant));
    if (NULL == variant)
    {
        vrd_AVL_tree_destroy(&subset);
        return PyErr_NoMemory();
    } // if

    size_t count = 0;
    Py_BEGIN_ALLOW_THREADS
    count = vrd_SNV_table_query_region(self->table, len + 1, reference, start, end, subset, size, variant);
    Py_END_ALLOW_THREADS

    vrd_AVL_tree_destroy(&subset);

    if ((size_t) -1 == count)
    {
        free(variant);
        PyErr_SetString(PyExc_ValueError, "SNVTable.query_region: reference not found");
        return NULL;
    } // if

    PyObject* const result = PyList_New(count);
    if (NULL == result)
    {
        free(variant);
        return PyErr_NoMemory();
    } // if

    for (size_t i = 0; i < count; ++i)
    {
        size_t position = 0;
        size_t allele_count = 0;
        size_t sample_id = 0;
        size_t phase = 0;
        char inserted = '\0';

        vrd_SNV_unpack(variant[i], &position, &allele_count, &sample_id, &phase, &inserted);
        PyObject* const item = Py_BuildValue("{s:i,s:i,s:i,s:i,s:C}",
                                             "position", position,
                                             "allele_count", allele_count,
                                             "sample_id", sample_id,
                                             "phase", phase,
                                             "inserted", inserted);
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
} // SNVTable_query_region


static PyObject*
SNVTable_remove(SNVTableObject* const self, PyObject* const args)
{
    PyObject* list = NULL;

    if (!PyArg_ParseTuple(args, "O!:SNVTable.remove", &PyList_Type, &list))
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
    result = vrd_SNV_table_remove(self->table, subset);
    vrd_AVL_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // SNVTable_remove


static PyObject*
SNVTable_export(SNVTableObject* const self, PyObject* const args)
{
    char const* path = NULL;

    if (!PyArg_ParseTuple(args, "s:SNVTable.export", &path))
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
    vrd_SNV_table_export(self->table, stream);
    Py_END_ALLOW_THREADS

    if (0 != fclose(stream))
    {
        PyErr_SetFromErrno(PyExc_OSError);
        return NULL;
    } // if

    return Py_BuildValue("i", result);
} // SNVTable_export


static PyMethodDef SNVTable_methods[] =
{
    {"insert", (PyCFunction) SNVTable_insert, METH_VARARGS,
     "insert(reference, position, allele_count, sample_id, inserted[, phase])\n"
     "Insert a region in the :py:class:`SNVTable`\n\n"
     ":param string reference: The reference sequence ID\n"
     ":param integer position: The start position of the SNV\n"
     ":param integer allele_count: The allele count of the SNV\n"
     ":param integer sample_id: The sample ID\n"
     ":param string inserted: The inserted nucleotide from IUPAC\n"
     ":param phase: The phase group (position based)\n"
     ":type phase: integer, optional\n"},

    {"query", (PyCFunction) SNVTable_query, METH_VARARGS,
     "query(reference, position, inserted[, homozygous, size, subset])\n"
     "Query for SNVs in the :py:class:`SNVTable`\n\n"
     ":param string reference: The reference sequence ID\n"
     ":param integer position: The position of the SNV\n"
     ":param string inserted: The inserted nucleotide from IUPAC\n"
     ":param bool homozygous: Toggle to only count homozygous variants\n"
     ":param integer size: The maximum size of the result vector\n"
     ":param subset: A list of sample IDs (`integer`), defaults to `None`\n"
     ":type subset: list, optional\n"
     ":return: The list contained SNVs\n"
     ":rtype: list of dictionaries\n"},

    {"query_region", (PyCFunction) SNVTable_query_region, METH_VARARGS,
     "query_region(reference, start, end, size[, subset])\n"
     "Query for SNVs in a region [start, end) in the :py:class:`SNVTable`\n\n"
     ":param string reference: The reference sequence ID\n"
     ":param integer start: The start of the region\n"
     ":param integer end: The end of the region\n"
     ":param integer size: The maximum size of the result vector\n"
     ":param subset: A list of sample IDs (`integer`), defaults to `None`\n"
     ":type subset: list, optional\n"
     ":return: A list of SNVs contained in the query interval\n"
     ":rtype: list of dictionaries\n"},

    {"remove", (PyCFunction) SNVTable_remove, METH_VARARGS,
     "remove(subset)\n"
     "Remove for SNVs in the :py:class:`SNVTable`\n\n"
     ":param subset: A list of sample IDs (`integer`)\n"
     ":type subset: list\n"
     ":return: The number of removed SNVs\n"
     ":rtype: integer\n"},

    {"reorder", (PyCFunction) SNVTable_reorder, METH_NOARGS,
     "reorder()\n"
     "Reorders all structures in the :py:class:`SNVTable`\n\n"},

    {"read", (PyCFunction) SNVTable_read, METH_VARARGS,
     "read(path)\n"
     "Read a :py:class:`SNVTable` from files\n\n"
     ":param string path: A path including a prefix that identifies the files\n"},

    {"write", (PyCFunction) SNVTable_write, METH_VARARGS,
     "write(path)\n"
     "Write a :py:class:`SNVTable` to files\n\n"
     ":param string path: A path including a prefix that identifies the files\n"},

    {"export", (PyCFunction) SNVTable_export, METH_VARARGS,
     "export(path, seq_table)\n"
     "Export a :py:class:`SNVTable`\n\n"
     ":param string path: A path including a prefix that identifies the file\n"
     ":return: The number of exported SNVs\n"
     ":rtype: integer\n"},

    {"diagnostics", (PyCFunction) SNVTable_diagnostics, METH_NOARGS,
     "diagnostics()\n"
     "Gives diagnostic information about the structures in the :py:class:`SNVTable`\n\n"
     ":return: Diagnostics information\n"
     ":rtype: dictionary\n"},

    {"preload", (PyCFunction) SNVTable_preload, METH_VARARGS,
     "preload(reference_id, capacity)\n"
     "Preload a `reference_id` with a specified `capacity` in the :py:class:`SNVTable`\n\n"
     ":param string reference_id: A reference sequence ID\n"
     ":param integer: The capacity for the reference\n"},

    {NULL, NULL, 0, NULL}  // sentinel
}; // SNVTable_methods


PyTypeObject SNVTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.ext.SNVTable",
    .tp_doc = "SNVTable([ref_capacity[, tree_capacity]]])\n"
              "Table containing single nucleotide variants (SNV).\n\n"
              ":param ref_capacity:  defaults to :c:data:`CFG_REF_CAPACITY`\n"
              ":type ref_capacity: integer, optional\n"
              ":param tree_capacity:  defaults to :c:data:`CFG_REF_TREE_CAPACITY`\n"
              ":type tree_capacity: integer, optional\n",
    .tp_basicsize = sizeof(SNVTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = SNVTable_new,
    .tp_dealloc = (destructor) SNVTable_dealloc,
    .tp_methods = SNVTable_methods
}; // SNVTable
