#include <assert.h>     // assert
#include <ctype.h>      // isprint
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX, uint32_t
#include <stdlib.h>     // malloc, free

#include "../include/iupac_trie.h"  // vrd_IUPAC_Trie, vrd_iupac_trie_*
#include "iupac.h"   // IUPAC_SIZE, iupac_to_idx


enum
{
    NULLPTR = 0
}; // constants


struct Node
{
    void* data;
    uint32_t child[IUPAC_SIZE];
}; // Node


struct vrd_IUPAC_Trie
{
    uint32_t root;

    uint32_t next;
    uint32_t capacity;
    struct Node nodes[];
}; // vrd_IUPAC_Trie


static uint32_t
node_init(vrd_IUPAC_Trie* const trie)
{
    assert(NULL != trie);

    if (UINT32_MAX == trie->next || trie->capacity < trie->next)
    {
        return NULLPTR;
    } // if

    uint32_t const ptr = trie->next;
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
    if ((size_t) UINT32_MAX <= capacity)
    {
        return NULL;
    } // if

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

    uint32_t tmp = trie->root;
    for (size_t i = 0; i < len; ++i)
    {
        size_t const idx = iupac_to_idx(str[i]);
        if (IUPAC_SIZE <= idx)
        {
            return NULL;
        } // if

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
    return trie->nodes[tmp].data;
} // vrd_iupac_trie_insert


void*
vrd_iupac_trie_find(vrd_IUPAC_Trie const* const trie,
                    size_t const len,
                    char const str[len])
{
    assert(NULL != trie);

    uint32_t tmp = trie->root;
    for (size_t i = 0; i < len; ++i)
    {
        size_t const idx = iupac_to_idx(str[i]);
        if (IUPAC_SIZE <= idx)
        {
            return NULL;
        } // if

        if (NULLPTR == trie->nodes[tmp].child[idx])
        {
            return NULL;
        } // if

        tmp = trie->nodes[tmp].child[idx];
    } // for

    return trie->nodes[tmp].data;
} // vrd_iupac_trie_find
