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
#define VRD_VERSION_PATCH 4
#endif


enum
{
    VRD_HOMOZYGOUS = (1 << 28) - 1
}; // constants


static inline void
vrd_version(int* const major,
            int* const minor,
            int* const patch)
{
    *major = VRD_VERSION_MAJOR;
    *minor = VRD_VERSION_MINOR;
    *patch = VRD_VERSION_PATCH;
} // vrd_version


#include "avl_tree.h"
#include "cov_table.h"
#include "mnv_table.h"
#include "seq_table.h"
#include "snv_table.h"
#include "utils.h"


#ifdef __cplusplus
} // extern "C"
#endif

#endif
