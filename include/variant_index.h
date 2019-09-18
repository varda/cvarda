#ifndef VARIANT_INDEX_H
#define VARIANT_INDEX_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t

#include "alloc.h"      // vrd_Alloc


typedef struct Variant_Index vrd_Variant_Index;


extern size_t const VRD_VARIANT_NODE_SIZE;


vrd_Variant_Index*
vrd_variant_init(vrd_Alloc* const restrict alloc);


void
vrd_variant_destroy(vrd_Variant_Index* restrict* const restrict index);


int
vrd_variant_insert(vrd_Variant_Index* const restrict index,
                   uint32_t const start,
                   uint32_t const end,
                   uint32_t const sample_id,
                   uint32_t const phase,
                   void* const restrict inserted);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
