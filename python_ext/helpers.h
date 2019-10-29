#ifndef HELPERS_H
#define HELPERS_H


#ifdef __cplusplus
#define restrict

extern "C"
{
#endif


#define PY_SSIZE_T_CLEAN
#include <Python.h>     // PyObject

#include "../include/avl_tree.h"   // vrd_AVL_Tree


enum
{
    CFG_REF_CAPACITY = 1000,
    CFG_SEQ_CAPACITY = 1000000,
    CFG_TREE_CAPACITY = 1 << 24
}; // constants


vrd_AVL_Tree*
sample_set(PyObject* const list);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
