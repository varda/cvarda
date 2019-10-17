#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
#define restrict
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
vrd_coverage_from_file(FILE* restrict stream,
                       vrd_Cov_Table* const restrict cov,
                       size_t const sample_id);


size_t
vrd_variants_from_file(FILE* restrict stream,
                       vrd_SNV_Table* const restrict snv,
                       vrd_MNV_Table* const restrict mnv,
                       vrd_Seq_Table* const restrict seq,
                       size_t const sample_id);


size_t
vrd_annotate_from_file(FILE* restrict ostream,
                       FILE* restrict istream,
                       vrd_Cov_Table const* const restrict cov,
                       vrd_SNV_Table const* const restrict snv,
                       vrd_MNV_Table const* const restrict mnv,
                       vrd_Seq_Table const* const restrict seq,
                       vrd_AVL_Tree const* const restrict subset);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
