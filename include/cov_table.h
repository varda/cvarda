#ifndef COV_TABLE_H
#define COV_TABLE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "avl_tree.h"   // vrd_AVL_Tree


/**
 * Opaque data structure for a coverage table.
 */
typedef struct vrd_Cov_Table vrd_Cov_Table;


/**
 * Create and initialize a coverage table.
 *
 * @param ref_capacity limits the number of reference sequences in the
 *                     table.
 * @param ref_size_capacity limits the combined length of the reference
 *                          sequences in the table.
 * @param tree_capacity limits the number of entries per reference
 *                      sequence in the table.
 * @return A pointer to the table on success, otherwise NULL.
 */
vrd_Cov_Table*
vrd_cov_table_init(size_t const ref_capacity,
                   size_t const ref_size_capacity,
                   size_t const tree_capacity);


/**
 * Destroy a coverage table.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param table is the reference to the table.
 */
void
vrd_cov_table_destroy(vrd_Cov_Table* restrict* const table);


/**
 * Insert a coverage region in the table.
 *
 * @param table is the table.
 * @param len the length of the reference ID (excluding the '\0').
 * @param reference the reference ID.
 * @param start is the start position of the covered region (included).
 * @param end is the end position of the covered region (excluded).
 * @param sample_id is the ID of the sample that contains the covered
 *                  region.
 * @return 0 on success; otherwise -1.
 */
int
vrd_cov_table_insert(vrd_Cov_Table* const table,
                     size_t const len,
                     char const reference[len],
                     size_t const start,
                     size_t const end,
                     size_t const sample_id);


/**
 * Query for coverage regions in the table.
 *
 * @param table is the table.
 * @param len the length of the reference ID (excluding the '\0').
 * @param reference the reference ID.
 * @param start is the start position of the covered region (included).
 * @param end is the end position of the covered region (excluded).
 * @param subset is the subset of sample IDs.
 * @return The count of reported regions.
 */
size_t
vrd_cov_table_query(vrd_Cov_Table const* const restrict table,
                    size_t const len,
                    char const reference[len],
                    size_t const start,
                    size_t const end,
                    vrd_AVL_Tree const* const restrict subset);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
