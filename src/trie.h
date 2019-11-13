#ifndef VRD_TRIE_H
#define VRD_TRIE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdbool.h>    // bool
#include <stddef.h>     // size_t


typedef struct vrd_Trie_Node
{
    void* data;
    size_t count;
} vrd_Trie_Node;


typedef struct vrd_Trie vrd_Trie;


vrd_Trie*
vrd_trie_init(void);


void
vrd_trie_destroy(vrd_Trie** const self);


vrd_Trie_Node*
vrd_trie_insert(vrd_Trie* const self,
                size_t const len,
                char const key[len],
                void* const data);


bool
vrd_trie_remove(vrd_Trie* const self,
                size_t const len,
                char const key[len]);


vrd_Trie_Node*
vrd_trie_find(vrd_Trie const* const self,
              size_t const len,
              char const key[len]);


size_t
vrd_trie_key(void* const ptr, char** key);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
