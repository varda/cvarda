#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t, uint64_t, int64_t,
                        // UINT64_C, uintptr_t
#include <stdlib.h>     // NULL, malloc, free

#include <stdio.h>      // FIXME: DEBUG FILE*, fprintf

#include "../include/alloc.h"       // vrd_Alloc, vrd_alloc, vrd_deref
                                    // vrd_dealloc
#include "../include/itv_tree.h"    // vrd_Itv_Tree, vrd_Itv_Node,
                                    // vrd_itv_*


enum
{
    LEFT = 0,
    RIGHT = 1
}; // constants


struct Itv_Tree
{
    vrd_Alloc* restrict alloc;
    uint32_t root;
}; // Itv_Tree


static inline uint32_t
max(uint32_t const a,
    uint32_t const b)
{
    return a > b ? a : b;
} // max


static inline uint32_t
update_max(vrd_Alloc const* const alloc,
           uint32_t const root)
{
#define DEREF(ptr) ((vrd_Itv_Node*) \
vrd_deref(alloc, ((void*) (uintptr_t) ptr)))


    uint32_t res = DEREF(root)->max;
    if (0 != DEREF(root)->child[LEFT])
    {
        res = max(res, DEREF(DEREF(root)->child[LEFT])->max);
    } // if
    if (0 != DEREF(root)->child[RIGHT])
    {
        res = max(res, DEREF(DEREF(root)->child[RIGHT])->max);
    } // if
    return res;


#undef DEREF
} // update_max


static inline uintptr_t
itv_node_init(vrd_Alloc* const restrict alloc,
              uint32_t const start,
              uint32_t const end)
{
    void* const restrict ptr =
        vrd_alloc(alloc, sizeof(struct vrd_Itv_Node));
    if (NULL == ptr)
    {
        return 0;
    } // if

    vrd_Itv_Node* const restrict node = vrd_deref(alloc, ptr);
    node->child[LEFT] = 0;
    node->child[RIGHT] = 0;
    node->start = start;
    node->end = end;
    node->max = end;
    node->balance = 0;
    node->extra = 0;

    return (uintptr_t) ptr;
} // itv_node_init


static void
itv_node_destroy(vrd_Alloc* const restrict alloc,
                 uint32_t const root)
{
    if (0 == root)
    {
        return;
    } // if

    vrd_Itv_Node const* const restrict node =
        vrd_deref(alloc, (void*) (uintptr_t) root);
    itv_node_destroy(alloc, node->child[LEFT]);
    itv_node_destroy(alloc, node->child[RIGHT]);
    vrd_dealloc(alloc, (void*) (uintptr_t) root);
} // itv_node_destroy


// FIXME: DEBUG
static size_t
itv_node_print(FILE* const restrict stream,
               vrd_Alloc const* const restrict alloc,
               uint32_t const root,
               int const indent)
{
    enum
    {
        INDENT = 8
    }; // constants

    if (0 == root)
    {
        return 0;
    } // if

    vrd_Itv_Node const* const restrict node = vrd_deref(alloc, (void*) (uintptr_t) root);
    return itv_node_print(stream, alloc, node->child[RIGHT], indent + INDENT) +
           fprintf(stream, "%*s%zu--%zu [%zu] (%2d)\n", indent, "", (size_t) node->start, (size_t) node->end, (size_t) node->max, node->balance) +
           itv_node_print(stream, alloc, node->child[LEFT], indent + INDENT);
} // itv_node_print


vrd_Itv_Tree*
vrd_itv_init(vrd_Alloc* const restrict alloc)
{
    if (NULL == alloc)
    {
        return NULL;
    } // if

    struct Itv_Tree* const restrict tree = malloc(sizeof(*tree));
    if (NULL == tree)
    {
        return NULL;
    } // if

    tree->root = 0;
    tree->alloc = alloc;

    return tree;
} // vrd_itv_init


void
vrd_itv_destroy(vrd_Itv_Tree* restrict* const restrict tree)
{
    if (NULL == tree || NULL == *tree)
    {
        return;
    } // if

    itv_node_destroy((*tree)->alloc, (*tree)->root);

    free(*tree);
    *tree = NULL;
} // vrd_itv_destroy


vrd_Itv_Node*
vrd_itv_insert(vrd_Itv_Tree* const restrict tree,
               uint32_t const start,
               uint32_t const end)
{
#define DEREF(ptr) ((vrd_Itv_Node*) \
vrd_deref(tree->alloc, ((void*) (uintptr_t) ptr)))


    if (NULL == tree)
    {
        return NULL;
    } // if

    if (0 == tree->root)
    {
        tree->root = itv_node_init(tree->alloc, start, end);
        if (0 == tree->root)
        {
            return NULL;
        } // if
        return vrd_deref(tree->alloc, (void*) (uintptr_t) tree->root);
    } // if

    // limiting to height 64 becomes a problem after allocating 413 TiB
    // at the earliest; it allows for a minimum of
    // 27,777,890,035,287 nodes
    uint64_t path = 0; // bit-path to first unbalanced ancestor
    int len = 0; // length of the path
    int dir = 0;

    uint32_t tmp_par = tree->root; // parent of tmp
    uint32_t tmp = tree->root;

    uint32_t unbal = tree->root; // first unbalanced ancestor of tmp
    uint32_t unbal_par = tree->root; // parent of unbalanced

    // Insert a new node at the BST position
    while (0 != tmp)
    {
        int64_t const cmp = (int64_t) start -
                            (int64_t) DEREF(tmp)->start;

        if (0 != DEREF(tmp)->balance)
        {
            // this is now the first unbalanced ancestor of tmp
            unbal_par = tmp_par;
            unbal = tmp;
            path = 0;
            len = 0;
        } // if

        dir = cmp > 0;
        if (1 == dir)
        {
            path |= UINT64_C(1) << len;
        } // if
        len += 1;

        tmp_par = tmp;
        tmp = DEREF(tmp)->child[dir];
    } // while


    uint32_t const node = itv_node_init(tree->alloc, start, end);
    if (0 == node)
    {
        return NULL;
    } // if
    DEREF(tmp_par)->child[dir] = node;


    // Update the balance factors along the path from the first
    // unbalanced ancenstor to the new node
    tmp = unbal;
    while (tmp != node)
    {
        if (0 == (path & 1))
        {
            DEREF(tmp)->balance -= 1;
        } // if
        else
        {
            DEREF(tmp)->balance += 1;
        } // else

        tmp = DEREF(tmp)->child[path & 1];
        path >>= 1;
    } // while


    // Do the rotations if necessary
    uint32_t root = 0;
    if (-2 == DEREF(unbal)->balance)
    {
        uint32_t const child = DEREF(unbal)->child[LEFT];
        if (-1 == DEREF(child)->balance)
        {
            root = child;
            DEREF(unbal)->child[LEFT] = DEREF(child)->child[RIGHT];
            DEREF(child)->child[RIGHT] = unbal;
            DEREF(child)->balance = 0;
            DEREF(unbal)->balance = 0;
            DEREF(child)->max = update_max(tree->alloc, child);
            DEREF(unbal)->max = update_max(tree->alloc, unbal);
        } // if
        else
        {
            root = DEREF(child)->child[RIGHT];
            DEREF(child)->child[RIGHT] = DEREF(root)->child[LEFT];
            DEREF(root)->child[LEFT] = child;
            DEREF(unbal)->child[LEFT] = DEREF(root)->child[RIGHT];
            DEREF(root)->child[RIGHT] = unbal;
            if (-1 == DEREF(root)->balance)
            {
                DEREF(child)->balance = 0;
                DEREF(unbal)->balance = 1;
            } // if
            else if (0 == DEREF(root)->balance)
            {
                DEREF(child)->balance = 0;
                DEREF(unbal)->balance = 0;
            } // if
            else
            {
                DEREF(child)->balance = -1;
                DEREF(unbal)->balance = 0;
            } // else
            DEREF(root)->balance = 0;
            DEREF(root)->max = update_max(tree->alloc, root);
            DEREF(child)->max = update_max(tree->alloc, child);
            DEREF(unbal)->max = update_max(tree->alloc, unbal);
        } // else
    } // if
    else if (2 == DEREF(unbal)->balance)
    {
        uint32_t const child = DEREF(unbal)->child[RIGHT];
        if (1 == DEREF(child)->balance)
        {
            root = child;
            DEREF(unbal)->child[RIGHT] = DEREF(child)->child[LEFT];
            DEREF(child)->child[LEFT] = unbal;
            DEREF(child)->balance = 0;
            DEREF(unbal)->balance = 0;
            DEREF(child)->max = update_max(tree->alloc, child);
            DEREF(unbal)->max = update_max(tree->alloc, unbal);
        } // if
        else
        {
            root = DEREF(child)->child[LEFT];
            DEREF(child)->child[LEFT] = DEREF(root)->child[RIGHT];
            DEREF(root)->child[RIGHT] = child;
            DEREF(unbal)->child[RIGHT] = DEREF(root)->child[LEFT];
            DEREF(root)->child[LEFT] = unbal;
            if (1 == DEREF(root)->balance)
            {
                DEREF(child)->balance = 0;
                DEREF(unbal)->balance = -1;
            } // if
            else if(0 == DEREF(root)->balance)
            {
                DEREF(child)->balance = 0;
                DEREF(unbal)->balance = 0;
            } // if
            else
            {
                DEREF(child)->balance = 1;
                DEREF(unbal)->balance = 0;
            } // else
            DEREF(root)->balance = 0;
            DEREF(root)->max = update_max(tree->alloc, root);
            DEREF(child)->max = update_max(tree->alloc, child);
            DEREF(unbal)->max = update_max(tree->alloc, unbal);
        } // else
    } // if
    else
    {
        return vrd_deref(tree->alloc, (void*) (uintptr_t) node);
    } // else


    DEREF(unbal_par)->child[unbal !=
                            DEREF(unbal_par)->child[LEFT]] = root;
    DEREF(unbal_par)->max = update_max(tree->alloc, unbal_par);

    return vrd_deref(tree->alloc, (void*) (uintptr_t) node);


#undef DEREF
} // vrd_itv_insert


// FIXME: DEBUG
size_t
vrd_itv_print(FILE* const restrict stream,
              vrd_Itv_Tree const* const restrict tree)
{
    if (NULL == tree)
    {
        return 0;
    } // if
    return itv_node_print(stream, tree->alloc, tree->root, 0);
} // vrd_itv_print
