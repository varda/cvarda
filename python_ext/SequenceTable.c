#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t

#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_Seq_table_*
#include "../include/trie.h"        // vrd_Trie_Node
#include "utils.h"          // CFG_*
#include "SequenceTable.h"  // SequenceTable*


static PyObject*
SequenceTable_new(PyTypeObject* const type,
                  PyObject* const args,
                  PyObject* const kwds)
{
    (void) kwds;

    size_t ref_capacity = CFG_SEQ_CAPACITY;

    if (!PyArg_ParseTuple(args, "|n:SequenceTable", &ref_capacity))
    {
        return NULL;
    } // if

    SequenceTableObject* const self = (SequenceTableObject*) type->tp_alloc(type, 0);

    self->table = vrd_Seq_table_init(ref_capacity);
    if (NULL == self->table)
    {
        Py_TYPE(self)->tp_free((PyObject*) self);
        PyErr_SetString(PyExc_RuntimeError, "SequenceTable: vrd_Seq_table_init() failed");
        return NULL;
    } // if

    return (PyObject*) self;
} // SequenceTable_new


static void
SequenceTable_dealloc(SequenceTableObject* const self)
{
    vrd_Seq_table_destroy(&self->table);
    Py_TYPE(self)->tp_free((PyObject*) self);
} // SequenceTable_dealloc


static PyObject*
SequenceTable_insert(SequenceTableObject* const self,
                     PyObject* const args)
{
    char const* sequence = NULL;
    size_t len = 0;

    if (!PyArg_ParseTuple(args, "s#:SequenceTable.insert", &sequence, &len))
    {
        return NULL;
    } // if

    vrd_Trie_Node* const result = vrd_Seq_table_insert(self->table, len + 1, sequence);
    if (NULL == result)
    {
        PyErr_SetString(PyExc_RuntimeError, "SequenceTable.insert: vrd_Seq_table_insert() failed");
        return NULL;
    } // if

    return Py_BuildValue("i", *(size_t*) result->data);
} // SequenceTable_insert


static PyObject*
SequenceTable_query(SequenceTableObject* const self,
                    PyObject* const args)
{
    char const* sequence = NULL;
    size_t len = 0;

    if (!PyArg_ParseTuple(args, "s#:SequenceTable.query", &sequence, &len))
    {
        return NULL;
    } // if

    vrd_Trie_Node* const result = vrd_Seq_table_query(self->table, len + 1, sequence);
    if (NULL == result)
    {
        Py_RETURN_NONE;
    } // if

    return Py_BuildValue("i", *(size_t*) result->data);
} // SequenceTable_query


static PyObject*
SequenceTable_remove(SequenceTableObject* const self,
                     PyObject* const args)
{
    int elem = 0;

    if (!PyArg_ParseTuple(args, "i:SequenceTable.remove", &elem))
    {
        return NULL;
    } // if

    if (0 != vrd_Seq_table_remove(self->table, elem))
    {
        PyErr_SetString(PyExc_RuntimeError, "SequenceTable.remove: vrd_Seq_table_remove() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // SequenceTable_remove


static PyObject*
SequenceTable_read(SequenceTableObject* const self,
                   PyObject* const args)
{
    char const* path = NULL;

    if (!PyArg_ParseTuple(args, "s:SequenceTable.read", &path))
    {
        return NULL;
    } // if

    if (0 != vrd_Seq_table_read(self->table, path))
    {
        PyErr_SetString(PyExc_RuntimeError, "SequenceTable.read: vrd_Seq_table_read() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // SequenceTable_read


static PyObject*
SequenceTable_write(SequenceTableObject* const self,
                    PyObject* const args)
{
    char const* path = NULL;

    if (!PyArg_ParseTuple(args, "s:SequenceTable.write", &path))
    {
        return NULL;
    } // if

    if (0 != vrd_Seq_table_write(self->table, path))
    {
        PyErr_SetString(PyExc_RuntimeError, "SequenceTable.write: vrd_Seq_table_write() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // SequenceTable_write


static PyMethodDef SequenceTable_methods[] =
{
    {"insert", (PyCFunction) SequenceTable_insert, METH_VARARGS,
     "insert(sequence)\n"
     "Insert a sequence in the :py:class:`SequenceTable`\n\n"
     ":param string sequence: The sequence.\n"
     ":return: A reference to the inserted sequence\n"
     ":rtype: integer\n"},

    {"query", (PyCFunction) SequenceTable_query, METH_VARARGS,
     "query(sequence)\n"
     "Query for a sequence in the :py:class:`SequenceTable`\n\n"
     ":param string sequence: The sequence.\n"
     ":return: A reference to the found sequence\n"
     ":rtype: integer\n"},

    {"remove", (PyCFunction) SequenceTable_remove, METH_VARARGS,
     "remove(index)\n"
     "Remove a sequence from the :py:class:`SequenceTable`\n\n"
     ":param integer index: The index in the table.\n"},

    {"read", (PyCFunction) SequenceTable_read, METH_VARARGS,
     "read(path)\n"
     "Read a :py:class:`SequenceTable` from files\n\n"
     ":param string path: A path including a prefix that identifies the files.\n"},

    {"write", (PyCFunction) SequenceTable_write, METH_VARARGS,
     "write(path)\n"
     "Write a :py:class:`SequenceTable` to files\n\n"
     ":param string path: A path including a prefix that identifies the files.\n"},

    {NULL, NULL, 0, NULL}  // sentinel
}; // SequenceTable_methods


PyTypeObject SequenceTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.SequenceTable",
    .tp_doc = "SequenceTable([ref_capacity[, ref_size_capacity]])\n"
              "This class is a conceptual representation of a database"
              "table containing (inserted) sequences.\n\n"
              ":param ref_capacity:  defaults to :c:data:`CFG_SEQ_CAPACITY`\n"
              ":type ref_capacity: integer, optional\n",
    .tp_basicsize = sizeof(SequenceTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = SequenceTable_new,
    .tp_dealloc = (destructor) SequenceTable_dealloc,
    .tp_methods = SequenceTable_methods
}; // SequenceTable
