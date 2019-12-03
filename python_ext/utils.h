#ifndef VRD_EXT_UTILS_H
#define VRD_EXT_UTILS_H


#define PY_SSIZE_T_CLEAN
#include <Python.h>     // PyObject

#include <stddef.h>     // size_t

#include "../include/avl_tree.h"    // vrd_AVL_Tree


static size_t const CFG_REF_CAPACITY = 1000;
static size_t const CFG_SEQ_CAPACITY = 100000;
static size_t const CFG_TREE_CAPACITY = 1 << 24;


vrd_AVL_Tree*
sample_set(PyObject* const list);


#endif
