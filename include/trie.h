#ifndef TRIE_H
#define TRIE_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t

#include "alloc.h"      // vrd_Alloc


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


#ifdef __cplusplus

} // extern "C"

#endif

#endif
