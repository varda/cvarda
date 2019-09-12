#include <stddef.h>     // size_t
#include <stdint.h>     // SIZE_MAX
#include <stdlib.h>     // NULL, malloc, free

#include "../include/trie.h"    // vrd_trie_*


enum
{
    MAX_ALPHA_SIZE = 128
}; // constants


struct Trie_Node
{
    size_t idx;
    struct Trie_Node* restrict child[];
}; // Trie_Node


struct Trie
{
    struct Trie_Node* restrict root;
    size_t alpha_size;
    size_t (*to_index)(char const);
}; // Trie


static struct Trie_Node*
trie_init_node(size_t const size)
{
    // Overflow prevented by sensible values of MAX_ALPHA_SIZE
    struct Trie_Node* const restrict node = malloc(sizeof(*node) + size * sizeof(node->child[0]));
    if (NULL == node)
    {
        return NULL;
    } // if

    node->idx = -1;
    for (size_t i = 0; i < size; ++i)
    {
        node->child[i] = NULL;
    } // for
    return node;
} // trie_init_node


static void
trie_destroy_node(struct Trie_Node* const restrict root,
                  size_t const size)
{
    if (NULL == root)
    {
        return;
    } // if

    for (size_t i = 0; i < size; ++i)
    {
        trie_destroy_node(root->child[i], size);
    } // for
    free(root);
} // trie_destroy_node


struct Trie*
vrd_trie_init(size_t const alpha_size, size_t (*to_index)(char const))
{
    if (MAX_ALPHA_SIZE < alpha_size || 1 > alpha_size)
    {
        return NULL;
    } // if

    struct Trie* const restrict trie = malloc(sizeof(*trie));
    if (NULL == trie)
    {
        return NULL;
    } // if

    trie->root = trie_init_node(alpha_size);
    if (NULL == trie->root)
    {
        free(trie);
        return NULL;
    } // if

    trie->alpha_size = alpha_size;
    trie->to_index = to_index;
    return trie;
} // vrd_trie_init


void
vrd_trie_destroy(struct Trie* restrict* const restrict trie)
{
    if (NULL == trie || *trie == NULL)
    {
        return;
    } // if

    trie_destroy_node((*trie)->root, (*trie)->alpha_size);
    free(*trie);
    *trie = NULL;
} // vrd_trie_destroy


size_t
vrd_trie_insert(vrd_Trie* const restrict trie,
                size_t const len,
                char const str[len],
                size_t const idx)
{
    if (NULL == trie)
    {
        return -1;
    } // if

    struct Trie_Node* restrict tmp = trie->root;
    if (NULL == tmp)
    {
        return -1;
    } // if

    for (size_t i = 0; i < len; ++i)
    {
        size_t const j = trie->to_index(str[i]);
        if (MAX_ALPHA_SIZE <= j)
        {
            return -1;
        } // if

        if (NULL == tmp->child[j])
        {
            tmp->child[j] = trie_init_node(trie->alpha_size);
            if (NULL == tmp->child[j])
            {
                return -1;
            } // if
        } // if

        tmp = tmp->child[j];
    } // for

    tmp->idx = idx;
    return tmp->idx;
} // vrd_trie_insert


size_t
rvd_trie_find(struct Trie const* const restrict trie,
              size_t const len,
              char const str[restrict len])
{
    if (NULL == trie)
    {
        return -1;
    } // if

    struct Trie_Node* restrict tmp = trie->root;
    if (NULL == tmp)
    {
        return -1;
    } // if

    for (size_t i = 0; i < len; ++i)
    {
        size_t const j = trie->to_index(str[i]);
        if (MAX_ALPHA_SIZE <= j || NULL == tmp->child[j])
        {
            return -1;
        } // if
    } // for
    return tmp->idx;
} // vrd_trie_find
