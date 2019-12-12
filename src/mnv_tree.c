#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // int32_t, uint32_t
#include <stdio.h>      // FILE, fprintf

#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_AVL_tree_*
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

    uint32_t phase;
    uint32_t inserted;
}; // vrd_MNV_Node


#define VRD_INTERVAL
#include "template_tree.inc"    // vrd_MNV_tree_*
#undef VRD_INTERVAL


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
      vrd_AVL_Tree const* const subset)
{
    if (NULLPTR == root || self->nodes[root].max < start)
    {
        return 0;
    } // if

    if (self->nodes[root].key > start)
    {
        return query(self, self->nodes[root].child[LEFT], start, end, inserted, subset);
    } // if

    size_t res = 0;
    // TODO: match inserted; IUPAC, overlap, ...
    if (start == self->nodes[root].key &&
        end == self->nodes[root].end &&
        inserted == self->nodes[root].inserted &&
        (NULL == subset || vrd_AVL_tree_is_element(subset, self->nodes[root].sample_id)))
    {
        res = self->nodes[root].count;
    } // if

    return res + query(self, self->nodes[root].child[LEFT], start, end, inserted, subset) +
                 query(self, self->nodes[root].child[RIGHT], start, end, inserted, subset);
} // query


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_query)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                        size_t const start,
                                        size_t const end,
                                        size_t const inserted,
                                        vrd_AVL_Tree const* const subset)
{
    assert(NULL != self);

    return query(self, self->root, start, end, inserted, subset);
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


static void
export(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
       uint32_t const root,
       FILE* stream,
       size_t const len,
       char const reference[len],
       vrd_Seq_Table const* const seq_table)
{
    if (NULLPTR == root)
    {
        return;
    } // if

    export(self, self->nodes[root].child[LEFT], stream, len, reference, seq_table);

    char* inserted = NULL;
    size_t const inserted_len = vrd_Seq_table_key(seq_table, self->nodes[root].inserted, &inserted);

    (void) fprintf(stream, "%s\t%u\t%u\t%u\t%u\t%zu\t%s\n", reference, self->nodes[root].key, self->nodes[root].end, self->nodes[root].count, self->nodes[root].phase, inserted_len - 1, inserted);

    free(inserted);

    export(self, self->nodes[root].child[RIGHT], stream, len, reference, seq_table);
} // export


void
VRD_TEMPLATE(VRD_TYPENAME, _tree_export)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                         FILE* stream,
                                         size_t const len,
                                         char const reference[len],
                                         vrd_Seq_Table const* const seq_table)
{
    assert(NULL != self);
    assert(NULL != stream);
    assert(NULL != seq_table);

    export(self, self->root, stream, len, reference, seq_table);
} // vrd_MNV_export


#undef VRD_TYPENAME
