/**
 * @file avl_tree.c
 *
 * Implements an AVL tree
 */

#include <assert.h>     // assert
#include <stdbool.h>    // bool, false, true
#include <stddef.h>     // NULL
#include <stdint.h>     // UINT32_MAX, uint32_t, int32_t, uint64_t,
#include <stdlib.h>     // malloc, free

#include "../include/avl_tree.h"    // vrd_AVL_*, vrd_avl_tree_*
#include "tree.h"   // NULLPTR, LEFT, RIGHT


typedef struct
{
    uint32_t child[2];
    uint32_t value;
    int32_t  balance;  // we use [-2, ..., 2]
} vrd_AVL_Node;


struct vrd_AVL_Tree
{
    uint32_t root;

    uint32_t next;
    uint32_t capacity;
    vrd_AVL_Node nodes[];
}; // vrd_AVL_Tree


vrd_AVL_Tree*
vrd_avl_tree_init(size_t const capacity)
{
    if ((size_t) UINT32_MAX <= capacity)
    {
        return NULL;
    } // if

    vrd_AVL_Tree* const tree = malloc(sizeof(vrd_AVL_Tree) +
                                      sizeof(vrd_AVL_Node) *
                                      ((size_t) capacity + 1));
    if (NULL == tree)
    {
        return NULL;
    } // if

    tree->root = NULLPTR;
    tree->next = 1;
    tree->capacity = capacity;

    return tree;
} // vrd_avl_tree_init


void
vrd_avl_tree_destroy(vrd_AVL_Tree* restrict* const tree)
{
    if (NULL != tree)
    {
        free(*tree);
        *tree = NULL;
    } // if
} // vrd_avl_tree_destroy


#define TREE vrd_AVL_Tree
#define NODE vrd_AVL_Node
#define KEY value
#include "tree_insert.inc"
#undef KEY
#undef NODE
#undef TREE


void*
vrd_avl_tree_insert(vrd_AVL_Tree* const tree, size_t const value)
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
    tree->nodes[ptr].value = value;
    tree->nodes[ptr].balance = 0;

    return insert(tree, ptr);
} // vrd_avl_tree_insert


bool
vrd_avl_tree_is_element(vrd_AVL_Tree const* const tree,
                        size_t const value)
{
    assert(NULL != tree);

    uint32_t tmp = tree->root;
    while (NULLPTR != tmp)
    {
        if (value == tree->nodes[tmp].value)
        {
            return true;
        } // if
        tmp = tree->nodes[tmp].child[value > tree->nodes[tmp].value];
    } // while

    return false;
} // vrd_avl_tree_is_element





#include <inttypes.h>   // PRId32, PRIu32
#include <stdio.h>  // stderr, fprintf


void
print(FILE* restrict stream,
      vrd_AVL_Tree const* const restrict tree,
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
    (void) fprintf(stream, "%*s%" PRIu32 " (%2" PRId32 ") [%u]\n", indent, "", tree->nodes[root].value, tree->nodes[root].balance, root);
    print(stream, tree, tree->nodes[root].child[LEFT], indent + INDENT);
} // print



#define TREE vrd_AVL_Tree
#define NODE vrd_AVL_Node
#include "tree_layout.inc"
#undef NODE
#undef TREE


void
vrd_avl_tree_reorder(vrd_AVL_Tree* const tree)
{
    assert(NULL != tree);

    print(stderr, tree, tree->root, 0);
    reorder(tree);
    print(stderr, tree, tree->root, 0);
} // vrd_avl_tree_reorder
