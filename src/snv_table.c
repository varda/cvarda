#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t
#include <stdlib.h>     // free

#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_SNV_table_*
#include "../include/trie.h"        // vrd_Trie_Node, vrd_trie_*
#include "snv_tree.h"   // vrd_SNV_Tree, vrd_SNV_tree_*


#define VRD_TYPENAME SNV


#include "template_table.inc"   // reference_tree, vrd_SNV_table_*


int
VRD_TEMPLATE(VRD_TYPENAME, _table_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                          size_t const len,
                                          char const reference[len],
                                          size_t const position,
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

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(tree, position, allele_count, sample_id, phase, inserted);
} // vrd_SNV_table_insert


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
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

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_query)(elem->data, position, inserted, subset);
} // vrd_SNV_table_query


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
        return 0;
    } // if

    return VRD_TEMPLATE(VRD_TYPENAME, _tree_query_region)(elem->data, start, end, subset, len_res, result);
} // vrd_SNV_table_query_region


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_export)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                          FILE* stream)
{
    assert(NULL != self);

    size_t count = 0;
    for (size_t i = 0; i < self->next; ++i)
    {
        char* reference = NULL;
        size_t const len = vrd_trie_key(self->trees[i], &reference);

        count += VRD_TEMPLATE(VRD_TYPENAME, _tree_export)(self->trees[i]->data, stream, len, reference);  // OVERFLOW

        free(reference);
    } // for

    return count;
} // vrd_SNV_table_export


#undef VRD_TYPENAME
