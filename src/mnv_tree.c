#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX, uint32_t, int32_t
#include <stdlib.h>     // malloc, free

#include "mnv_tree.h"    // vrd_MNV_*, vrd_mnv_tree_*
#include "tree.h"   // NULLPTR, LEFT, RIGHT, HOMOZYGOUS


struct vrd_MNV_Node
{
    uint32_t child[2];
    uint32_t start;
    uint32_t end;
    uint32_t max;
    int32_t  balance   :  3;  // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t sample_id : 29;
    uint32_t phase;
    char const* inserted;
}; // vrd_MNV_Node


struct vrd_MNV_Tree
{
    uint32_t root;

    uint32_t next;
    uint32_t capacity;
    vrd_MNV_Node nodes[];
}; // vrd_MNV_Tree


vrd_MNV_Tree*
vrd_mnv_tree_init(size_t const capacity)
{
    if ((size_t) UINT32_MAX <= capacity)
    {
        return NULL;
    } // if

    vrd_MNV_Tree* const tree = malloc(sizeof(vrd_MNV_Tree) +
                                      sizeof(vrd_MNV_Node) *
                                      ((size_t) capacity + 1));
    if (NULL == tree)
    {
        return NULL;
    } // if

    tree->root = NULLPTR;
    tree->next = 1;
    tree->capacity = capacity;

    return tree;
} // vrd_mnv_tree_init


void
vrd_mnv_tree_destroy(vrd_MNV_Tree* restrict* const tree)
{
    if (NULL != tree)
    {
        free(*tree);
        *tree = NULL;
    } // if
} // vrd_mnv_tree_destroy


#define TREE vrd_MNV_Tree
#define NODE vrd_MNV_Node
#define KEY start
#define ITV
#include "tree_insert.inc"
#undef ITV
#undef KEY
#undef NODE
#undef TREE


vrd_MNV_Node*
vrd_mnv_tree_insert(vrd_MNV_Tree* const restrict tree,
                    size_t const start,
                    size_t const end,
                    size_t const sample_id,
                    size_t const phase,
                    char const* const restrict inserted)
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
    tree->nodes[ptr].start = start;
    tree->nodes[ptr].end = end;
    tree->nodes[ptr].max = end;
    tree->nodes[ptr].balance = 0;
    tree->nodes[ptr].sample_id = sample_id;
    tree->nodes[ptr].phase = phase;
    tree->nodes[ptr].inserted = inserted;

    return insert(tree, ptr);
} // vrd_mnv_tree_insert


static
size_t
query_contains(vrd_MNV_Tree const* const restrict tree,
               size_t const root,
               size_t const start,
               size_t const end,
               char const* const restrict inserted,
               vrd_AVL_Tree const* const restrict subset)
{
    if (NULLPTR == root || tree->nodes[root].max < start)
    {
        return 0;
    } // if

    if (tree->nodes[root].start > start)
    {
        return query_contains(tree, tree->nodes[root].child[LEFT], start, end, inserted, subset);
    } // if

    // TODO: semantics on inserted
    size_t res = 0;
    if (start >= tree->nodes[root].start &&
        end <= tree->nodes[root].end &&
        inserted == tree->nodes[root].inserted &&
        (NULL == subset || vrd_avl_tree_is_element(subset, tree->nodes[root].sample_id)))
    {
        res = 1;
        if (HOMOZYGOUS == tree->nodes[root].phase)
        {
            res = 2;
        } // if
    } // if

    return res + query_contains(tree, tree->nodes[root].child[LEFT], start, end, inserted, subset) +
                 query_contains(tree, tree->nodes[root].child[RIGHT], start, end, inserted, subset);
} // query_contains


size_t
vrd_mnv_tree_query(vrd_MNV_Tree const* const restrict tree,
                   size_t const start,
                   size_t const end,
                   char const* const restrict inserted,
                   vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != tree);

    return query_contains(tree, tree->root, start, end, inserted, subset);
} // vrd_mnv_tree_query


#define TREE vrd_MNV_Tree
#define ITV
#include "tree_remove.inc"
#undef ITV
#undef TREE


size_t
vrd_mnv_tree_remove(vrd_MNV_Tree* const restrict tree,
                    vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != tree);

    size_t const count = traverse(tree, tree->root, 0, 0, subset);
    balance(tree);
    uint32_t new_max = 0;
    update_avl(tree, tree->root, &new_max);

    return count;
} // vrd_mnv_tree_remove


#define TREE vrd_MNV_Tree
#define NODE vrd_MNV_Node
#include "tree_layout.inc"
#undef NODE
#undef TREE


void
vrd_mnv_tree_reorder(vrd_MNV_Tree* const tree)
{
    assert(NULL != tree);

    reorder(tree);
} // vrd_mnv_tree_reorder
