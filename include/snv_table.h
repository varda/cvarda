#ifndef VRD_SNV_TABLE_H
#define VRD_SNV_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdio.h>      // FILE

#include "avl_tree.h"   // vrd_AVL_Tree
#include "template.h"   // VRD_TEMPLATE


#define VRD_TYPENAME SNV


#include "../src/template_table.h"  // vrd_SNV_table_*


int
VRD_TEMPLATE(VRD_TYPENAME, _table_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                          size_t const len,
                                          char const reference[len],
                                          size_t const position,
                                          size_t const allele_count,
                                          size_t const sample_id,
                                          size_t const phase,
                                          size_t const inserted);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                         size_t const len,
                                         char const reference[len],
                                         size_t const position,
                                         size_t const inserted,
                                         vrd_AVL_Tree const* const subset);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query_region)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                                size_t const len_ref,
                                                char const reference[len_ref],
                                                size_t const start,
                                                size_t const end,
                                                vrd_AVL_Tree const* const subset,
                                                size_t const len_res,
                                                void* result[len_res]);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_export)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                          FILE* stream);


#undef VRD_TYPENAME


#ifdef __cplusplus
} // extern "C"
#endif

#endif
