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


#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_avl_tree_*
#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_cov_table_*
#include "../include/iupac_trie.h"  // vrd_IUPAC_Trie, vrd_iupac_*
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*


enum
{
    VRD_MAX_REFS = 1000,
    VRD_MAX_SEQS = 1000000,
    VRD_MAX_TRIE_SIZE = 10000,
    VRD_MAX_TREE_SIZE = 1 << 28
}; // constants


/**
 * Determine the semantic versioning information on run time.
 *
 * @param major is the major version number
 * @param minor is the minor version number
 * @param patch is the patch version number
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
