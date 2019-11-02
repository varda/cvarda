#ifndef VRD_HELPERS_H
#define VRD_HELPERS_H


#define PY_SSIZE_T_CLEAN
#include <Python.h>     // PyObject

#include "../include/avl_tree.h"   // vrd_AVL_Tree


enum
{
    CFG_REF_CAPACITY = 1000,
    CFG_SEQ_CAPACITY = 100000,
    CFG_TREE_CAPACITY = 1 << 24
}; // constants


vrd_AVL_Tree*
sample_set(PyObject* const list);


#endif
