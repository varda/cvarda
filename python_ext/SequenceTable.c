#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t

#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
#include "helpers.h"    // CFG_*


typedef struct
{
    PyObject_HEAD
    vrd_Seq_Table* table;
} SequenceTableObject;


static PyObject*
SequenceTable_new(PyTypeObject* const restrict type,
                  PyObject* const restrict args,
                  PyObject* const restrict kwds)
{
    (void) kwds;

    size_t ref_capacity = CFG_REF_CAPACITY;
    size_t ref_size_capacity = CFG_REF_SIZE_CAPACITY;

    if (!PyArg_ParseTuple(args, "|nn:SequenceTable", &ref_capacity, &ref_size_capacity))
    {
        return NULL;
    } // if

    SequenceTableObject* const restrict self = (SequenceTableObject*) type->tp_alloc(type, 0);

    self->table = vrd_seq_table_init(ref_capacity, ref_size_capacity);
    if (NULL == self->table)
    {
        Py_TYPE(self)->tp_free((PyObject*) self);
        PyErr_SetString(PyExc_RuntimeError, "SequenceTable: vrd_seq_table_init() failed");
        return NULL;
    } // if

    return (PyObject*) self;
} // SequenceTable_new


static void
SequenceTable_dealloc(SequenceTableObject* const self)
{
    vrd_seq_table_destroy(&self->table);
    Py_TYPE(self)->tp_free((PyObject*) self);
} // SequenceTable_dealloc


static PyObject*
SequenceTable_insert(SequenceTableObject* const restrict self,
                     PyObject* const restrict args)
{
    char const* restrict sequence = NULL;
    size_t len = 0;

    if (!PyArg_ParseTuple(args, "s#:SequenceTable.insert", &sequence, &len))
    {
        return NULL;
    } // if

    void* const restrict result = vrd_seq_table_insert(self->table, len, sequence);
    if (NULL == result)
    {
        PyErr_SetString(PyExc_RuntimeError, "SequenceTable.insert: vrd_seq_table_insert() failed");
        return NULL;
    } // if

    return PyCapsule_New(result, "sequence", NULL);
} // SequenceTable_insert


static PyMethodDef SequenceTable_methods[] =
{
    {"insert", (PyCFunction) SequenceTable_insert, METH_VARARGS,
     "insert(sequence)\n"
     "Insert a region in the :py:class:`SequenceTable`\n\n"
     ":param string sequence: The (inserted) sequence.\n"},

    {NULL}  // sentinel
}; // SequenceTable_methods


static PyTypeObject SequenceTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.SequenceTable",
    .tp_doc = "SequenceTable([ref_capacity[, ref_size_capacity]])\n"
              "This class is a conceptual representation of a database"
              "table containing (inserted) sequences.\n\n"
              ":param ref_capacity:  defaults to :c:data:`CFG_REF_CAPACITY`\n"
              ":type ref_capacity: integer, optional\n"
              ":param ref_size_capacity:  defaults to :c:data:`CFG_REF_SIZE_CAPACITY`\n"
              ":type ref_size_capacity: integer, optional\n",
    .tp_basicsize = sizeof(SequenceTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = SequenceTable_new,
    .tp_dealloc = (destructor) SequenceTable_dealloc,
    .tp_methods = SequenceTable_methods
}; // SequenceTable
