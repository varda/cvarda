#ifndef VARDA_H
#define VARDA_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include "alloc.h"      // vrd_Alloc, vrd_alloc, vrd_dealloc, vrd_deref,
                        // vrd_pool_*
#include "avl_tree.h"   // vrd_AVL_Tree, vrd_AVL_Node, vrd_avl_*
#include "snv_index.h"  // vrd_SNV_Index
#include "trie.h"       // vrd_Trie, vrd_trie_*


static inline void
vrd_version(int* const restrict major,
            int* const restrict minor,
            int* const restrict patch)
{
    *major = VRD_VERSION_MAJOR;
    *minor = VRD_VERSION_MINOR;
    *patch = VRD_VERSION_PATCH;
} // vrd_version


#ifdef __cplusplus

} // extern "C"

#endif

#endif
