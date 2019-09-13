#include <stddef.h>     // size_t
#include <stdlib.h>     // malloc, free

#include "../include/alloc.h"   // vrd_Alloc, vrd_alloc, vrd_dealloc,
                                // vrd_deref
#include "../include/trie.h"    // vrd_trie_*


enum
{
    MAX_ALPHA_SIZE = 128
}; // constants


struct Trie_Node
{
    void* restrict data;
    struct Trie_Node* restrict child[];
}; // Trie_Node


struct Trie
{
    struct Trie_Node* restrict root;
    vrd_Alloc* restrict alloc;
    size_t alpha_size;
    size_t (*to_index)(char const);
}; // Trie


static struct Trie_Node*
trie_init_node(vrd_Alloc* const restrict alloc,
               size_t const alpha_size)
{
    // Overflow prevented by sensible values of MAX_ALPHA_SIZE
    void* const restrict ptr = vrd_alloc(alloc, sizeof(struct Trie_Node) + alpha_size * sizeof(void*));
    if (NULL == ptr)
    {
        return NULL;
    } // if

    struct Trie_Node* const restrict node = vrd_deref(alloc, ptr);

    node->data = NULL;
    for (size_t i = 0; i < alpha_size; ++i)
    {
        node->child[i] = NULL;
    } // for

    return ptr;
} // trie_init_node


static void
trie_destroy_node(vrd_Alloc* const restrict alloc,
                  struct Trie_Node* const restrict root,
                  size_t const alpha_size)
{
    if (NULL == root)
    {
        return;
    } // if

    for (size_t i = 0; i < alpha_size; ++i)
    {
        struct Trie_Node* const restrict node = vrd_deref(alloc, root);
        trie_destroy_node(alloc,
                          node->child[i],
                          alpha_size);
    } // for
    vrd_dealloc(alloc, root);
} // trie_destroy_node


vrd_Trie*
vrd_trie_init(vrd_Alloc* const restrict alloc,
              size_t const alpha_size,
              size_t (*to_index)(char const))
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

    trie->root = trie_init_node(alloc, alpha_size);
    if (NULL == trie->root)
    {
        free(trie);
        return NULL;
    } // if

    trie->alloc = alloc;
    trie->alpha_size = alpha_size;
    trie->to_index = to_index;

    return trie;
} // vrd_trie_init


void
vrd_trie_destroy(struct Trie* restrict* const restrict trie)
{
    if (NULL == trie || NULL == *trie)
    {
        return;
    } // if

    trie_destroy_node((*trie)->alloc,
                      (*trie)->root,
                      (*trie)->alpha_size);
    free(*trie);
    *trie = NULL;
} // vrd_trie_destroy


void*
vrd_trie_insert(struct Trie* const restrict trie,
                size_t const len,
                char const str[len],
                void* const restrict data)
{
    if (NULL == trie)
    {
        return NULL;
    } // if

    struct Trie_Node* restrict tmp = trie->root;

    for (size_t i = 0; i < len; ++i)
    {
        size_t const idx = trie->to_index(str[i]);
        if (MAX_ALPHA_SIZE <= idx)
        {
            return NULL;
        } // if

        struct Trie_Node* const restrict node =
            vrd_deref(trie->alloc, tmp);

        if (NULL == node->child[idx])
        {
            node->child[idx] = trie_init_node(trie->alloc,
                                              trie->alpha_size);
            if (NULL == node->child[idx])
            {
                return NULL;
            } // if
        } // if

        tmp = node->child[idx];
    } // for

    struct Trie_Node* const restrict node = vrd_deref(trie->alloc, tmp);
    node->data = data;

    return node->data;
} // vrd_trie_insert
