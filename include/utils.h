#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdio.h>      // FILE

#include "../include/mnv_table.h"   // vrd_MNV_Table
#include "../include/seq_table.h"   // vrd_Seq_Table
#include "../include/snv_table.h"   // vrd_SNV_Table


size_t
vrd_variants_from_file(FILE* restrict stream,
                       vrd_SNV_Table* const restrict snv,
                       vrd_MNV_Table* const restrict mnv,
                       vrd_Seq_Table* const restrict seq,
                       size_t const sample_id);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
