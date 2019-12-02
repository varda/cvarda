#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // int32_t, uint32_t

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/template.h"    // VRD_TEMPLATE
#include "cov_tree.h"   // vrd_Cov_Tree, vrd_Cov_tree_*


#define VRD_TYPENAME Cov


struct VRD_TEMPLATE(VRD_TYPENAME, _Node)
{
    uint32_t child[2];
    uint32_t key;  // start
    uint32_t end;
    uint32_t max;
    int32_t  balance   :  3;  // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t sample_id : 29;
}; // vrd_Cov_Node


#define VRD_INTERVAL
#include "template_tree.inc"    // vrd_Cov_tree_*
#undef VRD_INTERVAL


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                         size_t const start,
                                         size_t const end,
                                         size_t const sample_id)
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
    self->nodes[ptr].key = start;
    self->nodes[ptr].end = end;
    self->nodes[ptr].max = end;
    self->nodes[ptr].balance = 0;
    self->nodes[ptr].sample_id = sample_id;

    insert(self, ptr);

    return 0;
} // vrd_Cov_tree_insert


static size_t
query_stab(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
           size_t const root,
           size_t const start,
           size_t const end,
           vrd_AVL_Tree const* const subset)
{
    if (NULLPTR == root || self->nodes[root].max < start)
    {
        return 0;
    } // if

    if (self->nodes[root].key > start)
    {
        return query_stab(self, self->nodes[root].child[LEFT], start, end, subset);
    } // if

    size_t res = 0;
    if (start >= self->nodes[root].key &&
        end <= self->nodes[root].end &&
        (NULL == subset || vrd_AVL_tree_is_element(subset, self->nodes[root].sample_id)))
    {
        res = 1;
    } // if

    return res + query_stab(self, self->nodes[root].child[LEFT], start, end, subset) +
                 query_stab(self, self->nodes[root].child[RIGHT], start, end, subset);
} // query_stab


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                             size_t const start,
                                             size_t const end,
                                             vrd_AVL_Tree const* const subset)
{
    assert(NULL != self);

    return query_stab(self, self->root, start, end, subset);
} // vrd_Cov_tree_query_stab


#undef VRD_TYPENAME
