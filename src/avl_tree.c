#include <assert.h>     // assert
#include <stddef.h>     // NULL
#include <stdint.h>     // UINT32_MAX, uint32_t, int32_t, uint64_t,
#include <stdlib.h>     // malloc, free

#include "../include/avl_tree.h"    // vrd_AVL_*, vrd_avl_tree_*


enum
{
    NULLPTR = 0,

    LEFT =  0,
    RIGHT = 1
}; // constants


struct vrd_AVL_Node
{
    uint32_t child[2];
    uint32_t value;
    int32_t  balance :  3;  // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t extra   : 29;  // this extra space can be used to store data
}; // vrd_AVL_Node


struct vrd_AVL_Tree
{
    uint32_t root;

    uint32_t next;
    uint32_t capacity;
    vrd_AVL_Node nodes[];
}; // vrd_AVL_Tree


vrd_AVL_Tree*
vrd_avl_tree_init(uint32_t const capacity)
{
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


// Adapted from:
// http://adtinfo.org/libavl.html/Inserting-into-an-AVL-Tree.html
static
vrd_AVL_Node*
insert(vrd_AVL_Tree* tree, uint32_t const ptr)
{
    assert(NULL != tree);

    // This is the first node in the tree
    if (NULLPTR == tree->root)
    {
        tree->root = ptr;
        return &tree->nodes[ptr];
    } // if

    // limiting to height 64 becomes a problem after allocating 413 TiB
    // at the earliest; it allows for a minimum of
    // 27,777,890,035,287 nodes
    uint64_t path = 0;  // bit-path to first unbalanced ancestor
    int len = 0;    // length of the path
    int dir = 0;

    uint32_t tmp = tree->root;
    uint32_t tmp_par = tree->root;  // parent of tmp

    uint32_t unbal = tree->root;    // first unbalanced ancestor of tmp
    uint32_t unbal_par = tree->root;    // parent of unbalanced

    // Insert a new node at the BST position
    while (NULLPTR != tmp)
    {
        if (0 != tree->nodes[tmp].balance)
        {
            // this is now the first unbalanced ancestor of tmp
            unbal_par = tmp_par;
            unbal = tmp;
            path = 0;
            len = 0;
        } // if

        dir = tree->nodes[ptr].value > tree->nodes[tmp].value;
        if (RIGHT == dir)
        {
            path |= (uint64_t) RIGHT << len;
        } // if
        len += 1;

        tmp_par = tmp;
        tmp = tree->nodes[tmp].child[dir];
    } // while

    tree->nodes[tmp_par].child[dir] = ptr;

    // Update the balance factors along the path from the first
    // unbalanced ancestor to the new node
    tmp = unbal;
    while (tmp != ptr)
    {
        if (LEFT == (path & RIGHT))
        {
            tree->nodes[tmp].balance -= 1;
        } // if
        else
        {
            tree->nodes[tmp].balance += 1;
        } // else

        tmp = tree->nodes[tmp].child[path & RIGHT];
        path >>= 1;
    } // while

    // Do the rotations if necessary
    uint32_t root = 0;
    if (-2 == tree->nodes[unbal].balance)
    {
        uint32_t const child = tree->nodes[unbal].child[LEFT];
        if (-1 == tree->nodes[child].balance)
        {
            root = child;
            tree->nodes[unbal].child[LEFT] = tree->nodes[child].child[RIGHT];
            tree->nodes[child].child[RIGHT] = unbal;
            tree->nodes[child].balance = 0;
            tree->nodes[unbal].balance = 0;
        } // if
        else
        {
            root = tree->nodes[child].child[RIGHT];
            tree->nodes[child].child[RIGHT] = tree->nodes[root].child[LEFT];
            tree->nodes[root].child[LEFT] = child;
            tree->nodes[unbal].child[LEFT] = tree->nodes[root].child[RIGHT];
            tree->nodes[root].child[RIGHT] = unbal;
            if (-1 == tree->nodes[root].balance)
            {
                tree->nodes[child].balance = 0;
                tree->nodes[unbal].balance = 1;
            } // if#include <inttypes.h>
            else if (0 == tree->nodes[root].balance)
            {
                tree->nodes[child].balance = 0;
                tree->nodes[unbal].balance = 0;
            } // if
            else
            {
                tree->nodes[child].balance = -1;
                tree->nodes[unbal].balance = 0;
            } // else
            tree->nodes[root].balance = 0;
        } // else
    } // if
    else if (2 == tree->nodes[unbal].balance)
    {
        uint32_t const child = tree->nodes[unbal].child[RIGHT];
        if (1 == tree->nodes[child].balance)
        {
            root = child;
            tree->nodes[unbal].child[RIGHT] = tree->nodes[child].child[LEFT];
            tree->nodes[child].child[LEFT] = unbal;
            tree->nodes[child].balance = 0;
            tree->nodes[unbal].balance = 0;
        } // if
        else
        {
            root = tree->nodes[child].child[LEFT];
            tree->nodes[child].child[LEFT] = tree->nodes[root].child[RIGHT];
            tree->nodes[root].child[RIGHT] = child;
            tree->nodes[unbal].child[RIGHT] = tree->nodes[root].child[LEFT];
            tree->nodes[root].child[LEFT] = unbal;
            if (1 == tree->nodes[root].balance)
            {
                tree->nodes[child].balance = 0;
                tree->nodes[unbal].balance = -1;
            } // if
            else if(0 == tree->nodes[root].balance)
            {
                tree->nodes[child].balance = 0;
                tree->nodes[unbal].balance = 0;
            } // if
            else
            {
                tree->nodes[child].balance = 1;
                tree->nodes[unbal].balance = 0;
            } // else
            tree->nodes[root].balance = 0;
        } // else
    } // if
    else
    {
        return &tree->nodes[ptr];
    } // else

    if (tree->root == unbal)
    {
        tree->root = root;
        return &tree->nodes[ptr];
    } // if

    tree->nodes[unbal_par].child[unbal !=
                                 tree->nodes[unbal_par].child[LEFT]] = root;
    return &tree->nodes[ptr];
} // insert


vrd_AVL_Node*
vrd_avl_tree_insert(vrd_AVL_Tree* const tree, uint32_t const value)
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
    tree->nodes[ptr].extra = 0;

    return insert(tree, ptr);
} // vrd_avl_tree_insert


#ifndef NDEBUG

#include <errno.h>  // errno
#include <inttypes.h>   // PRIu32
#include <stdio.h>  // FILE, fprintf, perror


static int
print(FILE* restrict stream,
      vrd_AVL_Tree const* const restrict tree,
      uint32_t const root,
      int const indent)
{
    static int const INDENT = 8;

    if (NULLPTR == root)
    {
        return 0;
    } // if

    int ret = print(stream, tree, tree->nodes[root].child[RIGHT], indent + INDENT);
    if (0 > ret)
    {
        return ret;
    } // if

    int written = ret;

    errno = 0;
    ret = fprintf(stream, "%*s%" PRIu32 " (%2d)\n", indent, "", tree->nodes[root].value, tree->nodes[root].balance);
    if (0 > ret)
    {
        perror("fprintf() failed");
        return ret;
    } // if

    written += ret;

    ret = print(stream, tree, tree->nodes[root].child[LEFT], indent + INDENT);
    if (0 > ret)
    {
        return ret;
    } // if

    return written + ret;
} // print


int
vrd_avl_tree_print(FILE* restrict stream,
                   vrd_AVL_Tree const* const restrict tree)
{
    assert(NULL != stream);
    assert(NULL != tree);

    return print(stream, tree, tree->root, 0);
} // vrd_avl_tree_print

#endif
