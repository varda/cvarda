#ifndef VRD_SNV_TABLE_H
#define VRD_SNV_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t

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


#undef VRD_TYPENAME


#ifdef __cplusplus
} // extern "C"
#endif

#endif
