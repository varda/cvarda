/**
 * @file mnv_table.h
 *
 * Defines a table to store multi nucleotide variants (MNV), possibly on
 * multiple reference sequences. The supported operations are:
 *   - create an empty table (vrd_mnv_table_init())
 *   - destroy a table (vrd_mnv_table_destroy())
 *   - insert a MNV (vrd_mnv_table_insert())
 *   - count the number of times a MNV is contained within the MNVs in
 *     the table (vrd_mnv_table_query())
 *   - remove all MNVs belonging to a set of samples
 *     (vrd_mnv_table_remove())
 * @warning The number of the reference sequence identifiers may be
 *          limited by the implementation. The number of MNVs per
 *          reference sequence may by limited by the implementation.
 */


#ifndef MNV_TABLE_H
#define MNV_TABLE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "avl_tree.h"   // vrd_AVL_Tree
#include "seq_table.h"  // vrd_Seq_Table


/**
 * Opaque data type for a MNV table.
 *
 * Provides an opaque reference to a MNV table. See also:
 * https://en.wikipedia.org/wiki/Opaque_data_type
 */
typedef struct vrd_MNV_Table vrd_MNV_Table;


/**
 * Create an empty MNV table.
 *
 * @param ref_capacity limits the number of distinct reference sequence
 *                     identifiers in the table. This number may be
 *                     further limited by the implementation.
 * @param tree_capacity limits the number of covered regions per
 *                      reference sequence in the table. This number may
 *                      be further limited by the implementation.
 * @return An opaque pointer to the table on success, otherwise `NULL`.
 */
vrd_MNV_Table*
vrd_mnv_table_init(size_t const ref_capacity,
                   size_t const tree_capacity);


/**
 * Destroy a MNV table.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param table is a reference to a table. The reference may be `NULL`.
 *              Calling this function multiple times is safe.
 */
void
vrd_mnv_table_destroy(vrd_MNV_Table* restrict* const table);


/**
 * Insert a MNV to a MNV table.
 *
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param len is the length of the reference sequence identifier
 *            (`reference`). `strlen()` may be used to calculate the
 *            length of a `\0`-terminated string.
 * @param reference is the reference sequence identifier in printable
 *                  ASCII.
 * @param start is the start position of the deleted part of the MNV
 *              (included). This value is not bound checked. It is the
 *              resposibility of the caller to make sure that the table
 *              can actually store the value.
 * @param end is the end position of the deleted part of the MNV
 *            (excluded). This value is not bound checked. It is the
 *            resposibility of the caller to make sure that the table can
 *            actually store the value.
 * @param sample_id is an sample identifier indicating to which sample
 *                  the MNV belongs. This value is not bound checked. It
 *                  is the resposibility of the caller to make sure that
 *                  the table can actually store the value.
 * @param phase is the phase set identifier for the MNV. This value is
 *              not bound checked. It is the resposibility of the caller
 *              to make sure that the table can actually store the value.
 * @param inserted is the inserted sequence of the MNV as a string of
 *                 IUPAC nucleotides (ASCII encoded). May be `NULL`. The
 *                 string must be accessible in the future. It should
 *                 therefore be stored. Perhaps using a vrd_Seq_Table().
 * @return `0` on success, otherwise `-1`.
 */
int
vrd_mnv_table_insert(vrd_MNV_Table* const restrict table,
                     size_t const len,
                     char const reference[len],
                     size_t const start,
                     size_t const end,
                     size_t const sample_id,
                     size_t const phase,
                     size_t const inserted);


/**
 * Count the number of times a MNV is contained within the MNVs in the
 * table.
 *
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param len is the length of the reference sequence identifier
 *            (`reference`). `strlen()` may be used to
 *            calculate the length of a `\0`-terminated string.
 * @param reference is the reference sequence identifier in printable
 *                  ASCII.
 * @param start is the start position of the deleted part of the MNV
 *              (included). This value is not bound checked. It is the
 *              resposibility of the caller to make sure that the table
 *              can actually store the value.
 * @param end is the end position of the deleted part of the MNV
 *            (excluded). This value is not bound checked. It is the
 *            resposibility of the caller to make sure that the table can
 *            actually store the value.
 * @param inserted is the inserted sequence of the MNV as a string of
 *                 IUPAC nucleotides (ASCII encoded). May be `NULL`.
 * @param subset is an AVL tree holding sample identifiers that define a
 *               subset of the samples in the table. Only MNVs that
 *               belong to a sample in this tree are considered. If the
 *               subset is `NULL`, all MNVs in the table are
 *               considered.
 * @return The number of MNVs (for the subset) that are included in the
 *         table.
 */
size_t
vrd_mnv_table_query(vrd_MNV_Table const* const restrict table,
                    size_t const len,
                    char const reference[len],
                    size_t const start,
                    size_t const end,
                    size_t const inserted,
                    vrd_AVL_Tree const* const restrict subset);


/**
 * Remove all MNVs belonging to a set of samples from the table.
 *
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param subset is an AVL tree holding sample identifiers that define a
 *               subset of the samples in the table. Only MNVs that
 *               belong to a sample in this tree are removed.
 * @return The number of MNVs that where removed.
 */
size_t
vrd_mnv_table_remove(vrd_MNV_Table* const restrict table,
                     vrd_AVL_Tree const* const restrict subset,
                     vrd_Seq_Table* const restrict seq_table);


int
vrd_mnv_table_reorder(vrd_MNV_Table* const table);


int
vrd_mnv_table_read(vrd_MNV_Table* const restrict table,
                   char const* const restrict path);


int
vrd_mnv_table_write(vrd_MNV_Table const* const restrict table,
                    char const* const restrict path);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
