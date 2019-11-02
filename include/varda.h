#ifndef VRD_VARDA_H
#define VRD_VARDA_H

#ifdef __cplusplus
#define restrict
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
#define VRD_VERSION_PATCH 1
#endif


static inline void
vrd_version(int* const restrict major,
            int* const restrict minor,
            int* const restrict patch)
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


#ifdef __cplusplus
} // extern "C"
#endif

#endif
