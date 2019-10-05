#ifndef HELPERS_H
#define HELPERS_H


#ifdef __cplusplus
#define restrict

extern "C"
{
#endif


#define PY_SSIZE_T_CLEAN
#include <Python.h>     // PyObject

#include "../include/varda.h"   // vrd_AVL_Tree


vrd_AVL_Tree*
sample_set(PyObject* const list);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
