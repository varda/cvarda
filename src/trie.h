#ifndef VRD_TRIE_H
#define VRD_TRIE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t


typedef struct vrd_Trie vrd_Trie;


vrd_Trie*
vrd_trie_init(void);


void
vrd_trie_destroy(vrd_Trie* restrict* const self);


void*
vrd_trie_insert(vrd_Trie* const restrict self,
                size_t const len,
                char const key[len],
                void* const restrict data);


void
vrd_trie_remove(vrd_Trie* const restrict self,
                size_t const len,
                char const key[len]);


void*
vrd_trie_find(vrd_Trie const* const self,
              size_t const len,
              char const key[len]);


size_t
vrd_trie_key(void* const restrict ptr, char** restrict key);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
