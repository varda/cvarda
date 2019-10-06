#ifndef SNV_TABLE_H
#define SNV_TABLE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "avl_tree.h"   // vrd_AVL_Tree


/**
 * Opaque data structure for a single nucleotide variant (SNV) table.
 */
typedef struct vrd_SNV_Table vrd_SNV_Table;


/**
 * Create and initialize a SNV table.
 *
 * @param ref_capacity limits the number of reference sequences in the
 *                     table.
 * @param ref_size_capacity limits the combined length of the reference
 *                          sequences in the table.
 * @param tree_capacity limits the number of entries per reference
 *                      sequence in the table.
 * @return A pointer to the table on success, otherwise NULL.
 */
vrd_SNV_Table*
vrd_snv_table_init(size_t const ref_capacity,
                   size_t const ref_size_capacity,
                   size_t const tree_capacity);


/**
 * Destroy a SNV table.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param table is the reference to the table.
 */
void
vrd_snv_table_destroy(vrd_SNV_Table* restrict* const table);


/**
 * Insert a SNV in the table.
 *
 * @param table is the table.
 * @param len the length of the reference ID (excluding the '\0').
 * @param reference the reference ID.
 * @param position is the position of the SNV.
 * @param sample_id is the ID of the sample that contains the SNV.
 * @param phase is the phase group (position based) to which the SNV
 *              belongs.
 * @param type is the type of the SNV; nucleotide IUPAC is supported.
 * @return 0 on success; otherwise -1.
 */
int
vrd_snv_table_insert(vrd_SNV_Table* const table,
                     size_t const len,
                     char const reference[len],
                     size_t const position,
                     size_t const sample_id,
                     size_t const phase,
                     size_t const type);


/**
 * Query for SNVs in the table.
 *
 * @param table is the table.
 * @param len the length of the reference ID (excluding the '\0').
 * @param reference the reference ID.
 * @param position is the position of the SNV.
 * @param type is the type of the SNV.
 * @param subset is the subset of sample IDs.
 * @return The count of reported regions.
 */
size_t
vrd_snv_table_query(vrd_SNV_Table const* const restrict table,
                    size_t const len,
                    char const reference[len],
                    size_t const position,
                    size_t const type,
                    vrd_AVL_Tree const* const restrict subset);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
