typedef struct
{
    PyObject_HEAD
    vrd_Cov_Table* table;
} CoverageTableObject;


static PyObject*
CoverageTable_new(PyTypeObject* const restrict type,
                  PyObject* const restrict args,
                  PyObject* const restrict kwds)
{
    (void) args;
    (void) kwds;

    CoverageTableObject* const restrict self = (CoverageTableObject*) type->tp_alloc(type, 0);

    self->table = vrd_cov_table_init();
    if (NULL == self->table)
    {
        Py_TYPE(self)->tp_free((PyObject*) self);
        PyErr_SetString(PyExc_RuntimeError, "CoverageTable(): vrd_cov_table_init() failed");
        return NULL;
    } // if

    return (PyObject*) self;
} // CoverageTable_new


static void
CoverageTable_dealloc(CoverageTableObject* const self)
{
    vrd_cov_table_destroy(&self->table);
    Py_TYPE(self)->tp_free((PyObject*) self);
} // CoverageTable_dealloc


static PyObject*
CoverageTable_insert(CoverageTableObject* const restrict self,
                     PyObject* const restrict args)
{
    char const* restrict reference = NULL;
    size_t len = 0;
    uint32_t start = 0;
    uint32_t end = 0;
    uint32_t sample_id = 0;

    if (!PyArg_ParseTuple(args, "s#III:CoverageTable.insert", &reference, &len, &start, &end, &sample_id))
    {
        return NULL;
    } // if

    if (-1 == vrd_cov_table_insert(self->table, len, reference, start, end, sample_id))
    {
        PyErr_SetString(PyExc_RuntimeError, "CoverageTable.insert(): vrd_cov_table_insert() failed");
        return NULL;
    } // if

    Py_RETURN_NONE;
} // CoverageTable_insert


static PyMethodDef CoverageTable_methods[] =
{
    {"insert", (PyCFunction) CoverageTable_insert, METH_VARARGS,
     "Insert a region in the :py:class:`CoverageTable`\n\n"
     ":param str reference: The reference sequence ID\n"
     ":param int start: The start position of the region (included)\n"
     ":param int end: The end position of the region (excluded)\n"
     ":param int sample_id: The sample ID\n"},

    {NULL}  // sentinel
}; // RegionTable_methods


static PyTypeObject CoverageTable =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "cvarda.CoverageTable",
    .tp_doc = "This class is a conceptual representation of a database"
              "table containing covered regions on a reference sequence.",
    .tp_basicsize = sizeof(CoverageTableObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = CoverageTable_new,
    .tp_dealloc = (destructor) CoverageTable_dealloc,
    .tp_methods = CoverageTable_methods
}; // CoverageTable
