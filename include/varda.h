#ifndef VARDA_H
#define VARDA_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#ifndef VRD_VERSION_MAJOR
#define VRD_VERSION_MAJOR 0
#endif

#ifndef VRD_VERSION_MINOR
#define VRD_VERSION_MINOR 0
#endif

#ifndef VRD_VERSION_PATCH
#define VRD_VERSION_PATCH 0
#endif


#include <stdint.h>     // UINT64_C

#include "alloc.h"      // vrd_Alloc, vrd_alloc, vrd_dealloc, vrd_deref,
                        // vrd_pool_*
#include "avl_tree.h"   // vrd_AVL_Tree, vrd_AVL_Node, VRD_AVL_NODE_SIZE,
                        // vrd_avl_*
#include "itv_tree.h"   // vrd_Itv_Tree, vrd_Itv_Node, VRD_ITV_NODE_SIZE,
                        // vrd_itv_*
#include "mnv_index.h"  // vrd_MNV_Index, vrd_mnv_index_*,
                        // VRD_MNV_NODE_SIZE
#include "mnv_table.h"  // vrd_MNV_Table, vrd_mnv_table_*
#include "region_index.h"   // vrd_Region_Index, vrd_Region_Node,
                            // VRD_REGION_NODE_SIZE, vrd_region_index_*
#include "region_table.h"   // vrd_Region_Table, vrd_region_table_*
#include "snv_index.h"  // vrd_SNV_Index, vrd_snv_index_*,
                        // VRD_SNV_NODE_SIZE
#include "snv_table.h"  // vrd_SNV_Table, vrd_snv_table_*
#include "trie.h"       // vrd_Trie, vrd_trie_*


enum
{
    // max number of elements (nodes) in each index
    VRD_MAX_INDEX_SIZE = (UINT64_C(1) << 20),

    // max value for a position
    VRD_MAX_POSITION   = (UINT64_C(1) << 28) - 1,

    // max number of distinct references per table
    VRD_MAX_REFERENCES = 512,

    // max string length of each reference id
    VRD_MAX_REFSEQ_ID  = 256,

    // max value for a sample id
    VRD_MAX_SAMPLE_ID  = (UINT64_C(1) << 29) - 1
}; // constants


/**
 * Runtime version information
 *
 * Gets the semantic versioning information on runtime.
 *
 * :param major: The major version number.
 * :type major: int
 * :param minor: The minor version number.
 * :type minor: int
 * :param patch: The patch version number.
 * :type patch: int
 */
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
