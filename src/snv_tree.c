#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // int32_t, uint32_t
#include <stdio.h>      // FILE, fprintf

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/constants.h"   // VRD_HOMOZYGOUS
#include "../include/iupac.h"       // vrd_idx_to_iupac
#include "../include/template.h"    // VRD_TEMPLATE
#include "snv_tree.h"   // vrd_SNV_Tree, vrd_SNV_tree_*
#include "tree.h"       // NULLPTR, LEFT, RIGHT


#define VRD_TYPENAME SNV


struct VRD_TEMPLATE(VRD_TYPENAME, _Node)
{
    uint32_t child[2];

    uint32_t key       : 28;    // position
    uint32_t count     :  4;

    int32_t  balance   :  3;    // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t sample_id : 29;

    uint32_t phase     : 28;
    uint32_t inserted  :  4;    // [0, ..., 15]
}; // vrd_SNV_Node


#include "template_tree.inc"    // vrd_SNV_tree_*


void
VRD_TEMPLATE(VRD_TYPENAME, _unpack)(void* const ptr,
                                    size_t* const position,
                                    size_t* const count,
                                    size_t* const sample_id,
                                    size_t* const phase,
                                    char* const inserted)
{
    struct vrd_SNV_Node const* const node = ptr;
    *position = node->key;
    *count = node->count;
    *sample_id = node->sample_id;
    *phase = node->phase == VRD_HOMOZYGOUS ? -1 : node->phase;
    *inserted = vrd_idx_to_iupac(node->inserted);
} // vrd_SNV_unpack


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                         size_t const position,
                                         size_t const count,
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
    self->nodes[ptr].count = count;
    self->nodes[ptr].balance = 0;
    self->nodes[ptr].sample_id = sample_id;
    self->nodes[ptr].phase = phase;
    self->nodes[ptr].inserted = inserted;

    insert(self, ptr);

    return 0;
} // vrd_SNV_tree_insert


static size_t
query(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
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
        return query(self, self->nodes[root].child[LEFT], position, inserted, subset);
    } // if

    if (self->nodes[root].key < position)
    {
        return query(self, self->nodes[root].child[RIGHT], position, inserted, subset);
    } // if

    size_t res = 0;
    // TODO: IUPAC match on inserted
    if (inserted == self->nodes[root].inserted &&
        (NULL == subset || vrd_AVL_tree_is_element(subset, self->nodes[root].sample_id)))
    {
        res = self->nodes[root].count;
    } // if

    return res + query(self, self->nodes[root].child[LEFT], position, inserted, subset) +
                 query(self, self->nodes[root].child[RIGHT], position, inserted, subset);
} // query


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_query)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                        size_t const position,
                                        size_t const inserted,
                                        vrd_AVL_Tree const* const subset)
{
    assert(NULL != self);

    return query(self, self->root, position, inserted, subset);
} // vrd_SNV_tree_query


static size_t
query_region(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
             size_t const root,
             size_t const start,
             size_t const end,
             vrd_AVL_Tree const* const subset,
             size_t const next,
             size_t const len,
             void* result[len])
{
    if (NULLPTR == root || next >= len)
    {
        return next;
    } // if

    if (self->nodes[root].key > end)
    {
        return query_region(self, self->nodes[root].child[LEFT], start, end, subset, next, len, result);
    } // if

    if (self->nodes[root].key < start)
    {
        return query_region(self, self->nodes[root].child[RIGHT], start, end, subset, next, len, result);
    } // if

    size_t match = 0;
    if (self->nodes[root].key >= start && self->nodes[root].key < end &&
        (NULL == subset || vrd_AVL_tree_is_element(subset, self->nodes[root].sample_id)))
    {
        result[next] = (void*) &self->nodes[root];
        match = 1;
    } // if

    size_t const count = query_region(self, self->nodes[root].child[LEFT], start, end, subset, next + match, len, result);
    return query_region(self, self->nodes[root].child[RIGHT], start, end, subset, count, len, result);
} // query_region


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_query_region)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                               size_t const start,
                                               size_t const end,
                                               vrd_AVL_Tree const* const subset,
                                               size_t const len,
                                               void* result[len])
{
    assert(NULL != self);

    return query_region(self, self->root, start, end, subset, 0, len, result);
} // vrd_SNV_tree_query_region


static size_t
export(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
       uint32_t const root,
       FILE* stream,
       size_t const len,
       char const reference[len])
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    size_t count = export(self, self->nodes[root].child[LEFT], stream, len, reference);

    int const phase = self->nodes[root].phase == VRD_HOMOZYGOUS ? -1 : (int) self->nodes[root].phase;

    (void) fprintf(stream, "%s\t%u\t%u\t%u\t%d\t1\t%c\n", reference, self->nodes[root].key, self->nodes[root].key + 1, self->nodes[root].count, phase, vrd_idx_to_iupac(self->nodes[root].inserted));

    count += export(self, self->nodes[root].child[RIGHT], stream, len, reference);
    return count + 1;
} // export


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_export)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                         FILE* stream,
                                         size_t const len,
                                         char const reference[len])
{
    assert(NULL != self);
    assert(NULL != stream);

    return export(self, self->root, stream, len, reference);
} // vrd_SNV_export


#undef VRD_TYPENAME
