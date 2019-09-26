#ifndef SNV_INDEX_H
#define SNV_INDEX_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t
#include <stdio.h>      // FILE*

#include "alloc.h"      // vrd_Alloc


typedef struct SNV_Index vrd_SNV_Index;


extern size_t const VRD_SNV_NODE_SIZE;
extern size_t const VRD_SNV_INDEX_SIZE;


vrd_SNV_Index*
vrd_snv_index_init(size_t const capacity);


void
vrd_snv_index_destroy(vrd_SNV_Index* restrict* const restrict index);


int
vrd_snv_index_insert(vrd_SNV_Index* const restrict index,
                     uint32_t const position,
                     uint32_t const sample_id,
                     uint32_t const phase,
                     uint32_t const type);


size_t
vrd_snv__index_print(FILE* const restrict stream,
                     vrd_SNV_Index const* const restrict index);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
