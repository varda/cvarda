#ifndef VRD_DIAGNOSTICS_H
#define VRD_DIAGNOSTICS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t


typedef struct vrd_Diagnostics
{
    char* reference;
    size_t entries;
    size_t height;
} vrd_Diagnostics;


#ifdef __cplusplus
} // extern "C"
#endif

#endif
