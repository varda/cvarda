/**
 * @file snv_table.h
 *
 * Defines a table to store single nucleotide variants (SNV), possibly on
 * multiple reference sequences. The supported operations are:
 *   - create an empty table (vrd_snv_table_init())
 *   - destroy a table (vrd_snv_table_destroy())
 *   - insert a SNV (vrd_snv_table_insert())
 *   - count the number of times a SNV is contained within the SNVs in
 *     the table (vrd_snv_table_query())
 *   - remove all SNVs belonging to a set of samples
 *     (vrd_snv_table_remove())
 * @warning The number of the reference sequence identifiers may be
 *          limited by the implementation. The number of SNVs per
 *          reference sequence may by limited by the implementation.
 */


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
 * Opaque data type for a SNV table.
 *
 * Provides an opaque reference to a SNV table. See also:
 * https://en.wikipedia.org/wiki/Opaque_data_type
 */
typedef struct vrd_SNV_Table vrd_SNV_Table;


/**
 * Create an empty SNV table.
 *
 * @param ref_capacity limits the number of distinct reference sequence
 *                     identifiers in the table. This number may be
 *                     further limited by the implementation.
 * @param tree_capacity limits the number of covered regions per
 *                      reference sequence in the table. This number may
 *                      be further limited by the implementation.
 * @return An opaque pointer to the table on success, otherwise `NULL`.
 */
vrd_SNV_Table*
vrd_snv_table_init(size_t const ref_capacity,
                   size_t const tree_capacity);


/**
 * Destroy a SNV table.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param table is a reference to a table. The reference may be `NULL`.
 *              Calling this function multiple times is safe.
 */
void
vrd_snv_table_destroy(vrd_SNV_Table* restrict* const table);


/**
 * Insert a SNV to a SNV table.
 *
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param len is the length of the reference sequence identifier
 *            (`reference`). `strlen()` may be used to calculate the
 *            length of a `\0`-terminated string.
 * @param reference is the reference sequence identifier in printable
 *                  ASCII.
 * @param position is the position of the SNV. This value is not bound
 *                 checked. It is the resposibility of the caller to make
 *                 sure that the table can actually store the value.
 * @param sample_id is an sample identifier indicating to which sample
 *                  the SNV belongs. This value is not bound checked. It
 *                  is the resposibility of the caller to make sure that
 *                  the table can actually store the value.
 * @param phase is the phase set identifier for the SNV. This value is
 *              not bound checked. It is the resposibility of the caller
 *              to make sure that the table can actually store the value.
 * @param inserted is the inserted nucleotide of the SNV as integer
 *                 index using vrd_iupac_to_idx().
 * @return `0` on success, otherwise `-1`.
 */
int
vrd_snv_table_insert(vrd_SNV_Table* const table,
                     size_t const len,
                     char const reference[len],
                     size_t const position,
                     size_t const sample_id,
                     size_t const phase,
                     size_t const inserted);


/**
 * Count the number of times a SNV is contained within the SNVs in the
 * table.
 *
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param len is the length of the reference sequence identifier
 *            (`reference`). `strlen()` may be used to calculate the
 *            length of a `\0`-terminated string.
 * @param reference is the reference sequence identifier in printable
 *                  ASCII.
 * @param position is the position of the SNV. This value is not bound
 *                 checked. It is the resposibility of the caller to make
 *                 sure that the table can actually store the value.
 * @param inserted is the inserted nucleotide of the SNV as integer
 *                 index using vrd_iupac_to_idx().
 * @param subset is an AVL tree holding sample identifiers that define a
 *               subset of the samples in the table. Only SNVs that
 *               belong to a sample in this tree are considered. If the
 *               subset is `NULL`, all SNVs in the table are
 *               considered.
 * @return The number of SNVs (for the subset) that are included in the
 *         table.
 */
size_t
vrd_snv_table_query(vrd_SNV_Table const* const restrict table,
                    size_t const len,
                    char const reference[len],
                    size_t const position,
                    size_t const inserted,
                    vrd_AVL_Tree const* const restrict subset);


/**
 * Remove all SNVs belonging to a set of samples from the table.
 *
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param subset is an AVL tree holding sample identifiers that define a
 *               subset of the samples in the table. Only SNVs that
 *               belong to a sample in this tree are removed.
 * @return The number of SNVs that where removed.
 */
size_t
vrd_snv_table_remove(vrd_SNV_Table* const restrict table,
                     vrd_AVL_Tree const* const restrict subset);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
