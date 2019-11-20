#ifndef VRD_VARDA_H
#define VRD_VARDA_H

#ifdef __cplusplus
extern "C"
{
#endif


#ifndef VRD_VERSION_MAJOR
#define VRD_VERSION_MAJOR 2
#endif

#ifndef VRD_VERSION_MINOR
#define VRD_VERSION_MINOR 0
#endif

#ifndef VRD_VERSION_PATCH
#define VRD_VERSION_PATCH 8
#endif


static inline void
vrd_version(int* const major,
            int* const minor,
            int* const patch)
{
    *major = VRD_VERSION_MAJOR;
    *minor = VRD_VERSION_MINOR;
    *patch = VRD_VERSION_PATCH;
} // vrd_version


#include "avl_tree.h"   // vrd_AVL_Tree, vrd_AVL_tree_*
#include "cov_table.h"  // vrd_Cov_Table, vrd_Cov_table_*
#include "iupac.h"      // VRD_IUPAC_SIZE, vrd_iupac_to_idx
#include "mnv_table.h"  // vrd_MNV_Table, vrd_MNV_table_*
#include "seq_table.h"  // vrd_Seq_Table, vrd_Seq_table_*
#include "snv_table.h"  // vrd_SNV_Table, vrd_SNV_table_*
#include "trie.h"       // vrd_Trie_Node, vrd_Trie, vrd_trie_*
#include "utils.h"      // vrd_coverage_from_file,
                        // vrd_variants_from_file,
                        // vrd_annotate_from_file


#ifdef __cplusplus
} // extern "C"
#endif

#endif
