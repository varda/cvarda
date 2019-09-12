#ifndef TRIE_H
#define TRIE_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t


typedef struct Trie vrd_Trie;


vrd_Trie*
vrd_trie_init(size_t const alpha_size, size_t (*to_index)(char const));


void
vrd_trie_destroy(vrd_Trie* restrict* const restrict trie);


size_t
vrd_trie_insert(vrd_Trie* const restrict trie,
                size_t const len,
                char const str[len],
                size_t const idx);


size_t
rvd_trie_find(struct Trie const* const restrict trie,
              size_t const len,
              char const str[restrict len]);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
