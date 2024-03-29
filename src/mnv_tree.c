#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdbool.h>    // bool
#include <stdint.h>     // int32_t, uint32_t
#include <stdio.h>      // FILE, fprintf

#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_AVL_tree_*
#include "../include/constants.h"   // VRD_HOMOZYGOUS
#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_Seq_table_*
#include "../include/template.h"    // VRD_TEMPLATE
#include "mnv_tree.h"   // vrd_MNV_Tree, vrd_MNV_tree_*
#include "tree.h"       // NULLPTR, LEFT, RIGHT


#define VRD_TYPENAME MNV


struct VRD_TEMPLATE(VRD_TYPENAME, _Node)
{
    uint32_t child[2];

    uint32_t key       : 28;    // start
    uint32_t count     :  4;

    uint32_t end;
    uint32_t max;

    int32_t  balance   :  3;    // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t sample_id : 29;

    uint32_t phase     : 28;
    uint32_t unused    :  4;    // For consistency with SNVs, we don't need the remaining bits (yet)

    uint32_t inserted;
}; // vrd_MNV_Node


#define VRD_INTERVAL
#include "template_tree.inc"    // vrd_MNV_tree_*
#undef VRD_INTERVAL


void
VRD_TEMPLATE(VRD_TYPENAME, _unpack)(void* const ptr,
                                    size_t* const start,
                                    size_t* const end,
                                    size_t* const count,
                                    size_t* const sample_id,
                                    size_t* const phase,
                                    size_t* const inserted)
{
    struct vrd_MNV_Node const* const node = ptr;
    *start = node->key;
    *end = node->end;
    *count = node->count;
    *sample_id = node->sample_id;
    *phase = node->phase == VRD_HOMOZYGOUS ? (size_t) -1 : node->phase;
    *inserted = node->inserted;
} // vrd_MNV_unpack


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                         size_t const start,
                                         size_t const end,
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
    self->nodes[ptr].key = start;
    self->nodes[ptr].count = count;
    self->nodes[ptr].end = end;
    self->nodes[ptr].max = end;
    self->nodes[ptr].balance = 0;
    self->nodes[ptr].sample_id = sample_id;
    self->nodes[ptr].phase = phase;
    self->nodes[ptr].inserted = inserted;

    insert(self, ptr);

    return 0;
} // vrd_MNV_tree_insert


static size_t
query(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
      size_t const root,
      size_t const start,
      size_t const end,
      size_t const inserted,
      bool const homozygous,
      vrd_AVL_Tree const* const subset)
{
    if (NULLPTR == root || self->nodes[root].max < start)
    {
        return 0;
    } // if

    if (self->nodes[root].key > start)
    {
        return query(self, self->nodes[root].child[LEFT], start, end, inserted, homozygous, subset);
    } // if

    size_t res = 0;
    // TODO: match inserted; IUPAC, overlap, ...
    if (start == self->nodes[root].key &&
        end == self->nodes[root].end &&
        inserted == self->nodes[root].inserted &&
        (!homozygous || (homozygous && self->nodes[root].phase == VRD_HOMOZYGOUS)) &&
        (NULL == subset || vrd_AVL_tree_is_element(subset, self->nodes[root].sample_id)))
    {
        res = self->nodes[root].count;
    } // if

    return res + query(self, self->nodes[root].child[LEFT], start, end, inserted, homozygous, subset) +
                 query(self, self->nodes[root].child[RIGHT], start, end, inserted, homozygous, subset);
} // query


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
    if (NULLPTR == root || next >= len || self->nodes[root].max < start)
    {
        return next;
    } // if

    if (self->nodes[root].key > end)
    {
        return query_region(self, self->nodes[root].child[LEFT], start, end, subset, next, len, result);
    } // if

    size_t match = 0;
    if (start <= self->nodes[root].key && end > self->nodes[root].end &&
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
} // vrd_MNV_tree_query_region


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_query)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                        size_t const start,
                                        size_t const end,
                                        size_t const inserted,
                                        bool const homozygous,
                                        vrd_AVL_Tree const* const subset)
{
    assert(NULL != self);

    return query(self, self->root, start, end, inserted, homozygous, subset);
} // vrd_MNV_tree_query


static size_t
traverse_seq(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
             uint32_t const root,
             int const depth,
             uint64_t const path,
             vrd_AVL_Tree const* const subset,
             vrd_Seq_Table* const seq_table)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    size_t count = 0;

    count += traverse_seq(self, self->nodes[root].child[LEFT], depth + 1, (path << 1) + LEFT, subset, seq_table);
    count += traverse_seq(self, self->nodes[root].child[RIGHT], depth + 1, (path << 1) + RIGHT, subset, seq_table);

    if (vrd_AVL_tree_is_element(subset, self->nodes[root].sample_id))
    {
        node_remove(self, depth, path);
        vrd_Seq_table_remove(seq_table, self->nodes[root].inserted);
        count += 1;
    } // if
    return count;
} // traverse_seq


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_remove_seq)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                             vrd_AVL_Tree const* const subset,
                                             vrd_Seq_Table* const seq_table)
{
    assert(NULL != self);

    size_t const count = traverse_seq(self, self->root, 0, 0, subset, seq_table);
    balance(self);
    update_avl(self, self->root, 0);

    return count;
} // vrd_MNV_tree_remove_seq


static size_t
export(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
       uint32_t const root,
       FILE* stream,
       size_t const len,
       char const reference[len],
       vrd_Seq_Table const* const seq_table)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    size_t count = export(self, self->nodes[root].child[LEFT], stream, len, reference, seq_table);

    char* inserted = NULL;
    size_t const inserted_len = vrd_Seq_table_key(seq_table, self->nodes[root].inserted, &inserted);

    int const phase = self->nodes[root].phase == VRD_HOMOZYGOUS ? -1 : (int) self->nodes[root].phase;

    (void) fprintf(stream, "%s\t%u\t%u\t%u\t%d\t%zu\t%s\n", reference, self->nodes[root].key, self->nodes[root].end, self->nodes[root].count, phase, inserted_len - 1, inserted_len == 1 ? "." : inserted);

    free(inserted);

    count += export(self, self->nodes[root].child[RIGHT], stream, len, reference, seq_table);

    return count + 1;
} // export


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_export)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                         FILE* stream,
                                         size_t const len,
                                         char const reference[len],
                                         vrd_Seq_Table const* const seq_table)
{
    assert(NULL != self);
    assert(NULL != stream);
    assert(NULL != seq_table);

    return export(self, self->root, stream, len, reference, seq_table);
} // vrd_MNV_export


#undef VRD_TYPENAME
