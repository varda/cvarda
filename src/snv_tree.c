#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, fread, fwrite
#include <stdint.h>     // UINT32_MAX, uint32_t, int32_t, uint64_t,
#include <stdlib.h>     // malloc, free

#include "../include/avl_tree.h"    // vrd_AVL_Tree,
                                    // vrd_avl_tree_is_element
#include "snv_tree.h"    // vrd_SNV_*, vrd_snv_tree_*
#include "tree.h"   // NULLPTR, LEFT, RIGHT, HOMOZYGOUS


struct vrd_SNV_Node
{
    uint32_t child[2];
    uint32_t position;
    int32_t  balance   :  3;    // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t sample_id : 29;
    uint32_t phase     : 28;
    uint32_t inserted  :  4;    // [0, ..., 15]
}; // vrd_SNV_Node


struct vrd_SNV_Tree
{
    uint32_t root;

    uint32_t next;
    uint32_t capacity;
    vrd_SNV_Node nodes[];
}; // vrd_SNV_Tree


vrd_SNV_Tree*
vrd_snv_tree_init(size_t const capacity)
{
    if ((size_t) UINT32_MAX <= capacity)
    {
        return NULL;
    } // if

    vrd_SNV_Tree* const tree = malloc(sizeof(vrd_SNV_Tree) +
                                      sizeof(vrd_SNV_Node) *
                                      ((size_t) capacity + 1));
    if (NULL == tree)
    {
        return NULL;
    } // if

    tree->root = NULLPTR;
    tree->next = 1;
    tree->capacity = capacity;

    return tree;
} // vrd_snv_tree_init


void
vrd_snv_tree_destroy(vrd_SNV_Tree* restrict* const tree)
{
    if (NULL == tree)
    {
        return;
    } // if

    free(*tree);
    *tree = NULL;
} // vrd_snv_tree_destroy


#define TREE vrd_SNV_Tree
#define NODE vrd_SNV_Node
#define KEY position
#include "tree_insert.inc"
#undef KEY
#undef NODE
#undef TREE


vrd_SNV_Node*
vrd_snv_tree_insert(vrd_SNV_Tree* const tree,
                    size_t const position,
                    size_t const sample_id,
                    size_t const phase,
                    size_t const inserted)
{
    assert(NULL != tree);

    if (UINT32_MAX == tree->next || tree->capacity < tree->next)
    {
        return NULL;
    } // if

    uint32_t const ptr = tree->next;
    tree->next += 1;

    tree->nodes[ptr].child[LEFT] = NULLPTR;
    tree->nodes[ptr].child[RIGHT] = NULLPTR;
    tree->nodes[ptr].position = position;
    tree->nodes[ptr].balance = 0;
    tree->nodes[ptr].sample_id = sample_id;
    tree->nodes[ptr].phase = phase;
    tree->nodes[ptr].inserted = inserted;

    return insert(tree, ptr);
} // vrd_snv_tree_insert


static size_t
query_contains(vrd_SNV_Tree const* const restrict tree,
               uint32_t const root,
               size_t const position,
               size_t const inserted,
               vrd_AVL_Tree const* const restrict subset)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    if (tree->nodes[root].position > position)
    {
        return query_contains(tree, tree->nodes[root].child[LEFT], position, inserted, subset);
    } // if

    if (tree->nodes[root].position < position)
    {
        return query_contains(tree, tree->nodes[root].child[RIGHT], position, inserted, subset);
    } // if

    size_t res = 0;
    // TODO: IUPAC match on inserted
    if (tree->nodes[root].inserted == inserted &&
        (NULL == subset || vrd_avl_tree_is_element(subset, tree->nodes[root].sample_id)))
    {
        res = 1;
        if (HOMOZYGOUS == tree->nodes[root].phase)
        {
            res = 2;
        } // if
    } // if

    return res + query_contains(tree, tree->nodes[root].child[LEFT], position, inserted, subset) +
                 query_contains(tree, tree->nodes[root].child[RIGHT], position, inserted, subset);
} // query_contains


size_t
vrd_snv_tree_query(vrd_SNV_Tree const* const restrict tree,
                   size_t const position,
                   size_t const inserted,
                   vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != tree);

    return query_contains(tree, tree->root, position, inserted, subset);
} // vrd_snv_tree_query


#define TREE vrd_SNV_Tree
#include "tree_remove.inc"  // traverse, balance, update_avl
#undef TREE


#define TREE vrd_SNV_Tree
#define NODE vrd_SNV_Node
#include "tree_layout.inc"  // reorder
#undef NODE
#undef TREE


size_t
vrd_snv_tree_remove(vrd_SNV_Tree* const restrict tree,
                    vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != tree);

    size_t const count = traverse(tree, tree->root, 0, 0, subset);
    balance(tree);
    update_avl(tree, tree->root);

    reorder(tree);

    return count;
} // vrd_snv_tree_remove


void
vrd_snv_tree_reorder(vrd_SNV_Tree* const tree)
{
    assert(NULL != tree);

    reorder(tree);
} // vrd_snv_tree_reorder


int
vrd_snv_tree_read(vrd_SNV_Tree* const restrict tree,
                  FILE* restrict stream)
{
    assert(NULL != tree);

    size_t count = fread(&tree->root, sizeof(tree->root), 1, stream);
    if (1 != count)
    {
        return -1;
    } // if
    count = fread(&tree->next, sizeof(tree->next), 1, stream);
    if (1 != count)
    {
        return -1;
    } // if
    count = fread(&tree->nodes, sizeof(tree->nodes[0]), tree->next, stream);
    if (tree->next != count)
    {
        return -1;
    } // if
    return 0;
} // vrd_snv_tree_read


int
vrd_snv_tree_write(vrd_SNV_Tree const* const restrict tree,
                   FILE* restrict stream)
{
    assert(NULL != tree);

    size_t count = fwrite(&tree->root, sizeof(tree->root), 1, stream);
    if (1 != count)
    {
        return -1;
    } // if
    count = fwrite(&tree->next, sizeof(tree->next), 1, stream);
    if (1 != count)
    {
        return -1;
    } // if
    count = fwrite(&tree->nodes, sizeof(tree->nodes[0]), tree->next, stream);
    if (tree->next != count)
    {
        return -1;
    } // if
    return 0;
} // vrd_snv_tree_write
