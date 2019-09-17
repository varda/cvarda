#include <stdint.h>     // uint32_t, int32_t, uint64_t, int64_t,
                        // UINT64_C, uintptr_t
#include <stdlib.h>     // NULL, malloc, free

#include "../include/alloc.h"       // vrd_Alloc, vrd_alloc, vrd_deref
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
    int32_t balance  :  3;  // [-2, .., 2]
    uint32_t sample  : 29;
}; // AVL_Node


struct AVL_Tree
{
    struct AVL_Node* restrict root;
    vrd_Alloc* restrict alloc;
}; // AVL_Tree


static inline struct AVL_Node*
avl_node_init(vrd_Alloc* const restrict alloc,
              uint32_t const value,
              uint32_t const sample)
{
    void* const restrict ptr = vrd_alloc(alloc, sizeof(struct AVL_Node));
    if (NULL == ptr)
    {
        return NULL;
    } // if

    struct AVL_Node* const restrict node = vrd_deref(alloc, ptr);
    node->child[LEFT] = 0;
    node->child[RIGHT] = 0;
    node->value = value;
    node->balance = 0;
    node->sample = sample;

    return node;
} // avl_node_init


vrd_AVL_Tree*
vrd_avl_init(vrd_Alloc* const restrict alloc)
{
    if (NULL == alloc)
    {
        return NULL;
    } // if

    struct AVL_Tree* const restrict tree = malloc(sizeof(*tree));
    if (NULL == tree)
    {
        return NULL;
    } // if

    tree->root = NULL;
    tree->alloc = alloc;

    return tree;
} // vrd_avl_init


void
vrd_avl_destroy(vrd_AVL_Tree* restrict* const restrict tree)
{
    if (NULL == tree)
    {
        return;
    } // if

    free(*tree);
    *tree = NULL;
} // vrd_avl_destroy


int
vrd_avl_insert(vrd_AVL_Tree* const restrict tree,
               uint32_t const value,
               uint32_t const sample)
{
#define PTR_EXPAND(ptr) ((void*) (uintptr_t) ptr)
#define PTR_SHORTEN(ptr) ((uint32_t) (uintptr_t) ptr)


    if (NULL == tree)
    {
        return -1;
    } // if

    // FIXME: range check sample (29 bits)
    if (NULL == tree->root)
    {
        tree->root = avl_node_init(tree->alloc, value, sample);
        return 0;
    } // if

    // limiting to height 64 becomes a problem after allocating 413 TiB
    // at the earliest; it allows for a minimum of
    // 27,777,890,035,287 nodes
    uint64_t path = 0; // bit-path to first non-zero balance ancestor
    int len = 0; // length of the path
    int dir = 0;

    struct AVL_Node* restrict tmp_par = tree->root; // parent of tmp
    struct AVL_Node* restrict tmp = tree->root;

    struct AVL_Node* restrict unbal = tree->root; // first non-zero balance ancestor of tmp
    struct AVL_Node* restrict unbal_par = tree->root; // parent of unbalanced


    // Insert a new node at the BST position
    while (NULL != tmp)
    {
        int64_t const cmp = (int64_t) value - (int64_t) tmp->value;

        if (tmp->balance != 0)
        {
            // this node is now the first non-zero balance ancestor of tmp
            unbal_par = tmp_par;
            unbal = tmp;
            path = 0;
            len = 0;
        } // if

        dir = cmp > 0;
        if (dir == 1)
        {
            path |= UINT64_C(1) << len;
        } // if
        len += 1;

        tmp_par = tmp;
        tmp = vrd_deref(tree->alloc, PTR_EXPAND(tmp->child[dir]));
    } // while


    struct AVL_Node* const restrict node =
        avl_node_init(tree->alloc, value, sample);
    tmp_par->child[dir] = PTR_SHORTEN(node);


    // Update the balance factors along the path from the first
    // non-zero ancenstor to the new node
    tmp = unbal;
    while (tmp != node)
    {
        if ((path & 1) == 0)
        {
            tmp->balance -= 1;
        } // if
        else
        {
            tmp->balance += 1;
        } // else

        tmp = vrd_deref(tree->alloc, PTR_EXPAND(tmp->child[path & 1]));
        path >>= 1;
    } // while


    // Do the rotations if necessary
    struct AVL_Node* restrict root = NULL;
    if (-2 == unbal->balance)
    {
        struct AVL_Node* const restrict child =
            vrd_deref(tree->alloc, PTR_EXPAND(unbal->child[LEFT]));
        if (-1 == child->balance)
        {
            root = child;
            unbal->child[LEFT] = child->child[RIGHT];
            child->child[RIGHT] = PTR_SHORTEN(unbal);
            child->balance = 0;
            unbal->balance = 0;
        } // if
        else
        {
            root = vrd_deref(tree->alloc, PTR_EXPAND(child->child[RIGHT]));
            child->child[RIGHT] = root->child[LEFT];
            root->child[LEFT] = PTR_SHORTEN(child);
            unbal->child[LEFT] = root->child[RIGHT];
            root->child[RIGHT] = PTR_SHORTEN(unbal);
            if (-1 == root->balance)
            {
                child->balance = 0;
                unbal->balance = 1;
            } // if
            else if (0 == root->balance)
            {
                child->balance = 0;
                unbal->balance = 0;
            } // if
            else
            {
                child->balance = -1;
                unbal->balance = 0;
            } // else
            root->balance = 0;
        } // else
    } // if
    else if (2 == unbal->balance)
    {
        struct AVL_Node* const restrict child =
            vrd_deref(tree->alloc, PTR_EXPAND(unbal->child[RIGHT]));
        if (1 == child->balance)
        {
            root = child;
            unbal->child[RIGHT] = child->child[LEFT];
            child->child[LEFT] = PTR_SHORTEN(unbal);
            child->balance = 0;
            unbal->balance = 0;
        } // if
        else
        {
            root = vrd_deref(tree->alloc, PTR_EXPAND(child->child[LEFT]));
            child->child[LEFT] = root->child[RIGHT];
            root->child[RIGHT] = PTR_SHORTEN(child);
            unbal->child[RIGHT] = root->child[LEFT];
            root->child[LEFT] = PTR_SHORTEN(unbal);
            if (1 == root->balance)
            {
                child->balance = 0;
                unbal->balance = -1;
            } // if
            else if(0 == root->balance)
            {
                child->balance = 0;
                unbal->balance = 0;
            } // if
            else
            {
                child->balance = 1;
                unbal->balance = 0;
            } // else
            root->balance = 0;
        } // else
    } // if
    else
    {
        return 0;
    } // else

    unbal_par->child[unbal != vrd_deref(tree->alloc, PTR_EXPAND(unbal_par->child[LEFT]))] = PTR_SHORTEN(root);

    return 0;


#undef PTR_EXPAND
#undef PTR_SHORTEN
} // vrd_avl_insert
