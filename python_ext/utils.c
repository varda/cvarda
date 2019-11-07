#define PY_SSIZE_T_CLEAN
#include <Python.h>     // Py*

#include <stddef.h>     // NULL, size_t

#include "../include/avl_tree.h"   // vrd_AVL_Tree, vrd_AVL_tree_*
#include "utils.h"    // sample_set


vrd_AVL_Tree*
sample_set(PyObject* const list)
{
    size_t const n = PyList_Size(list);
    vrd_AVL_Tree* tree = vrd_AVL_tree_init(n);
    if (NULL == tree)
    {
        PyErr_SetNone(PyExc_MemoryError);
        return NULL;
    } // if

    for (size_t i = 0; i < n; ++i)
    {
        int overflow = 0;
        long const sample_id = PyLong_AsLongAndOverflow(PyList_GetItem(list, i), &overflow);
        if (NULL != PyErr_Occurred())
        {
            vrd_AVL_tree_destroy(&tree);
            return NULL;
        } // if

        if (0 != overflow)
        {
            vrd_AVL_tree_destroy(&tree);
            PyErr_SetString(PyExc_ValueError, "sample_set(): integer overflow");
            return NULL;
        } // if

        if (0 != vrd_AVL_tree_insert(tree, sample_id))
        {
            vrd_AVL_tree_destroy(&tree);
            PyErr_SetString(PyExc_RuntimeError, "sample_set(): vrd_avl_tree_insert() failed");
            return NULL;
        } // if
    } // for

    return tree;
} // sample_set
