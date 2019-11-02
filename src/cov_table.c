#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t

#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_Cov_table_*
#include "cov_tree.h"   // vrd_Cov_Tree, vrd_Cov_tree_*


#define VRD_TYPENAME Cov


#include "template_table.inc"   // vrd_Cov_table_*


int
VRD_TEMPLATE(VRD_TYPENAME, _table_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const restrict self,
                                          size_t const len,
                                          char const reference[len],
                                          size_t const start,
                                          size_t const end,
                                          size_t const sample_id)
{
    assert(NULL != self);

    VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const restrict tree = reference_tree(self, len, reference);
    if (NULL == tree)
    {
        return errno;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(tree, start, end, sample_id);
} // vrd_Cov_table_insert


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const restrict self,
                                              size_t const len,
                                              char const reference[len],
                                              size_t const start,
                                              size_t const end,
                                              vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != self);

    void* const restrict elem = vrd_trie_find(self->trie, len, reference);
    if (NULL == elem)
    {
        return 0;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_query_stab)(*(VRD_TEMPLATE(VRD_TYPENAME, _Tree)**) elem, start, end, subset);
} // vrd_Cov_table_query_stab


#undef VRD_TYPENAME
