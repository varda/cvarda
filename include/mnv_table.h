#ifndef VRD_MNV_TABLE_H
#define VRD_MNV_TABLE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "avl_tree.h"   // vrd_AVL_Tree
#include "seq_table.h"  // vrd_Seq_Table
#include "template.h"   // VRD_TEMPLATE


#define VRD_TYPENAME MNV


#include "../src/template_table.h"  // vrd_MNV_table_*


int
VRD_TEMPLATE(VRD_TYPENAME, _table_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const restrict self,
                                          size_t const len,
                                          char const reference[len],
                                          size_t const start,
                                          size_t const end,
                                          size_t const sample_id,
                                          size_t const phase,
                                          size_t const inserted);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const restrict self,
                                              size_t const len,
                                              char const reference[len],
                                              size_t const start,
                                              size_t const end,
                                              size_t const inserted,
                                              vrd_AVL_Tree const* const restrict subset);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_remove_seq)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const restrict self,
                                              vrd_AVL_Tree const* const restrict subset,
                                              vrd_Seq_Table* const restrict seq_table);


#undef VRD_TYPENAME


#ifdef __cplusplus
} // extern "C"
#endif

#endif
