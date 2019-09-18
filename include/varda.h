#ifndef VARDA_H
#define VARDA_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stdint.h>     // UINT64_C

#include "alloc.h"      // vrd_Alloc, vrd_alloc, vrd_dealloc, vrd_deref,
                        // vrd_pool_*
#include "avl_tree.h"   // vrd_AVL_Tree, vrd_AVL_Node, VRD_AVL_NODE_SIZE,
                        // vrd_avl_*
#include "itv_tree.h"   // vrd_Itv_Tree, vrd_Itv_Node, VRD_ITV_NODE_SIZE,
                        // vrd_itv_*
#include "region_index.h"   // vrd_Region_Index, vrd_Region_Node,
                            // VRD_REGION_NODE_SIZE, vrd_region_*
#include "snv_index.h"  // vrd_SNV_Index, vrd_snv_*, VRD_SNV_NODE_SIZE
#include "trie.h"       // vrd_Trie, vrd_trie_*
#include "variant_index.h"  // vrd_Variant_Index, vrd_variant_*,
                            // VRD_VARIANT_NODE_SIZE


enum
{
    VRD_MAX_POSITION = (UINT64_C(1) << 28) - 1,
    VRD_MAX_SAMPLE_ID = (UINT64_C(1) << 29) - 1
}; // constants


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
