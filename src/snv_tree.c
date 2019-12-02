#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // int32_t, uint32_t

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/constants.h"   // VRD_HOMOZYGOUS
#include "../include/template.h"    // VRD_TEMPLATE
#include "snv_tree.h"   // vrd_SNV_Tree, vrd_SNV_tree_*


#define VRD_TYPENAME SNV


struct VRD_TEMPLATE(VRD_TYPENAME, _Node)
{
    uint32_t child[2];
    uint32_t key;  // position
    int32_t  balance   :  3;    // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t sample_id : 29;
    uint32_t phase     : 28;
    uint32_t inserted  :  4;    // [0, ..., 15]
}; // vrd_SNV_Node


#include "template_tree.inc"    // vrd_SNV_tree_*


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                         size_t const position,
                                         size_t const sample_id,
                                         size_t const phase,
                                         size_t const inserted)
{
    assert(NULL != self);

    if (UINT32_MAX == self->next || self->capacity < self->next)
    {
        return -1;
    } // if

    uint32_t const ptr = self->next;
    self->next += 1;

    self->nodes[ptr].child[LEFT] = NULLPTR;
    self->nodes[ptr].child[RIGHT] = NULLPTR;
    self->nodes[ptr].key = position;
    self->nodes[ptr].balance = 0;
    self->nodes[ptr].sample_id = sample_id;
    self->nodes[ptr].phase = phase;
    self->nodes[ptr].inserted = inserted;

    insert(self, ptr);

    return 0;
} // vrd_SNV_tree_insert


static size_t
query_stab(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
           size_t const root,
           size_t const position,
           size_t const inserted,
           vrd_AVL_Tree const* const subset)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    if (self->nodes[root].key > position)
    {
        return query_stab(self, self->nodes[root].child[LEFT], position, inserted, subset);
    } // if

    if (self->nodes[root].key < position)
    {
        return query_stab(self, self->nodes[root].child[RIGHT], position, inserted, subset);
    } // if

    size_t res = 0;
    // TODO: IUPAC match on inserted
    if (inserted == self->nodes[root].inserted &&
        (NULL == subset || vrd_AVL_tree_is_element(subset, self->nodes[root].sample_id)))
    {
        res = 1;
        if (VRD_HOMOZYGOUS == self->nodes[root].phase)
        {
            res = 2;
        } // if
    } // if

    return res + query_stab(self, self->nodes[root].child[LEFT], position, inserted, subset) +
                 query_stab(self, self->nodes[root].child[RIGHT], position, inserted, subset);
} // query_stab


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                             size_t const position,
                                             size_t const inserted,
                                             vrd_AVL_Tree const* const subset)
{
    assert(NULL != self);

    return query_stab(self, self->root, position, inserted, subset);
} // vrd_SNV_tree_query_stab


#undef VRD_TYPENAME
