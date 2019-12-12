#ifndef VRD_MNV_TREE_H
#define VRD_MNV_TREE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/seq_table.h"   // vrd_Seq_Table
#include "../include/template.h"    // VRD_TEMPLATE


#define VRD_TYPENAME MNV


typedef struct VRD_TEMPLATE(VRD_TYPENAME, _Tree) VRD_TEMPLATE(VRD_TYPENAME, _Tree);


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                         size_t const start,
                                         size_t const end,
                                         size_t const count,
                                         size_t const sample_id,
                                         size_t const phase,
                                         size_t const inserted);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_query)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                        size_t const start,
                                        size_t const end,
                                        size_t const inserted,
                                        vrd_AVL_Tree const* const subset);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_remove_seq)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                             vrd_AVL_Tree const* const subset,
                                             vrd_Seq_Table* const seq_table);


#include "template_tree.h"  // vrd_MNV_tree_*


#undef VRD_TYPENAME


#ifdef __cplusplus
} // extern "C"
#endif

#endif
