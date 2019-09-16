#include <stdint.h>     // UINT32_MAX, uint32_t, int32_t, uint64_t
#include <stdlib.h>     // NULL, malloc, free

#include "../include/alloc.h"       // vrd_Alloc, vrd_alloc, vrd_dealloc,
                                    // vrd_deref, vrd_pool_*
#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_avl_*


enum
{
    LEFT = 0,
    RIGHT = 1
}; // constants


struct AVL_Node
{
    uint32_t child[2];
    uint32_t value;
    int32_t balance  :  6;
    uint32_t sample  : 26;
}; // AVL_Node


struct AVL_Tree
{
    vrd_Alloc* restrict alloc;
    uint32_t root;
}; // AVL_Tree


static inline uint32_t
avl_node_init(vrd_Alloc* const restrict alloc,
              uint32_t const value,
              uint32_t const sample)
{
    void* const restrict ptr =
        vrd_alloc(alloc, sizeof(struct AVL_Node));
    if (NULL == ptr)
    {
        return 0;
    } // if

    struct AVL_Node* const restrict node = vrd_deref(alloc, ptr);
    node->child[LEFT] = 0;
    node->child[RIGHT] = 0;
    node->value = value;
    node->balance = 0;
    node->sample = sample;

    return (uint32_t) (size_t) ptr;
} // avl_node32_init


vrd_AVL_Tree*
vrd_avl_init(size_t const capacity)
{
    if (UINT32_MAX <= capacity)
    {
        return NULL;
    } // if

    struct AVL_Tree* const restrict tree = malloc(sizeof(*tree));
    if (NULL == tree)
    {
        return NULL;
    } // if

    tree->alloc = vrd_pool_init(capacity, sizeof(struct AVL_Node));
    if (NULL == tree->alloc)
    {
        free(tree);
        return NULL;
    } // if

    tree->root = 0;

    return tree;
} // vrd_avl_init


void
vrd_avl_destroy(vrd_AVL_Tree* restrict* const restrict tree)
{
    if (NULL == tree || NULL == *tree)
    {
        return;
    } // if

    vrd_pool_destroy(&(*tree)->alloc);
    free(*tree);
    *tree = NULL;
} // vrd_avl_destroy


// Adapted from:
//     http://adtinfo.org/libavl.html/Inserting-into-an-AVL-Tree.html
uint32_t
vrd_avl_insert(vrd_AVL_Tree* const restrict tree,
               uint32_t const value,
               uint32_t const sample)
{
#define DEREF(ptr) ((struct AVL_Node*) vrd_deref(tree->alloc, (void*) (size_t) (ptr)))

    if (NULL == tree)
    {
        return 0;
    } // if

    if (0 == tree->root)
    {
        tree->root = avl_node_init(tree->alloc, value, sample);
        return tree->root;
    } // if


    // limiting to height 64 becomes a problem after allocating 413 TiB
    // at the earliest; it allows for a minimum of
    // 27,777,890,035,287 nodes
    uint64_t cache = 0; // bit-path to first non-zero balance ancestor
    int k = 0;
    int dir = 0;

    uint32_t z = tree->root; // parent of y
    uint32_t y = tree->root; // first non-zero balance ancestor of p

    uint32_t q = tree->root; // parent of p
    uint32_t p = tree->root;

    while (0 != p)
    {
        if (0 != DEREF(p)->balance)
        {
            z = q;
            y = p;
            cache = 0;
            k = 0;
        } // if

        uint32_t const cmp = value - DEREF(p)->value;
        dir = cmp > 0;
        if (1 == dir)
        {
            cache |= UINT64_C(1) << k;
        } // if
        k += 1;

        q = p;
        p = DEREF(p)->child[dir];
    } // while

    uint32_t const n = avl_node_init(tree->alloc, value, sample);
    DEREF(q)->child[dir] = n;

    if (0 == y)
    {
        return n;
    } // if

    p = y;
    while (p != n)
    {
        if ((cache & 1) == 0)
        {
            DEREF(p)->balance -= 1;
        } // if
        else
        {
            DEREF(p)->balance += 1;
        } // else

        p = DEREF(p)->child[cache & 1];
        cache >>= 1;
    } // while

    uint32_t r = 0; // new root

    if (-2 == DEREF(y)->balance)
    {
        uint32_t const x = DEREF(y)->child[LEFT];
        if (-1 == DEREF(x)->balance)
        {
            r = x;
            DEREF(y)->child[LEFT] = DEREF(x)->child[RIGHT];
            DEREF(x)->child[RIGHT] = y;
            DEREF(x)->balance = 0;
            DEREF(y)->balance = 0;
        } // if
        else
        {
            r = DEREF(x)->child[RIGHT];
            DEREF(x)->child[RIGHT] = DEREF(r)->child[LEFT];
            DEREF(r)->child[LEFT] = x;
            DEREF(y)->child[LEFT] = DEREF(r)->child[RIGHT];
            DEREF(r)->child[RIGHT] = y;
            if (-1 == DEREF(r)->balance)
            {
                DEREF(x)->balance = 0;
                DEREF(y)->balance = 1;
            } // if
            else if (0 == DEREF(r)->balance)
            {
                DEREF(x)->balance = 0;
                DEREF(y)->balance = 0;
            } // if
            else
            {
                DEREF(x)->balance = -1;
                DEREF(y)->balance = 0;
            } // else
            DEREF(r)->balance = 0;
        } // else
    } // if
    else if (2 == DEREF(y)->balance)
    {
        uint32_t const x = DEREF(y)->child[RIGHT];
        if (1 == DEREF(x)->balance)
        {
            r = x;
            DEREF(y)->child[RIGHT] = DEREF(x)->child[LEFT];
            DEREF(x)->child[LEFT] = y;
            DEREF(x)->balance = 0;
            DEREF(y)->balance = 0;
        } // if
        else
        {
            r = DEREF(x)->child[LEFT];
            DEREF(x)->child[LEFT] = DEREF(r)->child[RIGHT];
            DEREF(r)->child[RIGHT] = x;
            DEREF(y)->child[RIGHT] = DEREF(r)->child[LEFT];
            DEREF(r)->child[LEFT] = y;
            if (1 == DEREF(r)->balance)
            {
                DEREF(x)->balance = 0;
                DEREF(y)->balance = -1;
            } // if
            else if(0 == DEREF(r)->balance)
            {
                DEREF(x)->balance = 0;
                DEREF(y)->balance = 0;
            } // if
            else
            {
                DEREF(x)->balance = 1;
                DEREF(y)->balance = 0;
            } // else
            DEREF(r)->balance = 0;
        } // else
    } // if
    else
    {
        return n;
    } // else

    if (y == tree->root)
    {
        return n;
    } // if

    DEREF(z)->child[y != DEREF(z)->child[LEFT]] = r;
    return n;
#undef DEREF
} // vrd_avl_insert
