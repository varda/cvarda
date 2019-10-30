/**
 * @file varda.h
 *
 * Provides the public C API in one header and defines the version. See
 * also: https://semver.org/
 *
 * The prefices `VRD_` and `vrd_` are reserved.
 */


#ifndef VARDA_H
#define VARDA_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#ifndef VRD_VERSION_MAJOR
/**
 * The default major version number. This should be defined by the build
 * engine (`make`) on compile time, e.g., `-DVRD_VERSION_MAJOR=2`.
 */
#define VRD_VERSION_MAJOR 1
#endif

#ifndef VRD_VERSION_MINOR
/**
 * The default minor version number. This should be defined by the build
 * engine (`make`) on compile time, e.g., `-DVRD_VERSION_MINOR=2`.
 */
#define VRD_VERSION_MINOR 2
#endif

#ifndef VRD_VERSION_PATCH
/**
 * The default patch version number. This should be defined by the build
 * engine (`make`) on compile time, e.g., `-DVRD_VERSION_PATCH=2`.
 */
#define VRD_VERSION_PATCH 2
#endif


#include "iupac.h"      // VRD_IUPAC_SIZE, vrd_iupac_to_idx
#include "avl_tree.h"   // vrd_AVL_Tree, vrd_avl_tree_*
#include "cov_table.h"  // vrd_Cov_Table, vrd_cov_table_*
#include "mnv_table.h"  // vrd_MNV_Table, vrd_mnv_table_*
#include "seq_table.h"  // vrd_Seq_Table, vrd_seq_table_*
#include "snv_table.h"  // vrd_SNV_Table, vrd_snv_table_*
#include "utils.h"      // vrd_coverage_from_file, vrd_variants_from_file


/**
 * The semantic versioning information on run time.
 *
 * @param major is the major version number.
 * @param minor is the minor version number.
 * @param patch is the patch version number.
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
