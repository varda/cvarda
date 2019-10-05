#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t

#include "../include/varda.h"   // vrd_*


typedef struct
{
    PyObject_HEAD
    vrd_SNV_Table* table;
} SNVTableObject;


static PyObject*
SNVTable_new(PyTypeObject* const restrict type,
             PyObject* const restrict args,
             PyObject* const restrict kwds)
{
    (void) args;
    (void) kwds;

    SNVTableObject* const restrict self = (SNVTableObject*) type->tp_alloc(type, 0);

    self->table = vrd_snv_table_init();
    if (NULL == self->table)
    {
        Py_TYPE(self)->tp_free((PyObject*) self);
        PyErr_SetString(PyExc_RuntimeError, "SNVTable: vrd_snv_table_init() failed");
        return NULL;
    } // if

    return (PyObject*) self;
} // SNVTable_new


static void
SNVTable_dealloc(SNVTableObject* const self)
{
    vrd_snv_table_destroy(&self->table);
    Py_TYPE(self)->tp_free((PyObject*) self);
} // SNVTable_dealloc


static PyObject*
SNVTable_insert(SNVTableObject* const restrict self,
                PyObject* const restrict args)
{
    char const* restrict reference = NULL;
    size_t len = 0;
    int position = 0;
    int sample_id = 0;
    char const* restrict inserted = NULL;
    size_t len_inserted = 0;
    int phase = 0;

    if (!PyArg_ParseTuple(args, "s#iis#|i:SNVTable.insert", &reference, &len, &position, &sample_id, &inserted, &len_inserted, &phase))
    {
        return NULL;
    } // if

    if (1 != len_inserted)
    {
        PyErr_SetString(PyExc_ValueError, "SNVTable.insert: expected one inserted nucleotide");
        return NULL;
    } // if

    if (-1 == vrd_snv_table_insert(self->table, len, reference, position, sample_id, phase, vrd_iupac_to_idx(inserted[0])))
    {
        PyErr_SetString(PyExc_RuntimeError, "SNVTable.insert: vrd_snv_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // SNVTable_insert


static PyObject*
SNVTable_query(CoverageTableObject* const restrict self,
               PyObject* const restrict args)
{
    char const* restrict reference = NULL;
    size_t len = 0;
    int position = 0;
    char const* restrict inserted = NULL;
    size_t len_inserted = 0;
    PyObject* restrict list = NULL;

    if (!PyArg_ParseTuple(args, "s#is#|O!:SNVTable.query", &reference, &len, &position, &inserted, &len_inserted, &PyList_Type, &list))
    {
        return NULL;
    } // if

    if (1 != len_inserted)
    {
        PyErr_SetString(PyExc_ValueError, "SNVTable.query: expected one inserted nucleotide");
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
    result = vrd_snv_table_query(self->table, len, reference, position, vrd_iupac_to_idx(inserted[0]), subset);
    vrd_avl_tree_destroy(&subset);
    Py_END_ALLOW_THREADS

    return Py_BuildValue("i", result);
} // SNVTable_query


static PyMethodDef SNVTable_methods[] =
{
    {"insert", (PyCFunction) SNVTable_insert, METH_VARARGS,
     "Insert a region in the :py:class:`SNVTable`\n\n"
     ":param str reference: The reference sequence ID\n"
     ":param int position: The start position of the SNV.\n"
     ":param int sample_id: The sample ID\n"
     ":param str type: The inserted base from IUPAC\n"
     ":param phase: The phase group (position based)\n"
     ":type phase: integer, optional\n"},

    {"query", (PyCFunction) SNVTable_query, METH_VARARGS,
     "Query for SNVs in the :py:class:`SNVTable`\n\n"
     ":param str reference: The reference sequence ID\n"
     ":param int position: The position of the SNV\n"
     ":param str inserted: The inserted base from IUPAC\n"
     ":param subset: A list of sample IDs, defaults to None\n"
     ":type subset: list, optional\n"
     ":return: The number of contained SNVs\n"
     ":rtype: integer\n"},

    {NULL}  // sentinel
}; // SNVTable_methods


static PyTypeObject SNVTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.SNVTable",
    .tp_doc = "This class is a conceptual representation of a database"
              "table containing single nucleotide variants (SNV).",
    .tp_basicsize = sizeof(SNVTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = SNVTable_new,
    .tp_dealloc = (destructor) SNVTable_dealloc,
    .tp_methods = SNVTable_methods
}; // SNVTable
