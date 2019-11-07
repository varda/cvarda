#ifndef VRD_UTILS_H
#define VRD_UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdio.h>      // FILE

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/cov_table.h"   // vrd_Cov_Table
#include "../include/mnv_table.h"   // vrd_MNV_Table
#include "../include/seq_table.h"   // vrd_Seq_Table
#include "../include/snv_table.h"   // vrd_SNV_Table


size_t
vrd_coverage_from_file(FILE* stream,
                       vrd_Cov_Table* const cov,
                       size_t const sample_id);


size_t
vrd_variants_from_file(FILE* stream,
                       vrd_SNV_Table* const snv,
                       vrd_MNV_Table* const mnv,
                       vrd_Seq_Table* const seq,
                       size_t const sample_id);


size_t
vrd_annotate_from_file(FILE* ostream,
                       FILE* istream,
                       vrd_Cov_Table const* const cov,
                       vrd_SNV_Table const* const snv,
                       vrd_MNV_Table const* const mnv,
                       vrd_Seq_Table const* const seq,
                       vrd_AVL_Tree const* const subset);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
