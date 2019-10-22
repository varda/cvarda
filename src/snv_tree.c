#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX, uint32_t, int32_t, uint64_t,
#include <stdlib.h>     // malloc, free

#include "../include/avl_tree.h"    // vrd_AVL_Tree,
                                    // vrd_avl_tree_is_element
#include "../include/snv_tree.h"    // vrd_SNV_*, vrd_snv_tree_*
#include "tree.h"   // NULLPTR, LEFT, RIGHT, HOMOZYGOUS


struct vrd_SNV_Node
{
    uint32_t child[2];
    uint32_t position;
    int32_t  balance   :  3;    // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t sample_id : 29;
    uint32_t phase     : 28;
    uint32_t inserted  :  4;    // [0, ..., 15]
}; // vrd_SNV_Node


struct vrd_SNV_Tree
{
    uint32_t root;

    uint32_t next;
    uint32_t capacity;
    vrd_SNV_Node nodes[];
}; // vrd_SNV_Tree


vrd_SNV_Tree*
vrd_snv_tree_init(size_t const capacity)
{
    if ((size_t) UINT32_MAX <= capacity)
    {
        return NULL;
    } // if

    vrd_SNV_Tree* const tree = malloc(sizeof(vrd_SNV_Tree) +
                                      sizeof(vrd_SNV_Node) *
                                      ((size_t) capacity + 1));
    if (NULL == tree)
    {
        return NULL;
    } // if

    tree->root = NULLPTR;
    tree->next = 1;
    tree->capacity = capacity;

    return tree;
} // vrd_snv_tree_init


void
vrd_snv_tree_destroy(vrd_SNV_Tree* restrict* const tree)
{
    if (NULL != tree)
    {
        free(*tree);
        *tree = NULL;
    } // if
} // vrd_snv_tree_destroy


static vrd_SNV_Node*
insert(vrd_SNV_Tree* tree, uint32_t const ptr)
{
    assert(NULL != tree);

#define KEY position
#include "tree_insert.inc"
#undef KEY
} // insert


vrd_SNV_Node*
vrd_snv_tree_insert(vrd_SNV_Tree* const tree,
                    size_t const position,
                    size_t const sample_id,
                    size_t const phase,
                    size_t const inserted)
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
    tree->nodes[ptr].position = position;
    tree->nodes[ptr].balance = 0;
    tree->nodes[ptr].sample_id = sample_id;
    tree->nodes[ptr].phase = phase;
    tree->nodes[ptr].inserted = inserted;

    return insert(tree, ptr);
} // vrd_snv_tree_insert


static size_t
query_contains(vrd_SNV_Tree const* const restrict tree,
               uint32_t const root,
               size_t const position,
               size_t const inserted,
               vrd_AVL_Tree const* const restrict subset)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    if (tree->nodes[root].position > position)
    {
        return query_contains(tree, tree->nodes[root].child[LEFT], position, inserted, subset);
    } // if

    if (tree->nodes[root].position < position)
    {
        return query_contains(tree, tree->nodes[root].child[RIGHT], position, inserted, subset);
    } // if

    size_t res = 0;
    // TODO: IUPAC match on inserted
    if (tree->nodes[root].inserted == inserted &&
        (NULL == subset || vrd_avl_tree_is_element(subset, tree->nodes[root].sample_id)))
    {
        res = 1;
        if (HOMOZYGOUS == tree->nodes[root].phase)
        {
            res = 2;
        } // if
    } // if

    return res + query_contains(tree, tree->nodes[root].child[LEFT], position, inserted, subset) +
                 query_contains(tree, tree->nodes[root].child[RIGHT], position, inserted, subset);
} // query_contains


size_t
vrd_snv_tree_query(vrd_SNV_Tree const* const restrict tree,
                   size_t const position,
                   size_t const inserted,
                   vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != tree);

    return query_contains(tree, tree->root, position, inserted, subset);
} // vrd_snv_tree_query





#include <inttypes.h>   // PRId32, PRIu32
#include <stdio.h>  // stderr, fprintf


void
print(FILE* restrict stream,
      vrd_SNV_Tree const* const restrict tree,
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
    (void) fprintf(stream, "%*s%" PRIu32 " (%2" PRId32 ")\n", indent, "", tree->nodes[root].position, tree->nodes[root].balance);
    print(stream, tree, tree->nodes[root].child[LEFT], indent + INDENT);
} // print


static inline int
ilog2(size_t const x)
{
    return 64 - __builtin_clzll(x);
} // ilog2


static inline size_t
ipow2(int const x)
{
    return (1ull << x);
} // ipow2


void
compress(vrd_SNV_Tree* const tree, size_t const count)
{
    uint32_t tmp = tree->nodes[tree->root].child[LEFT];
    tree->nodes[tree->root].child[LEFT] = tree->nodes[tmp].child[RIGHT];
    tree->nodes[tmp].child[RIGHT] = tree->root;
    tree->root = tmp;

    tmp = tree->root;
    for (size_t i = 1; i < count; ++i)
    {
        uint32_t const p = tree->nodes[tmp].child[LEFT];
        uint32_t const q = tree->nodes[p].child[LEFT];

        tree->nodes[p].child[LEFT] = tree->nodes[q].child[RIGHT];
        tree->nodes[q].child[RIGHT] = p;
        tree->nodes[tmp].child[LEFT] = q;
        tmp = q;
    } // for
} // compress


static int
update_avl(vrd_SNV_Tree* const tree, uint32_t const root)
{
    if (NULLPTR == root)
    {
        return 0;
    } // if

    int const left = update_avl(tree, tree->nodes[root].child[LEFT]);
    int const right = update_avl(tree, tree->nodes[root].child[RIGHT]);
    tree->nodes[root].balance = right - left;
    return 1 + max(left, right);
} // update_avl


static void
balance(vrd_SNV_Tree* const tree)
{
    uint32_t par = tree->root;
    uint32_t tmp = tree->root;

    // tree to vine
    size_t count = 0;
    while (NULLPTR != tmp)
    {
        fprintf(stderr, "tmp (%u): %d\n", tmp , tree->nodes[tmp].position);
        if (NULLPTR == tree->nodes[tmp].child[RIGHT])
        {
            fprintf(stderr, "tmp has no right child; traverse left\n");
            par = tmp;
            tmp = tree->nodes[tmp].child[LEFT];
            count += 1;
        } // if
        else
        {
            fprintf(stderr, "tmp has a right child\n");
            uint32_t const right = tree->nodes[tmp].child[RIGHT];
            fprintf(stderr, "right (%u): %d\n", right, tree->nodes[right].position);
            tree->nodes[tmp].child[RIGHT] = tree->nodes[right].child[LEFT];
            tree->nodes[right].child[LEFT] = tmp;
            if (tmp == tree->root)
            {
                tree->root = right;
                par = right;
            } // if
            else
            {
                tree->nodes[par].child[LEFT] = right;
            } // else
            tmp = right;
            print(stderr, tree, tree->root, 0);
            fprintf(stderr, "par (%u): %d\n", par, tree->nodes[par].position);
            fprintf(stderr, "tmp (%u): %d\n", tmp, tree->nodes[tmp].position);
        } // else
    } // while

    // vine to tree
    size_t const leaves = count - (ipow2(ilog2(count + 1) - 1) - 1);
    if (0 < leaves)
    {
        fprintf(stderr, "LEAVES: %zu\n", leaves);
        compress(tree, leaves);
    } // if

    fprintf(stderr, "REM: %zu\n", count - leaves);
    size_t rem = count - leaves;
    while (1 < rem)
    {
        rem /= 2;
        compress(tree, rem);
    } // while

    update_avl(tree, tree->root);


} // balance


// Adapted from:
// http://adtinfo.org/libavl.html/Deleting-from-an-AVL-Tree.html
void
remove_node(vrd_SNV_Tree* const tree,
            int const depth,
            uint64_t const path)
{
    uint32_t nodes[64] = {NULLPTR};
    int dir[64] = {LEFT};
    uint32_t tmp = tree->root;
    for (int i = 0; i < depth; ++i)
    {
        (void) fprintf(stderr, "%d ", tree->nodes[tmp].position);
        nodes[i] = tmp;
        dir[i] = ((path >> (depth - i - 1)) & RIGHT);
        tmp = tree->nodes[tmp].child[((path >> (depth - i - 1)) & RIGHT)];
    } // for
    (void) fprintf(stderr, ": %d\n", tree->nodes[tmp].position);

    for (int i = 0; i < depth; ++i)
    {
        (void) fprintf(stderr, "%d ", dir[i]);
    } // for
    (void) fprintf(stderr, "\n");

    int len = depth;

    if (NULLPTR == tree->nodes[tmp].child[RIGHT])
    {
        if (0 == depth)
        {
            tree->root = tree->nodes[tmp].child[LEFT];
        } // if
        else
        {
            (void) fprintf(stderr, "No right child: update %d\n", tree->nodes[nodes[depth - 1]].position);
            tree->nodes[nodes[depth - 1]].child[dir[depth - 1]] = tree->nodes[tmp].child[LEFT];
        } // else
    } // if
    else
    {
        uint32_t right = tree->nodes[tmp].child[RIGHT]; // right sub tree

        if (NULLPTR == tree->nodes[right].child[LEFT])
        {
            (void) fprintf(stderr, "Right child has no left child\n");

            tree->nodes[right].child[LEFT] = tree->nodes[tmp].child[LEFT];
            tree->nodes[right].balance = tree->nodes[tmp].balance;

            if (0 == depth)
            {
                tree->root = right;
            } // if
            else
            {
                (void) fprintf(stderr, "Update %d\n", tree->nodes[nodes[depth - 1]].position);
                tree->nodes[nodes[depth - 1]].child[dir[depth - 1]] = right;
            } // else
            dir[len] = RIGHT;
            nodes[len] = right;
            len += 1;
        } // if
        else
        {
            (void) fprintf(stderr, "Find inorder successor\n");
            uint32_t suc = NULLPTR; // find the inorder successor

            len += 1;
            while (1)
            {
                dir[len] = LEFT;
                nodes[len] = right;
                len += 1;
                suc = tree->nodes[right].child[LEFT];
                if (NULLPTR == tree->nodes[suc].child[LEFT])
                {
                    break;
                } // if
                right = suc;
            } // while

            (void) fprintf(stderr, "Inorder successor: %d\n", tree->nodes[suc].position);

            tree->nodes[suc].child[LEFT] = tree->nodes[tmp].child[LEFT];
            tree->nodes[right].child[LEFT] = tree->nodes[suc].child[RIGHT];
            tree->nodes[suc].child[RIGHT] = tree->nodes[tmp].child[RIGHT];
            tree->nodes[suc].balance = tree->nodes[tmp].balance;

            if (0 == depth)
            {
                tree->root = suc;
            } // if
            else
            {
                (void) fprintf(stderr, "Update %d\n", tree->nodes[nodes[depth - 1]].position);
                tree->nodes[nodes[depth - 1]].child[dir[depth - 1]] = suc;
            } // else
            dir[depth] = RIGHT;
            nodes[depth] = suc;
        } // else
    } // else

    return;


    // Rebalancing
    while (0 < --len)
    {
        uint32_t const unbal = nodes[len];
        if (dir[len] == LEFT)
        {
            tree->nodes[unbal].balance += 1;
            if (1 == tree->nodes[unbal].balance)
            {
                break;
            } // if
            else if (2 == tree->nodes[unbal].balance)
            {
                uint32_t const child = tree->nodes[unbal].child[RIGHT];
                if (-1 == tree->nodes[child].balance)
                {
                    uint32_t const root = tree->nodes[child].child[LEFT];
                    tree->nodes[child].child[LEFT] = tree->nodes[root].child[RIGHT];
                    tree->nodes[root].child[RIGHT] = child;
                    tree->nodes[unbal].child[RIGHT] = tree->nodes[root].child[LEFT];
                    tree->nodes[root].child[LEFT] = unbal;
                    if (1 == tree->nodes[root].balance)
                    {
                        tree->nodes[child].balance = 0;
                        tree->nodes[unbal].balance = -1;
                    } // if
                    else if (0 == tree->nodes[root].balance)
                    {
                        tree->nodes[child].balance = tree->nodes[unbal].balance = 0;
                    }
                    else // if (-1 == tree->nodes[root].balance)
                    {
                        tree->nodes[child].balance = 1,
                        tree->nodes[unbal].balance = 0;
                    } // else
                    tree->nodes[root].balance = 0;
                    tree->nodes[nodes[len - 1]].child[dir[len - 1]] = root;
                } // if
                else
                {
                    tree->nodes[unbal].child[RIGHT] = tree->nodes[child].child[LEFT];
                    tree->nodes[child].child[LEFT] = unbal;
                    tree->nodes[nodes[len - 1]].child[dir[len - 1]] = child;
                    if (0 == tree->nodes[child].balance)
                    {
                        tree->nodes[child].balance = -1;
                        tree->nodes[unbal].balance = 1;
                        break;
                    } // if
                    else
                    {
                        tree->nodes[child].balance = tree->nodes[unbal].balance = 0;
                    } // else
                } // else
            } // if
        } // if
        else // RIGHT
        {
            tree->nodes[unbal].balance -= 1;
            if (-1 == tree->nodes[unbal].balance)
            {
                break;
            } // if
            else if (-2 == tree->nodes[unbal].balance)
            {
                uint32_t const child = tree->nodes[unbal].child[LEFT];
                if (1 == tree->nodes[child].balance)
                {
                    uint32_t const root = tree->nodes[child].child[RIGHT];
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
                        tree->nodes[child].balance = tree->nodes[unbal].balance = 0;
                    } // if
                    else // if (1 == tree->nodes[root].balance)
                    {
                        tree->nodes[child].balance = -1;
                        tree->nodes[unbal].balance = 0;
                    } // else
                    tree->nodes[root].balance = 0;
                    tree->nodes[nodes[len - 1]].child[dir[len - 1]] = root;
                } // if
                else
                {
                    tree->nodes[unbal].child[LEFT] = tree->nodes[child].child[RIGHT];
                    tree->nodes[child].child[RIGHT] = unbal;
                    tree->nodes[nodes[len - 1]].child[dir[len - 1]] = child;
                    if (0 == tree->nodes[child].balance)
                    {
                        tree->nodes[child].balance = 1;
                        tree->nodes[unbal].balance = -1;
                        break;
                    } // if
                    else
                    {
                        tree->nodes[child].balance = tree->nodes[unbal].balance = 0;
                    } // else
                } // else
            } // if
        } // else
    } // while

} // remove_node


void
traverse(vrd_SNV_Tree* const tree,
         uint32_t const root,
         int const depth,
         uint64_t const path,
         size_t const sample_id)
{
    if (NULLPTR == root)
    {
        return;
    } // if

    traverse(tree, tree->nodes[root].child[LEFT], depth + 1, (path << 1) + LEFT, sample_id);

    (void) fprintf(stderr, "visit: %d\n", tree->nodes[root].position);

    if (sample_id == tree->nodes[root].position)
    {
        remove_node(tree, depth, path);
    } // if

    traverse(tree, tree->nodes[root].child[RIGHT], depth + 1, (path << 1) + RIGHT, sample_id);

} // traverse


size_t
vrd_snv_tree_remove(vrd_SNV_Tree* const tree, size_t const sample_id)
{
    assert(NULL != tree);

    print(stderr, tree, tree->root, 0);

    traverse(tree, tree->root, 0, 0, sample_id);
    balance(tree);


    print(stderr, tree, tree->root, 0);

    return 0;
} // vrd_snv_tree_remove
