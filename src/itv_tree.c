#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX, uint32_t, int32_t, uint64_t,
#include <stdlib.h>     // malloc, free

#include "../include/avl_tree.h"    // vrd_AVL_Tree,
                                    // vrd_avl_tree_is_element
#include "../include/itv_tree.h"    // vrd_Itv_*, vrd_itv_tree_*
#include "tree.h"   // NULLPTR, LEFT, RIGHT, max


typedef struct
{
    uint32_t child[2];
    uint32_t start;
    uint32_t end;
    uint32_t max;
    int32_t  balance   :  3;  // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t sample_id : 29;
} vrd_Itv_Node;


struct vrd_Itv_Tree
{
    uint32_t root;

    uint32_t next;
    uint32_t capacity;
    vrd_Itv_Node nodes[];
}; // vrd_Itv_Tree


vrd_Itv_Tree*
vrd_itv_tree_init(size_t const capacity)
{
    if ((size_t) UINT32_MAX <= capacity)
    {
        return NULL;
    } // if

    vrd_Itv_Tree* const tree = malloc(sizeof(vrd_Itv_Tree) +
                                      sizeof(vrd_Itv_Node) *
                                      ((size_t) capacity + 1));
    if (NULL == tree)
    {
        return NULL;
    } // if

    tree->root = NULLPTR;
    tree->next = 1;
    tree->capacity = capacity;

    return tree;
} // vrd_itv_tree_init


void
vrd_itv_tree_destroy(vrd_Itv_Tree* restrict* const tree)
{
    if (NULL != tree)
    {
        free(*tree);
        *tree = NULL;
    } // if
} // vrd_itv_tree_destroy


static inline uint32_t
update_max(vrd_Itv_Tree const* const tree, uint32_t const root)
{
    uint32_t res = tree->nodes[root].max;
    if (NULLPTR != tree->nodes[root].child[LEFT])
    {
        res = max(res, tree->nodes[tree->nodes[root].child[LEFT]].max);
    } // if
    if (NULLPTR != tree->nodes[root].child[RIGHT])
    {
        res = max(res, tree->nodes[tree->nodes[root].child[RIGHT]].max);
    } // if
    return res;
} // update_max


static vrd_Itv_Node*
insert(vrd_Itv_Tree* const tree, uint32_t const ptr)
{
    assert(NULL != tree);

#define KEY start
#define ITV
#include "tree_insert.inc"
#undef ITV
#undef KEY
} // insert


void*
vrd_itv_tree_insert(vrd_Itv_Tree* const tree,
                    size_t const start,
                    size_t const end,
                    size_t const sample_id)
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

    return insert(tree, ptr);
} // vrd_itv_tree_insert


static
size_t
query_contains(vrd_Itv_Tree const* const restrict tree,
               size_t const root,
               size_t const start,
               size_t const end,
               vrd_AVL_Tree const* const restrict subset)
{
    if (NULLPTR == root || tree->nodes[root].max < start)
    {
        return 0;
    } // if

    if (tree->nodes[root].start > start)
    {
        return query_contains(tree, tree->nodes[root].child[LEFT], start, end, subset);
    } // if

    size_t res = 0;
    if (start >= tree->nodes[root].start &&
        end <= tree->nodes[root].end &&
        (NULL == subset || vrd_avl_tree_is_element(subset, tree->nodes[root].sample_id)))
    {
        res = 1;
    } // if

    return res + query_contains(tree, tree->nodes[root].child[LEFT], start, end, subset) +
                 query_contains(tree, tree->nodes[root].child[RIGHT], start, end, subset);
} // query_contains


size_t
vrd_itv_tree_query(vrd_Itv_Tree const* const restrict tree,
                   size_t const start,
                   size_t const end,
                   vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != tree);

    return query_contains(tree, tree->root, start, end, subset);
} // vrd_itv_tree_query





#include <inttypes.h>   // PRId32, PRIu32
#include <stdio.h>  // stderr, fprintf


static void
print(FILE* restrict stream,
      vrd_Itv_Tree const* const restrict tree,
      uint32_t const root,
      int const indent)
{
    if (NULLPTR == root)
    {
        return;
    } // if

    enum
    {
        INDENT = 8
    }; // constants

    print(stream, tree, tree->nodes[root].child[RIGHT], indent + INDENT);
    (void) fprintf(stream, "%*s%" PRIu32 "--%" PRIu32 " [%" PRIu32 "] (%2" PRId32 ")\n", indent, "", tree->nodes[root].start, tree->nodes[root].end, tree->nodes[root].max, tree->nodes[root].balance);
    print(stream, tree, tree->nodes[root].child[LEFT], indent + INDENT);
} // print


#define TREE vrd_Itv_Tree
#define ITV
#include "tree_remove.inc"
#undef ITV
#undef TREE


size_t
vrd_itv_tree_remove(vrd_Itv_Tree* const restrict tree,
                    vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != tree);

    print(stderr, tree, tree->root, 0);
    size_t const count = traverse(tree, tree->root, 0, 0, subset);
    balance(tree);
    uint32_t new_max = 0;
    update_avl(tree, tree->root, &new_max);
    (void) fprintf(stderr, "======\n\n");
    print(stderr, tree, tree->root, 0);

    return count;
} // vrd_itv_tree_remove
