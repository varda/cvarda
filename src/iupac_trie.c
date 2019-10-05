#include <assert.h>     // assert
#include <ctype.h>      // isprint
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // SIZE_MAX
#include <stdlib.h>     // malloc, free

#include "../include/iupac_trie.h"  // vrd_IUPAC_Trie, vrd_iupac_trie_*


enum
{
    NULLPTR = 0,

    IUPAC_SIZE = 16
}; // constants


struct Node
{
    void* data;
    size_t child[IUPAC_SIZE];
}; // Node


struct vrd_IUPAC_Trie
{
    size_t root;

    size_t next;
    size_t capacity;
    struct Node nodes[];
}; // vrd_IUPAC_Trie


static size_t
node_init(vrd_IUPAC_Trie* const trie)
{
    assert(NULL != trie);

    if (SIZE_MAX == trie->next || trie->capacity < trie->next)
    {
        return NULLPTR;
    } // if

    size_t const ptr = trie->next;
    trie->next += 1;

    trie->nodes[ptr].data = NULL;
    for (size_t i = 0; i < IUPAC_SIZE; ++i)
    {
        trie->nodes[ptr].child[i] = NULLPTR;
    } // for

    return ptr;
} // node_init


vrd_IUPAC_Trie*
vrd_iupac_trie_init(size_t const capacity)
{
    // FIXME: overflow on capacity
    vrd_IUPAC_Trie* const trie = malloc(sizeof(*trie) +
                                        sizeof(trie->nodes[0]) *
                                        (capacity + 1));
    if (NULL == trie)
    {
        return NULL;
    } // if

    trie->next = 0;
    trie->capacity = capacity;
    trie->root = node_init(trie);

    return trie;
} // vrd_iupac_trie_init


void
vrd_iupac_trie_destroy(vrd_IUPAC_Trie* restrict* const trie)
{
    if (NULL != trie)
    {
        free(*trie);
        *trie = NULL;
    } // if
} // vrd_iupac_trie_destroy


void*
vrd_iupac_trie_insert(vrd_IUPAC_Trie* const restrict trie,
                      size_t const len,
                      char const str[len],
                      void* restrict data)
{
    assert(NULL != trie);

    size_t tmp = trie->root;
    for (size_t i = 0; i < len; ++i)
    {
        size_t const idx = vrd_iupac_to_idx(str[i]);

        if (NULLPTR == trie->nodes[tmp].child[idx])
        {
            trie->nodes[tmp].child[idx] = node_init(trie);
            if (NULLPTR == trie->nodes[tmp].child[idx])
            {
                return NULL;
            } // if
        } // if

        tmp = trie->nodes[tmp].child[idx];
    } // for

    trie->nodes[tmp].data = data;
    return &trie->nodes[tmp];
} // vrd_iupac_trie_insert


void*
vrd_iupac_trie_find(vrd_IUPAC_Trie const* const trie,
                    size_t const len,
                    char const str[len])
{
    assert(NULL != trie);

    size_t tmp = trie->root;
    for (size_t i = 0; i < len; ++i)
    {
        size_t const idx = vrd_iupac_to_idx(str[i]);
        if (NULLPTR == trie->nodes[tmp].child[idx])
        {
            return NULL;
        } // if

        tmp = trie->nodes[tmp].child[idx];
    } // for

    return trie->nodes[tmp].data;
} // vrd_iupac_trie_find
