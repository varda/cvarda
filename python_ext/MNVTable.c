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
    (void) args;
    (void) kwds;

    MNVTableObject* const restrict self = (MNVTableObject*) type->tp_alloc(type, 0);

    self->table = vrd_mnv_table_init();
    if (NULL == self->table)
    {
        Py_TYPE(self)->tp_free((PyObject*) self);
        PyErr_SetString(PyExc_RuntimeError, "MNVTable(): vrd_mnv_table_init() failed");
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
    uint32_t start = 0;
    uint32_t end = 0;
    uint32_t sample_id = 0;
    char const* restrict inserted = NULL;
    size_t len_inserted = 0;
    uint32_t phase = 0;

    if (!PyArg_ParseTuple(args, "s#III|s#I:MNVTable.insert", &reference, &len, &start, &end, &sample_id, &inserted, &len_inserted, &phase))
    {
        return NULL;
    } // if

    if (-1 == vrd_mnv_table_insert(self->table, len, reference, start, end, sample_id, phase, NULL))
    {
        PyErr_SetString(PyExc_RuntimeError, "MNVTable.insert(): vrd_mnv_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // MNVTable_insert


static PyMethodDef MNVTable_methods[] =
{
    {"insert", (PyCFunction) MNVTable_insert, METH_VARARGS,
     "Insert a region in the :py:class:`MNVTable`\n\n"
     ":param str reference: The reference sequence ID\n"
     ":param int start: The start position of the deleted part of the MNV\n"
     ":param int end: The end position of the deleted part of the MNV\n"
     ":param int sample_id: The sample ID\n"
     ":param str type: The inserted sequence\n"
     ":param phase: The phase group (position based)\n"
     ":type phase: integer, optional\n"},

    {NULL}  // sentinel
}; // MNVTable_methods


static PyTypeObject MNVTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.MNVTable",
    .tp_doc = "This class is a conceptual representation of a database"
              "table containing multi nucleotide variants (MNV).",
    .tp_basicsize = sizeof(MNVTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = MNVTable_new,
    .tp_dealloc = (destructor) MNVTable_dealloc,
    .tp_methods = MNVTable_methods
}; // MNVTable
