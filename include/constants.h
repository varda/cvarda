#ifndef VRD_CONSTANTS_H
#define VRD_CONSTANTS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t


static size_t const VRD_MAX_POSITION  = (1 << 28) - 1;
static size_t const VRD_MAX_ALLELE_COUNT = (1 << 4) - 1;

static size_t const VRD_MAX_SAMPLE_ID = (1 << 29) - 1;

static size_t const VRD_HOMOZYGOUS = (1 << 28) - 1;


#ifdef __cplusplus
} // extern "C"
#endif

#endif
