#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t

#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_MNV_table_*
#include "mnv_tree.h"   // vrd_MNV_Tree, vrd_MNV_tree_*


#define VRD_TYPENAME MNV


#include "template_table.inc"   // vrd_MNV_table_*


int
VRD_TEMPLATE(VRD_TYPENAME, _table_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const restrict self,
                                          size_t const len,
                                          char const reference[len],
                                          size_t const start,
                                          size_t const end,
                                          size_t const sample_id,
                                          size_t const phase,
                                          size_t const inserted)
{
    assert(NULL != self);

    VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const restrict tree = reference_tree(self, len, reference);
    if (NULL == tree)
    {
        return errno;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(tree, start, end, sample_id, phase, inserted);
} // vrd_MNV_table_insert


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const restrict self,
                                              size_t const len,
                                              char const reference[len],
                                              size_t const start,
                                              size_t const end,
                                              size_t const inserted,
                                              vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != self);

    void* const restrict elem = vrd_trie_find(self->trie, len, reference);
    if (NULL == elem)
    {
        return 0;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_query_stab)(*(VRD_TEMPLATE(VRD_TYPENAME, _Tree)**) elem, start, end, inserted, subset);
} // vrd_MNV_table_query_stab


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_remove_seq)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const restrict self,
                                              vrd_AVL_Tree const* const restrict subset,
                                              vrd_Seq_Table* const restrict seq_table)
{
    assert(NULL != self);

    size_t count = 0;
    for (size_t i = 0; i < self->next; ++i)
    {
        count += VRD_TEMPLATE(VRD_TYPENAME, _tree_remove_seq)(*(VRD_TEMPLATE(VRD_TYPENAME, _Tree)**) self->trees[i], subset, seq_table);  // OVERFLOW
    } // for

    return count;
} // vrd_MNV_table_remove_seq


#undef VRD_TYPENAME
