#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t

#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_SNV_table_*
#include "../include/trie.h"        // vrd_Trie_Node, vrd_trie_*
#include "snv_tree.h"   // vrd_SNV_Tree, vrd_SNV_tree_*


#define VRD_TYPENAME SNV


#include "template_table.inc"   // vrd_SNV_table_*


int
VRD_TEMPLATE(VRD_TYPENAME, _table_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                          size_t const len,
                                          char const reference[len],
                                          size_t const position,
                                          size_t const sample_id,
                                          size_t const phase,
                                          size_t const inserted)
{
    assert(NULL != self);

    VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const tree = reference_tree(self, len, reference);
    if (NULL == tree)
    {
        return errno;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(tree, position, sample_id, phase, inserted);
} // vrd_SNV_table_insert


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                              size_t const len,
                                              char const reference[len],
                                              size_t const position,
                                              size_t const inserted,
                                              vrd_AVL_Tree const* const subset)
{
    assert(NULL != self);

    vrd_Trie_Node* const elem = vrd_trie_find(self->trie, len, reference);
    if (NULL == elem)
    {
        return 0;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_query_stab)(*(VRD_TEMPLATE(VRD_TYPENAME, _Tree)**) elem, position, inserted, subset);
} // vrd_SNV_table_query_stab


#undef VRD_TYPENAME
