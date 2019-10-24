#include <stdint.h>     // uint32_t, uint64_t

#include "tree.h"   // NULLPTR, LEFT, RIGHT, umax


#ifdef ITV
static inline uint32_t
update_max(TREE const* const tree, uint32_t const root)
{
    uint32_t res = tree->nodes[root].max;
    if (NULLPTR != tree->nodes[root].child[LEFT])
    {
        res = umax(res, tree->nodes[tree->nodes[root].child[LEFT]].max);
    } // if
    if (NULLPTR != tree->nodes[root].child[RIGHT])
    {
        res = umax(res, tree->nodes[tree->nodes[root].child[RIGHT]].max);
    } // if
    return res;
} // update_max
#endif


// Adapted from:
// http://adtinfo.org/libavl.html/Inserting-into-an-AVL-Tree.html
static NODE*
insert(TREE* const tree, uint32_t const ptr)
{
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

    #ifdef ITV
        tree->nodes[tmp].max = umax(tree->nodes[tmp].max,
                                    tree->nodes[ptr].end);
    #endif

        if (0 != tree->nodes[tmp].balance)
        {
            // this is now the first unbalanced ancestor of tmp
            unbal_par = tmp_par;
            unbal = tmp;
            path = 0;
            len = 0;
        } // if

        dir = tree->nodes[ptr].KEY > tree->nodes[tmp].KEY;
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
    #ifdef ITV
            tree->nodes[child].max = update_max(tree, child);
            tree->nodes[unbal].max = update_max(tree, unbal);
    #endif
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
            } // if
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
    #ifdef ITV
            tree->nodes[root].max = update_max(tree, root);
            tree->nodes[child].max = update_max(tree, child);
            tree->nodes[unbal].max = update_max(tree, unbal);
    #endif
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
    #ifdef ITV
            tree->nodes[child].max = update_max(tree, child);
            tree->nodes[unbal].max = update_max(tree, unbal);
    #endif
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
    #ifdef ITV
            tree->nodes[root].max = update_max(tree, root);
            tree->nodes[child].max = update_max(tree, child);
            tree->nodes[unbal].max = update_max(tree, unbal);
    #endif
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
