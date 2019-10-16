/**
 * @file cov_table.h
 *
 * Defines a table to store covered genomic regions, possibly on multiple
 * reference sequences. The supported operations are:
 *   - create an empty table (vrd_cov_table_init())
 *   - destroy a table (vrd_cov_table_destroy())
 *   - insert a covered region (vrd_cov_table_insert())
 *   - count the number of times a region is contained within the regions
 *     in the table (vrd_cov_table_query())
 * @warning The number and length of the reference sequence identifiers
 *          may be limited by the implementation. The number of regions
 *          per reference sequence may by limited by the implementation.
 */


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
 * Opaque data type for a coverage table.
 *
 * Provides an opaque reference to a coverage table. See also:
 * https://en.wikipedia.org/wiki/Opaque_data_type
 */
typedef struct vrd_Cov_Table vrd_Cov_Table;


/**
 * Create an empty coverage table.
 *
 * @param ref_capacity limits the number of distinct reference sequence
 *                     identifiers in the table. This number may be
 *                     further limited by the implementation.
 * @param ref_size_capacity limits the number of distinct reference
 *                          sequence identifiers prefices in the table.
 *                          This number may be further limited by the
 *                          implementation.
 * @param tree_capacity limits the number of covered regions per
 *                      reference sequence in the table. This number may
 *                      be further limited by the implementation.
 * @return An opaque pointer to the table on success, otherwise `NULL`.
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
 * @param table is a reference to a table. The reference may be `NULL`.
 *              Calling this function multiple times is safe.
 */
void
vrd_cov_table_destroy(vrd_Cov_Table* restrict* const table);


/**
 * Insert a covered region to a coverage table.
 *
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param len is the length of the reference sequence identifier
              (`reference`).  `strlen()` may be used to
 *            calculate the length of a `\0`-terminated string.
 * @param reference is the reference sequence identifier in printable
 *                  ASCII.
 * @param start is the start position of the covered region (included).
 *              This value is not bound checked. It is the resposibility
 *              of the caller to make sure that the table can actually
 *              store the value.
 * @param end is the end position of the covered region (excluded).
 *            This value is not bound checked. It is the resposibility
 *            of the caller to make sure that the table can actually
 *            store the value.
 * @param sample_id is an sample identifier indicating to which sample
 *                  the covered region belongs. This value is not bound
 *                  checked. It is the resposibility of the caller to
 *                  make sure that the table can actually store the
 *                  value.
 * @return `0` on success, otherwise `-1`.
 */
int
vrd_cov_table_insert(vrd_Cov_Table* const table,
                     size_t const len,
                     char const reference[len],
                     size_t const start,
                     size_t const end,
                     size_t const sample_id);


/**
 * Count the number of times a region is contained within the regions
 * in the table.
 *
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param len is the length of the reference sequence identifier
              (`reference`).  `strlen()` may be used to
 *            calculate the length of a `\0`-terminated string.
 * @param reference is the reference sequence identifier in printable
 *                  ASCII.
 * @param start is the start position of the covered region (included).
 *              This value is not bound checked. It is the resposibility
 *              of the caller to make sure that the table can actually
 *              store the value.
 * @param end is the end position of the covered region (excluded).
 *            This value is not bound checked. It is the resposibility
 *            of the caller to make sure that the table can actually
 *            store the value.
 * @param subset is an AVL tree holding sample identifiers that define a
 *               subset of the samples in the table. Only regions that
 *               belong to a sample in this tree are considered. If the
 *               subset is `NULL`, all regions in the table are
 *               considered.
 * @return The number of covered regions (for the subset) that include
 *         the query region.
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
