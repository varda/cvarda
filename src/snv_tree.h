#ifndef VRD_SNV_TREE_H
#define VRD_SNV_TREE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdio.h>      // FILE

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/template.h"    // VRD_TEMPLATE


#define VRD_TYPENAME SNV


typedef struct VRD_TEMPLATE(VRD_TYPENAME, _Tree) VRD_TEMPLATE(VRD_TYPENAME, _Tree);


void
VRD_TEMPLATE(VRD_TYPENAME, _unpack)(void* const ptr,
                                    size_t* const position,
                                    size_t* const count,
                                    size_t* const sample_id,
                                    size_t* const phase,
                                    char* const inserted);


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                         size_t const position,
                                         size_t const count,
                                         size_t const sample_id,
                                         size_t const phase,
                                         size_t const inserted);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_query)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                        size_t const position,
                                        size_t const inserted,
                                        int const homozygous,
                                        vrd_AVL_Tree const* const subset);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_query_region)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                               size_t const start,
                                               size_t const end,
                                               vrd_AVL_Tree const* const subset,
                                               size_t const len,
                                               void* result[len]);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_export)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                         FILE* stream,
                                         size_t const len,
                                         char const reference[len]);


#include "template_tree.h"  // vrd_SNV_tree_*


#undef VRD_TYPENAME


#ifdef __cplusplus
} // extern "C"
#endif

#endif
