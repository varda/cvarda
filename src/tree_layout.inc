#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // uint32_t
#include <stdlib.h>     // malloc, free

#include "tree.h"   // NULLPTR, LEFT, RIGHT, umax, ilog2, ipow2


static inline size_t
bittest(size_t const n, size_t const i)
{
    if (n & (1 << i))
    {
        return 1;
    } // if
    return 0;
} // bittest


static uint32_t
van_emde_boas(TREE* const restrict tree,
              uint32_t next,
              uint32_t addr[restrict],
              uint32_t const root,
              int const height)
{
    if (0 == height)
    {
        return next;
    } // if

    if (1 == height)
    {
        addr[next] = root;
        next += 1;
        return next;
    } // if

    if (2 == height)
    {
        addr[next] = root;
        next += 1;
        if (NULLPTR != tree->nodes[root].child[LEFT])
        {
            addr[next] = tree->nodes[root].child[LEFT];
            next += 1;
        } // if
        if (NULLPTR != tree->nodes[root].child[RIGHT])
        {
            addr[next] = tree->nodes[root].child[RIGHT];
            next += 1;
        } // if
        return next;
    } // if

    int const height_bottom = ipow2((ilog2(height / 2 + 1)) - 1);
    int const height_top = height - height_bottom;

    next = van_emde_boas(tree, next, addr, root, height_top);

    for (size_t i = 0; i < ipow2(height_top); ++i)
    {
        uint32_t sub = root;
        for (int j = 0; j < height_top; ++j)
        {
            sub = tree->nodes[sub].child[bittest(i, height_top - j - 1)];
            if (NULLPTR == sub)
            {
                break;
            } // if
        } // for
        if (NULLPTR != sub)
        {
            next = van_emde_boas(tree, next, addr, sub, height_bottom);
        } // if
    } // for
    return next;
} // van_emde_boas


static int
height(TREE* const tree, uint32_t const root)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    return umax(height(tree, tree->nodes[root].child[LEFT]),
                height(tree, tree->nodes[root].child[RIGHT])) + 1;
} // height


#include <stdio.h>


static void
reorder(TREE* const tree)
{
    uint32_t* const restrict addr = malloc(tree->next * sizeof(*addr));
    if (NULL == addr)
    {
        return;
    } // if

    uint32_t const size = van_emde_boas(tree, 1, addr, tree->root, height(tree, tree->root));


    uint32_t* const restrict addr_inv = malloc(size * sizeof(*addr_inv));
    if (NULL == addr_inv)
    {
        free(addr);
        return;
    } // if

    for (size_t i = 1; i < size; ++i)
    {
        addr_inv[addr[i]] = i;
    } // for

    NODE* const restrict nodes = malloc(size * sizeof(*nodes));
    if (NULL == nodes)
    {
        free(addr);
        free(addr_inv);
        return;
    } // if

    for (size_t i = 1; i < size; ++i)
    {
        uint32_t const left = tree->nodes[addr[i]].child[LEFT];
        uint32_t const right = tree->nodes[addr[i]].child[RIGHT];

        nodes[i] = tree->nodes[addr[i]];
        nodes[i].child[LEFT] = NULLPTR != left ? addr_inv[left] : NULLPTR;
        nodes[i].child[RIGHT] = NULLPTR != right ? addr_inv[right] : NULLPTR;
    } // for

    for (size_t i = 1; i < size; ++i)
    {
        tree->nodes[i] = nodes[i];
    } // for
    tree->next = size;
    tree->root = size > 1 ? 1 : NULLPTR;

    free(addr);
    free(addr_inv);
    free(nodes);
} // reorder
