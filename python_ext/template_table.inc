#include "../include/template.h"    // VRD_TEMPLATE
#include "template.h"   // VRD_PY_TEMPLATE, VRD_PY_STRINGIZE
#ifndef VRD_TYPENAME
#error "Undefined template typename"
#endif
#ifndef VRD_OBJNAME
#error "Undefined template Python object name"
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*

#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t


static PyObject*
VRD_PY_TEMPLATE(VRD_OBJNAME, _new)(PyTypeObject* const restrict type,
                                   PyObject* const restrict args,
                                   PyObject* const restrict kwds)
{
    (void) kwds;

    size_t ref_capacity = CFG_REF_CAPACITY;
    size_t tree_capacity = CFG_TREE_CAPACITY;

    if (!PyArg_ParseTuple(args, "|nn:" VRD_PY_STRINGIZE(VRD_OBJNAME), &ref_capacity, &tree_capacity))
    {
        return NULL;
    } // if

    VRD_PY_TEMPLATE(VRD_OBJNAME, Object)* const restrict self = (VRD_PY_TEMPLATE(VRD_OBJNAME, Object)*) type->tp_alloc(type, 0);

    errno = 0;
    self->table = VRD_TEMPLATE(VRD_TYPENAME, _table_init)(ref_capacity, tree_capacity);
    if (NULL == self->table)
    {
        if (errno < 0)
        {
            PyErr_SetString(PyExc_RuntimeError, VRD_PY_STRINGIZE(VRD_OBJNAME) " initialization failed");
        } // if
        else
        {
            PyErr_SetFromErrno(PyExc_OSError);
        } // else
        Py_TYPE(self)->tp_free((PyObject*) self);
        return NULL;
    } // if

    return (PyObject*) self;
} // *_new


static void
VRD_PY_TEMPLATE(VRD_OBJNAME, _dealloc)(VRD_PY_TEMPLATE(VRD_OBJNAME, Object)* const self)
{
    VRD_TEMPLATE(VRD_TYPENAME, _table_destroy)(&self->table);
    Py_TYPE(self)->tp_free((PyObject*) self);
} // *_dealloc


static PyObject*
VRD_PY_TEMPLATE(VRD_OBJNAME, _reorder)(VRD_PY_TEMPLATE(VRD_OBJNAME, Object)* const restrict self,
                                       PyObject* const restrict args)
{
    (void) args;

    int const err = VRD_TEMPLATE(VRD_TYPENAME, _table_reorder)(self->table);
    if (0 != err)
    {
        if (err < 0)
        {
            PyErr_SetString(PyExc_RuntimeError, VRD_PY_STRINGIZE(VRD_OBJNAME) ".reorder failed");
        } // if
        else
        {
            PyErr_SetFromErrno(PyExc_OSError);
        } // else
        return NULL;
    } // if

    Py_RETURN_NONE;
} // *_reorder


static PyObject*
VRD_PY_TEMPLATE(VRD_OBJNAME, _read)(VRD_PY_TEMPLATE(VRD_OBJNAME, Object)* const restrict self,
                                    PyObject* const restrict args)
{
    char const* restrict path = NULL;

    if (!PyArg_ParseTuple(args, "s:" VRD_PY_STRINGIZE(VRD_OBJNAME) ".read", &path))
    {
        return NULL;
    } // if

    int const err = VRD_TEMPLATE(VRD_TYPENAME, _table_read)(self->table, path);
    if (0 != err)
    {
        if (err < 0)
        {
            PyErr_SetString(PyExc_RuntimeError, VRD_PY_STRINGIZE(VRD_OBJNAME) ".read failed");
        } // if
        else
        {
            PyErr_SetFromErrno(PyExc_OSError);
        } // else
        return NULL;
    } // if

    Py_RETURN_NONE;
} // *_read


static PyObject*
VRD_PY_TEMPLATE(VRD_OBJNAME, _write)(VRD_PY_TEMPLATE(VRD_OBJNAME, Object)* const restrict self,
                                     PyObject* const restrict args)
{
    char const* restrict path = NULL;

    if (!PyArg_ParseTuple(args, "s:" VRD_PY_STRINGIZE(VRD_OBJNAME) ".write", &path))
    {
        return NULL;
    } // if

    int const err = VRD_TEMPLATE(VRD_TYPENAME, _table_write)(self->table, path);
    if (0 != err)
    {
        if (err < 0)
        {
            PyErr_SetString(PyExc_RuntimeError, VRD_PY_STRINGIZE(VRD_OBJNAME) ".write failed");
        } // if
        else
        {
            PyErr_SetFromErrno(PyExc_OSError);
        } // else
        return NULL;
    } // if

    Py_RETURN_NONE;
} // *_write
