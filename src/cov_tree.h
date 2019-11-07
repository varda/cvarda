#ifndef VRD_COV_TREE_H
#define VRD_COV_TREE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/template.h"    // VRD_TEMPLATE


#define VRD_TYPENAME Cov


typedef struct VRD_TEMPLATE(VRD_TYPENAME, _Tree) VRD_TEMPLATE(VRD_TYPENAME, _Tree);


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                         size_t const start,
                                         size_t const end,
                                         size_t const sample_id);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                             size_t const start,
                                             size_t const end,
                                             vrd_AVL_Tree const* const subset);


#include "template_tree.h"  // vrd_Cov_tree_*


#undef VRD_TYPENAME


#ifdef __cplusplus
} // extern "C"
#endif

#endif
