#include "../include/template.h"    // VRD_TEMPLATE
#ifndef VRD_TYPENAME
#error "Undefined template typename"
#endif


#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stdbool.h>    // true
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX, uint32_t, uint64_t
#include <stdio.h>      // FILE, fread, fwrite
#include <stdlib.h>     // free, malloc

#include "imath.h"  // ilog2, ipow2, umax, bittest
#include "tree.h"   // NULLPTR, LEFT, RIGHT, vrd_Tree


struct VRD_TEMPLATE(VRD_TYPENAME, _Tree)
{
    vrd_Tree base;
    uint32_t root;

    uint32_t capacity;
    uint32_t next;
    struct VRD_TEMPLATE(VRD_TYPENAME, _Node) nodes[];
}; // vrd_*_Tree


VRD_TEMPLATE(VRD_TYPENAME, _Tree)*
VRD_TEMPLATE(VRD_TYPENAME, _tree_init)(size_t const capacity)
{
    if ((size_t) UINT32_MAX <= capacity)
    {
        errno = -1;
        return NULL;
    } // if

    VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const tree = malloc(sizeof(*tree) + sizeof(tree->nodes[0]) * (capacity + 1));
    if (NULL == tree)
    {
        return NULL;
    } // if

    tree->root = NULLPTR;
    tree->next = 1;  // we skip the 0th element as we use 0 as NULL pointer
    tree->capacity = capacity;

    tree->base.entries = 0;
    tree->base.entry_size = sizeof(tree->nodes[0]);
    tree->base.height = 0;

    return tree;
} // vrd_*_tree_init


void
VRD_TEMPLATE(VRD_TYPENAME, _tree_destroy)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)** const self)
{
    if (NULL == self)
    {
        return;
    } // if

    free(*self);
    *self = NULL;
} // vrd_*_tree_destroy


#ifdef VRD_INTERVAL
static inline uint32_t
update_max(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self, uint32_t const root)
{
    uint32_t res = self->nodes[root].max;
    if (NULLPTR != self->nodes[root].child[LEFT])
    {
        res = umax(res, self->nodes[self->nodes[root].child[LEFT]].max);
    } // if
    if (NULLPTR != self->nodes[root].child[RIGHT])
    {
        res = umax(res, self->nodes[self->nodes[root].child[RIGHT]].max);
    } // if
    return res;
} // update_max
#endif


// Adapted from:
// http://adtinfo.org/libavl.html/Inserting-into-an-AVL-Tree.html
static void
insert(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self, uint32_t const ptr)
{
    self->base.entries += 1;

    // This is the first node in the tree
    if (NULLPTR == self->root)
    {
        self->root = ptr;
        self->base.height = 1;
        return;
    } // if

    // limiting to height 64 becomes a problem after allocating 413 TiB
    // at the earliest; it allows for a minimum of
    // 27,777,890,035,287 nodes
    uint64_t path = 0;  // bit-path to first unbalanced ancestor
    int len = 0;    // length of the path
    unsigned int dir = 0;

    uint32_t tmp = self->root;
    uint32_t tmp_par = self->root;  // parent of tmp

    uint32_t unbal = self->root;    // first unbalanced ancestor of tmp
    uint32_t unbal_par = self->root;    // parent of unbalanced

    uint32_t height = 1;
    // Insert a new node at the BST position
    while (NULLPTR != tmp)
    {

#ifdef VRD_INTERVAL
        self->nodes[tmp].max = umax(self->nodes[tmp].max,
                                    self->nodes[ptr].end);
#endif

        if (0 != self->nodes[tmp].balance)
        {
            // this is now the first unbalanced ancestor of tmp
            unbal_par = tmp_par;
            unbal = tmp;
            path = 0;
            len = 0;
        } // if

        dir = self->nodes[ptr].key > self->nodes[tmp].key;
        if (RIGHT == dir)
        {
            path |= (uint64_t) RIGHT << len;
        } // if
        len += 1;

        height += 1;

        tmp_par = tmp;
        tmp = self->nodes[tmp].child[dir];
    } // while

    self->base.height = umax(self->base.height, height);

    self->nodes[tmp_par].child[dir] = ptr;

    // Update the balance factors along the path from the first
    // unbalanced ancestor to the new node
    tmp = unbal;
    while (tmp != ptr)
    {
        if (LEFT == (path & RIGHT))
        {
            self->nodes[tmp].balance -= 1;
        } // if
        else
        {
            self->nodes[tmp].balance += 1;
        } // else

        tmp = self->nodes[tmp].child[path & RIGHT];
        path >>= 1;
    } // while

    // Do the rotations if necessary
    uint32_t root = 0;
    if (-2 == self->nodes[unbal].balance)
    {
        uint32_t const child = self->nodes[unbal].child[LEFT];
        if (-1 == self->nodes[child].balance)
        {
            root = child;
            self->nodes[unbal].child[LEFT] = self->nodes[child].child[RIGHT];
            self->nodes[child].child[RIGHT] = unbal;
            self->nodes[child].balance = 0;
            self->nodes[unbal].balance = 0;

#ifdef VRD_INTERVAL
            self->nodes[child].max = update_max(self, child);
            self->nodes[unbal].max = update_max(self, unbal);
#endif

        } // if
        else
        {
            root = self->nodes[child].child[RIGHT];
            self->nodes[child].child[RIGHT] = self->nodes[root].child[LEFT];
            self->nodes[root].child[LEFT] = child;
            self->nodes[unbal].child[LEFT] = self->nodes[root].child[RIGHT];
            self->nodes[root].child[RIGHT] = unbal;
            if (-1 == self->nodes[root].balance)
            {
                self->nodes[child].balance = 0;
                self->nodes[unbal].balance = 1;
            } // if
            else if (0 == self->nodes[root].balance)
            {
                self->nodes[child].balance = 0;
                self->nodes[unbal].balance = 0;
            } // if
            else
            {
                self->nodes[child].balance = -1;
                self->nodes[unbal].balance = 0;
            } // else
            self->nodes[root].balance = 0;

#ifdef VRD_INTERVAL
            self->nodes[root].max = update_max(self, root);
            self->nodes[child].max = update_max(self, child);
            self->nodes[unbal].max = update_max(self, unbal);
#endif

        } // else
    } // if
    else if (2 == self->nodes[unbal].balance)
    {
        uint32_t const child = self->nodes[unbal].child[RIGHT];
        if (1 == self->nodes[child].balance)
        {
            root = child;
            self->nodes[unbal].child[RIGHT] = self->nodes[child].child[LEFT];
            self->nodes[child].child[LEFT] = unbal;
            self->nodes[child].balance = 0;
            self->nodes[unbal].balance = 0;

#ifdef VRD_INTERVAL
            self->nodes[child].max = update_max(self, child);
            self->nodes[unbal].max = update_max(self, unbal);
#endif

        } // if
        else
        {
            root = self->nodes[child].child[LEFT];
            self->nodes[child].child[LEFT] = self->nodes[root].child[RIGHT];
            self->nodes[root].child[RIGHT] = child;
            self->nodes[unbal].child[RIGHT] = self->nodes[root].child[LEFT];
            self->nodes[root].child[LEFT] = unbal;
            if (1 == self->nodes[root].balance)
            {
                self->nodes[child].balance = 0;
                self->nodes[unbal].balance = -1;
            } // if
            else if(0 == self->nodes[root].balance)
            {
                self->nodes[child].balance = 0;
                self->nodes[unbal].balance = 0;
            } // if
            else
            {
                self->nodes[child].balance = 1;
                self->nodes[unbal].balance = 0;
            } // else
            self->nodes[root].balance = 0;

#ifdef VRD_INTERVAL
            self->nodes[root].max = update_max(self, root);
            self->nodes[child].max = update_max(self, child);
            self->nodes[unbal].max = update_max(self, unbal);
#endif

        } // else
    } // if
    else
    {
        return;
    } // else

    if (self->root == unbal)
    {
        self->root = root;
        return;
    } // if

    self->nodes[unbal_par].child[unbal != self->nodes[unbal_par].child[LEFT]] = root;

    return;
} // insert


static size_t
tree_to_vine(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self)
{
    uint32_t par = self->root;
    uint32_t tmp = self->root;
    size_t count = 0;
    while (NULLPTR != tmp)
    {
        if (NULLPTR == self->nodes[tmp].child[RIGHT])
        {
            par = tmp;
            tmp = self->nodes[tmp].child[LEFT];
            count += 1;
        } // if
        else
        {
            uint32_t const right = self->nodes[tmp].child[RIGHT];
            self->nodes[tmp].child[RIGHT] = self->nodes[right].child[LEFT];
            self->nodes[right].child[LEFT] = tmp;
            if (tmp == self->root)
            {
                self->root = right;
                par = right;
            } // if
            else
            {
                self->nodes[par].child[LEFT] = right;
            } // else
            tmp = right;
        } // else
    } // while
    return count;
} // tree_to_vine


static void
compress(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self, size_t const count)
{
    uint32_t tmp = self->nodes[self->root].child[LEFT];
    self->nodes[self->root].child[LEFT] = self->nodes[tmp].child[RIGHT];
    self->nodes[tmp].child[RIGHT] = self->root;
    self->root = tmp;

    tmp = self->root;
    for (size_t i = 1; i < count; ++i)
    {
        uint32_t const p = self->nodes[tmp].child[LEFT];
        uint32_t const q = self->nodes[p].child[LEFT];

        self->nodes[p].child[LEFT] = self->nodes[q].child[RIGHT];
        self->nodes[q].child[RIGHT] = p;
        self->nodes[tmp].child[LEFT] = q;
        tmp = q;
    } // for
} // compress


// Day Stout Warren algorithm
static void
balance(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self)
{
    self->base.entries = tree_to_vine(self);
    self->base.height = 0;

    size_t const leaves = self->base.entries - (ipow2(ilog2(self->base.entries + 1) - 1) - 1);
    if (0 < leaves)
    {
        compress(self, leaves);
        self->base.height = 1;
    } // if

    size_t rem = self->base.entries - leaves;
    while (1 < rem)
    {
        rem /= 2;
        compress(self, rem);
        self->base.height += 1;
    } // while
} // balance


// Adapted from:
// http://adtinfo.org/libavl.html/Deleting-from-an-AVL-Tree.html
static void
node_remove(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
            int const depth,
            uint64_t const path)
{
    self->base.entries -= 1;

    uint32_t nodes[64] = {NULLPTR};
    int dir[64] = {LEFT};
    uint32_t tmp = self->root;
    for (int i = 0; i < depth; ++i)
    {
        nodes[i] = tmp;
        dir[i] = ((path >> (depth - i - 1)) & RIGHT);
        tmp = self->nodes[tmp].child[((path >> (depth - i - 1)) & RIGHT)];
    } // for

    int len = depth;
    if (NULLPTR == self->nodes[tmp].child[RIGHT])
    {
        if (0 == depth)
        {
            self->root = self->nodes[tmp].child[LEFT];
        } // if
        else
        {
            self->nodes[nodes[depth - 1]].child[dir[depth - 1]] = self->nodes[tmp].child[LEFT];
        } // else
    } // if
    else
    {
        uint32_t right = self->nodes[tmp].child[RIGHT]; // right sub tree

        if (NULLPTR == self->nodes[right].child[LEFT])
        {
            self->nodes[right].child[LEFT] = self->nodes[tmp].child[LEFT];
            self->nodes[right].balance = self->nodes[tmp].balance;

            if (0 == depth)
            {
                self->root = right;
            } // if
            else
            {
                self->nodes[nodes[depth - 1]].child[dir[depth - 1]] = right;
            } // else
            dir[len] = RIGHT;
            nodes[len] = right;
            len += 1;
        } // if
        else
        {
            uint32_t suc = NULLPTR; // find the inorder successor

            len += 1;
            while (true)
            {
                dir[len] = LEFT;
                nodes[len] = right;
                len += 1;
                suc = self->nodes[right].child[LEFT];
                if (NULLPTR == self->nodes[suc].child[LEFT])
                {
                    break;
                } // if
                right = suc;
            } // while

            self->nodes[suc].child[LEFT] = self->nodes[tmp].child[LEFT];
            self->nodes[right].child[LEFT] = self->nodes[suc].child[RIGHT];
            self->nodes[suc].child[RIGHT] = self->nodes[tmp].child[RIGHT];
            self->nodes[suc].balance = self->nodes[tmp].balance;

            if (0 == depth)
            {
                self->root = suc;
            } // if
            else
            {
                self->nodes[nodes[depth - 1]].child[dir[depth - 1]] = suc;
            } // else
            dir[depth] = RIGHT;
            nodes[depth] = suc;
        } // else
    } // else
} // node_remove


static size_t
traverse(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
         uint32_t const root,
         int const depth,
         uint64_t const path,
         vrd_AVL_Tree const* const subset)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    size_t count = 0;
    count += traverse(self, self->nodes[root].child[LEFT], depth + 1, (path << 1) + LEFT, subset);
    count += traverse(self, self->nodes[root].child[RIGHT], depth + 1, (path << 1) + RIGHT, subset);

    if (vrd_AVL_tree_is_element(subset, self->nodes[root].sample_id))
    {
        node_remove(self, depth, path);
        count += 1;
    } // if
    return count;
} // traverse


#ifdef VRD_INTERVAL
static int
update_avl(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self, uint32_t const root, uint32_t* const new_max)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    uint32_t max_left = 0;
    int const left = update_avl(self, self->nodes[root].child[LEFT], &max_left);
    uint32_t max_right = 0;
    int const right = update_avl(self, self->nodes[root].child[RIGHT], &max_right);
    self->nodes[root].max = umax(umax(max_left, max_right), self->nodes[root].end);
    self->nodes[root].balance = right - left;
    if (NULL != new_max)
    {
        *new_max = self->nodes[root].max;
    } // if
    return umax(left, right) + 1;
} // update_avl
#else
static int
update_avl(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self, uint32_t const root)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    int const left = update_avl(self, self->nodes[root].child[LEFT]);
    int const right = update_avl(self, self->nodes[root].child[RIGHT]);
    self->nodes[root].balance = right - left;
    return umax(left, right) + 1;
} // update_avl
#endif


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_remove)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                         vrd_AVL_Tree const* const subset)
{
    assert(NULL != self);

    size_t const count = traverse(self, self->root, 0, 0, subset);
    balance(self);

#ifdef VRD_INTERVAL
    (void) update_avl(self, self->root, NULL);
#else
    (void) update_avl(self, self->root);
#endif

    return count;
} // vrd_*_tree_remove


static uint32_t
van_emde_boas(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
              uint32_t next,
              uint32_t addr[],
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
        if (NULLPTR != self->nodes[root].child[LEFT])
        {
            addr[next] = self->nodes[root].child[LEFT];
            next += 1;
        } // if
        if (NULLPTR != self->nodes[root].child[RIGHT])
        {
            addr[next] = self->nodes[root].child[RIGHT];
            next += 1;
        } // if
        return next;
    } // if

    int const height_bottom = ipow2((ilog2(height / 2 + 1)) - 1);
    int const height_top = height - height_bottom;

    next = van_emde_boas(self, next, addr, root, height_top);

    for (size_t i = 0; i < ipow2(height_top); ++i)
    {
        uint32_t sub = root;
        for (int j = 0; j < height_top; ++j)
        {
            sub = self->nodes[sub].child[bittest(i, height_top - j - 1)];
            if (NULLPTR == sub)
            {
                break;
            } // if
        } // for
        if (NULLPTR != sub)
        {
            next = van_emde_boas(self, next, addr, sub, height_bottom);
        } // if
    } // for
    return next;
} // van_emde_boas


static int
height(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self, uint32_t const root)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    return umax(height(self, self->nodes[root].child[LEFT]),
                height(self, self->nodes[root].child[RIGHT])) + 1;
} // height


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_reorder)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self)
{
    assert(NULL != self);

    uint32_t* const addr = malloc(self->next * sizeof(*addr));
    if (NULL == addr)
    {
        return errno;
    } // if

    uint32_t const size = van_emde_boas(self, 1, addr, self->root, height(self, self->root));

    uint32_t* const addr_inv = malloc(size * sizeof(*addr_inv));
    if (NULL == addr_inv)
    {
        free(addr);
        return errno;
    } // if

    for (size_t i = 1; i < size; ++i)
    {
        addr_inv[addr[i]] = i;
    } // for

    struct VRD_TEMPLATE(VRD_TYPENAME, _Node)* const nodes = malloc(size * sizeof(*nodes));
    if (NULL == nodes)
    {
        free(addr);
        free(addr_inv);
        return errno;
    } // if

    for (size_t i = 1; i < size; ++i)
    {
        uint32_t const left = self->nodes[addr[i]].child[LEFT];
        uint32_t const right = self->nodes[addr[i]].child[RIGHT];

        nodes[i] = self->nodes[addr[i]];
        nodes[i].child[LEFT] = NULLPTR != left ? addr_inv[left] : NULLPTR;
        nodes[i].child[RIGHT] = NULLPTR != right ? addr_inv[right] : NULLPTR;
    } // for

    for (size_t i = 1; i < size; ++i)
    {
        self->nodes[i] = nodes[i];
    } // for
    self->next = size;
    self->root = size > 1 ? 1 : NULLPTR;

    free(addr);
    free(addr_inv);
    free(nodes);

    return 0;
} // vrd_*_tree_reorder


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_read)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                       FILE* stream)
{
    assert(NULL != self);
    assert(NULL != stream);

    size_t count = fread(&self->root, sizeof(self->root), 1, stream);
    if (1 != count)
    {
        return errno;
    } // if
    count = fread(&self->next, sizeof(self->next), 1, stream);
    if (1 != count)
    {
        return errno;
    } // if
    count = fread(&self->nodes[1], sizeof(self->nodes[0]), self->next - 1, stream);
    if (self->next - 1 != count)
    {
        return errno;
    } // if

    self->base.entries = self->next;
    self->base.height = height(self, self->root);

    return 0;
} // vrd_*_tree_read


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_write)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                        FILE* stream)
{
    assert(NULL != self);
    assert(NULL != stream);

    size_t count = fwrite(&self->root, sizeof(self->root), 1, stream);
    if (1 != count)
    {
        return errno;
    } // if
    count = fwrite(&self->next, sizeof(self->next), 1, stream);
    if (1 != count)
    {
        return errno;
    } // if
    count = fwrite(&self->nodes[1], sizeof(self->nodes[0]), self->next - 1, stream);
    if (self->next - 1 != count)
    {
        return errno;
    } // if

    return 0;
} // vrd_*_tree_write
