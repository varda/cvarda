#ifndef MNV_TABLE_H
#define MNV_TABLE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "avl_tree.h"   // vrd_AVL_Tree


/**
 * Opaque data structure for a multi nucleotide variant (MNV) table.
 */
typedef struct vrd_MNV_Table vrd_MNV_Table;


/**
 * Create and initialize a MNV table.
 *
 * @param ref_capacity limits the number of reference sequences in the
 *                     table.
 * @param ref_size_capacity limits the combined length of the reference
 *                          sequences in the table.
 * @param tree_capacity limits the number of entries per reference
 *                      sequence in the table.
 * @return A pointer to the table on success, otherwise NULL.
 */
vrd_MNV_Table*
vrd_mnv_table_init(size_t const ref_capacity,
                   size_t const ref_size_capacity,
                   size_t const tree_capacity);


/**
 * Destroy a MNV table.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param table is the reference to the table.
 */
void
vrd_mnv_table_destroy(vrd_MNV_Table* restrict* const table);


/**
 * Insert a MNV in the table.
 *
 * @param table is the table.
 * @param len the length of the reference ID (excluding the '\0').
 * @param reference the reference ID.
 * @param start is the start position of the deleted part of the MNV
 *              (included).
 * @param end is the end position of the deleted part of the MNV
 *            (excluded).
 * @param sample_id is the ID of the sample that contains the MNV.
 * @param phase is the phase group (position based) to which the MNV
 *              belongs.
 * @param inserted is the inserted sequence.
 * @return 0 on success; otherwise -1.
 */
int
vrd_mnv_table_insert(vrd_MNV_Table* const restrict table,
                     size_t const len,
                     char const reference[len],
                     size_t const start,
                     size_t const end,
                     size_t const sample_id,
                     size_t const phase,
                     char const* const restrict inserted);


/**
 * Query for MNVs in the table.
 *
 * @param table is the table.
 * @param len the length of the reference ID (excluding the '\0').
 * @param reference the reference ID.
 * @param start is the start position of the deleted part of the MNV
 *              (included).
 * @param end is the end position of the deleted part of the MNV
 *            (excluded).
 * @param inserted is the inserted sequence of the MNV.
 * @param subset is the subset of sample IDs.
 * @return The count of reported regions.
 */
size_t
vrd_mnv_table_query(vrd_MNV_Table const* const restrict table,
                    size_t const len,
                    char const reference[len],
                    size_t const start,
                    size_t const end,
                    char const* const restrict inserted,
                    vrd_AVL_Tree const* const restrict subset);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
