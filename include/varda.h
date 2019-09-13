#ifndef VARDA_H
#define VARDA_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include "alloc.h"      // vrd_Alloc, vrd_alloc, vrd_dealloc, vrd_deref,
                        // vrd_pool_*
#include "trie.h"       // vrd_trie_*


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
