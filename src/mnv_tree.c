#include <assert.h>     // assert
#include <stddef.h>     // NULL
#include <stdint.h>     // UINT32_MAX, uint32_t, int32_t, uint64_t,
#include <stdlib.h>     // malloc, free

#include "../include/mnv_tree.h"    // vrd_MNV_*, vrd_mnv_tree_*


enum
{
    NULLPTR = 0,

    LEFT =  0,
    RIGHT = 1
}; // constants


struct vrd_MNV_Node
{
    uint32_t child[2];
    uint32_t start;
    uint32_t end;
    uint32_t max;
    int32_t  balance   :  3;  // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t sample_id : 29;
    uint32_t phase;
    void* inserted;
}; // vrd_MNV_Node


struct vrd_MNV_Tree
{
    uint32_t root;

    uint32_t next;
    uint32_t capacity;
    vrd_MNV_Node nodes[];
}; // vrd_MNV_Tree


vrd_MNV_Tree*
vrd_mnv_tree_init(uint32_t const capacity)
{
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
vrd_mnv_tree_destroy(vrd_MNV_Tree** const tree)
{
    if (NULL != tree)
    {
        free(*tree);
        *tree = NULL;
    } // if
} // vrd_mnv_tree_destroy


static inline uint32_t
max(uint32_t const a,
    uint32_t const b)
{
    return a > b ? a : b;
} // max


static inline uint32_t
update_max(vrd_MNV_Tree const* const tree,
           uint32_t const root)
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


// Adapted from:
// http://adtinfo.org/libmnv.html/Inserting-into-an-MNV-Tree.html
static
vrd_MNV_Node*
insert(vrd_MNV_Tree* tree, uint32_t const ptr)
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

        dir = tree->nodes[ptr].start > tree->nodes[tmp].start;
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
            tree->nodes[child].max = update_max(tree, child);
            tree->nodes[unbal].max = update_max(tree, unbal);
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
            tree->nodes[root].max = update_max(tree, root);
            tree->nodes[child].max = update_max(tree, child);
            tree->nodes[unbal].max = update_max(tree, unbal);
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
            tree->nodes[child].max = update_max(tree, child);
            tree->nodes[unbal].max = update_max(tree, unbal);
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
            tree->nodes[root].max = update_max(tree, root);
            tree->nodes[child].max = update_max(tree, child);
            tree->nodes[unbal].max = update_max(tree, unbal);
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
    tree->nodes[unbal_par].max = update_max(tree, unbal_par);
    return &tree->nodes[ptr];
} // insert


vrd_MNV_Node*
vrd_mnv_tree_insert(vrd_MNV_Tree* const tree,
                    uint32_t const start,
                    uint32_t const end,
                    uint32_t const sample_id,
                    uint32_t const phase,
                    void* const inserted)
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
