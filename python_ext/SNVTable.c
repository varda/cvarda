#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*, METH_VARARGS, destructor

#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // uint32_t

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
        PyErr_SetString(PyExc_RuntimeError, "SNVTable(): vrd_snv_table_init() failed");
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
    uint32_t position = 0;
    uint32_t sample_id = 0;
    char const* restrict inserted = NULL;
    size_t len_inserted = 0;
    uint32_t phase = 0;

    if (!PyArg_ParseTuple(args, "s#IIs#|I:SNVTable.insert", &reference, &len, &position, &sample_id, &inserted, &len_inserted, &phase))
    {
        return NULL;
    } // if

    uint32_t type = 0;

    if (-1 == vrd_snv_table_insert(self->table, len, reference, position, sample_id, phase, type))
    {
        PyErr_SetString(PyExc_RuntimeError, "SNVTable.insert(): vrd_snv_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // SNVTable_insert


static PyMethodDef SNVTable_methods[] =
{
    {"insert", (PyCFunction) SNVTable_insert, METH_VARARGS,
     "Insert a region in the :py:class:`SNVTable`\n\n"
     ":param str reference: The reference sequence ID\n"
     ":param int position: The start position of the SNV.\n"
     ":param int sample_id: The sample ID\n"
     ":param str type: The inserted base: 'A', 'C', 'G', or 'T'\n"
     ":param phase: The phase group (position based)\n"
     ":type phase: integer, optional\n"},

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
