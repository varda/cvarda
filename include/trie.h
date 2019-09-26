#ifndef TRIE_H
#define TRIE_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <ctype.h>      // isprint
#include <stddef.h>     // size_t

#include "alloc.h"      // vrd_Alloc


enum
{
    VRD_ASCII_SIZE = 95
}; // constants


static inline size_t
vrd_ascii_to_idx(char const ch)
{
    if (isprint(ch))
    {
        return ch - ' ';
    } // if
    return VRD_ASCII_SIZE;
} // vrd_ascii_to_idx


typedef struct Trie vrd_Trie;


vrd_Trie*
vrd_trie_init(vrd_Alloc* const restrict alloc,
              size_t const alpha_size,
              size_t (*to_index)(char const));


void
vrd_trie_destroy(vrd_Trie* restrict* const restrict trie);


void*
vrd_trie_insert(vrd_Trie* const restrict trie,
                size_t const len,
                char const str[len],
                void* const restrict data);


void*
vrd_trie_find(vrd_Trie const* const restrict trie,
              size_t const len,
              char const str[len]);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
