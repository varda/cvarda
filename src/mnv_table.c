#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t

#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_MNV_table_*
#include "../include/trie.h"        // vrd_Trie_Node, vrd_trie_*
#include "mnv_tree.h"   // vrd_MNV_Tree, vrd_MNV_tree_*


#define VRD_TYPENAME MNV


#include "template_table.inc"   // reference_tree, vrd_MNV_table_*


int
VRD_TEMPLATE(VRD_TYPENAME, _table_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                          size_t const len,
                                          char const reference[len],
                                          size_t const start,
                                          size_t const end,
                                          size_t const allele_count,
                                          size_t const sample_id,
                                          size_t const phase,
                                          size_t const inserted)
{
    assert(NULL != self);

    VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const tree = tree_from_reference(self, len, reference);
    if (NULL == tree)
    {
        return errno;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(tree, start, end, allele_count, sample_id, phase, inserted);
} // vrd_MNV_table_insert


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                              size_t const len,
                                              char const reference[len],
                                              size_t const start,
                                              size_t const end,
                                              size_t const inserted,
                                              vrd_AVL_Tree const* const subset)
{
    assert(NULL != self);

    vrd_Trie_Node* const elem = vrd_trie_find(self->trie, len, reference);
    if (NULL == elem)
    {
        return 0;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_query_stab)(elem->data, start, end, inserted, subset);
} // vrd_MNV_table_query_stab


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_remove_seq)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                              vrd_AVL_Tree const* const subset,
                                              vrd_Seq_Table* const seq_table)
{
    assert(NULL != self);

    size_t count = 0;
    for (size_t i = 0; i < self->next; ++i)
    {
        count += VRD_TEMPLATE(VRD_TYPENAME, _tree_remove_seq)(self->trees[i]->data, subset, seq_table);  // OVERFLOW
    } // for

    return count;
} // vrd_MNV_table_remove_seq


#undef VRD_TYPENAME
