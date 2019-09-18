#ifndef MNV_INDEX_H
#define MNV_INDEX_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t

#include "alloc.h"      // vrd_Alloc


typedef struct MNV_Index vrd_MNV_Index;


extern size_t const VRD_MNV_NODE_SIZE;


vrd_MNV_Index*
vrd_mnv_init(vrd_Alloc* const restrict alloc);


void
vrd_mnv_destroy(vrd_MNV_Index* restrict* const restrict index);


int
vrd_mnv_insert(vrd_MNV_Index* const restrict index,
               uint32_t const start,
               uint32_t const end,
               uint32_t const sample_id,
               uint32_t const phase,
               void* const restrict inserted);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
