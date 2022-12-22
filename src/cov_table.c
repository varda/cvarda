#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t

#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_Cov_table_*
#include "../include/trie.h"        // vrd_Trie_Node, vrd_trie_*
#include "cov_tree.h"   // vrd_Cov_Tree, vrd_Cov_tree_*


#define VRD_TYPENAME Cov


#include "template_table.inc"   // tree_from_reference, vrd_Cov_table_*


int
VRD_TEMPLATE(VRD_TYPENAME, _table_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                          size_t const len,
                                          char const reference[len],
                                          size_t const start,
                                          size_t const end,
                                          size_t const allele_count,
                                          size_t const sample_id)
{
    assert(NULL != self);

    VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const tree = tree_from_reference(self, len, reference);
    if (NULL == tree)
    {
        return errno;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(tree, start, end, allele_count, sample_id);
} // vrd_Cov_table_insert


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                              size_t const len,
                                              char const reference[len],
                                              size_t const start,
                                              size_t const end,
                                              vrd_AVL_Tree const* const subset)
{
    assert(NULL != self);

    vrd_Trie_Node* const elem = vrd_trie_find(self->trie, len, reference);
    if (NULL == elem)
    {
        return -1;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_query_stab)(elem->data, start, end, subset);
} // vrd_Cov_table_query_stab


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query_region)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                                size_t const len_ref,
                                                char const reference[len_ref],
                                                size_t const start,
                                                size_t const end,
                                                vrd_AVL_Tree const* const subset,
                                                size_t const len_res,
                                                void* result[len_res])
{
    assert(NULL != self);

    vrd_Trie_Node* const elem = vrd_trie_find(self->trie, len_ref, reference);
    if (NULL == elem)
    {
        return -1;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_query_region)(elem->data, start, end, subset, len_res, result);
} // vrd_Cov_table_query_region


#undef VRD_TYPENAME
