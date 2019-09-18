#ifndef SNV_INDEX_H
#define SNV_INDEX_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t

#include "alloc.h"      // vrd_Alloc


typedef struct SNV_Index vrd_SNV_Index;


extern size_t const VRD_SNV_NODE_SIZE;


vrd_SNV_Index*
vrd_snv_init(vrd_Alloc* const restrict alloc);


void
vrd_snv_destroy(vrd_SNV_Index* restrict* const restrict index);


int
vrd_snv_insert(vrd_SNV_Index* const restrict index,
               uint32_t const position,
               uint32_t const sample_id,
               uint32_t const phase,
               uint32_t const type);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
