/**
 * @file seq_table.h
 *
 * Defines a table to store (inserted) IUPAC nucleotide sequences. The
 * supported operations are:
 *   - create an empty table (vrd_seq_table_init())
 *   - destroy a table (vrd_seq_table_destroy())
 *   - insert a sequence (vrd_seq_table_insert())
 *   - retrieve a sequence (vrd_seq_table_query())
 * @warning The number of unique sequences may by limited by the
 *          implementation.
 */


#ifndef SEQ_TABLE_H
#define SEQ_TABLE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t


/**
 * Opaque data type for a sequence table.
 *
 * Provides an opaque reference to a sequence table. See also:
 * https://en.wikipedia.org/wiki/Opaque_data_type
 */
typedef struct vrd_Seq_Table vrd_Seq_Table;


/**
 * Create an empty sequence table.
 *
 * @param capacity limits the number of distinct sequences in the table.
 *                 This number may be further limited by the
 *                 implementation.
 * @return An opaque pointer to the table on success, otherwise `NULL`.
 */
vrd_Seq_Table*
vrd_seq_table_init(size_t const capacity);


/**
 * Destroy a sequence table.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param table is a reference to a table. The reference may be `NULL`.
 *              Calling this function multiple times is safe.
 */
void
vrd_seq_table_destroy(vrd_Seq_Table* restrict* const table);


/**
 * Insert a sequnece to a sequence table.
 *
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param len is the length of the reference sequence identifier
 *            (`reference`). `strlen()` may be used to calculate the
 *            length of a `\0`-terminated string.
 * @param seq is the sequence in IUPAC nucleotides (ASCII encoded).
 * @return A pointer to the copy of the stored sequence. May be `NULL`.
 */
void*
vrd_seq_table_insert(vrd_Seq_Table* const table,
                     size_t const len,
                     char const seq[len]);


/**
 * Retrieve a sequence from the table.
 *
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param table is a valid reference to a table. The reference to the
 *              table must be valid, otherwise this function results in
 *              undefined behavior.
 * @param len is the length of the reference sequence identifier
 *            (`reference`). `strlen()` may be used to calculate the
 *            length of a `\0`-terminated string.
 * @param seq is the sequence in IUPAC nucleotides (ASCII encoded).
 * @return A pointer to the copy of the stored sequence. May be `NULL`.
 */
void*
vrd_seq_table_query(vrd_Seq_Table const* const table,
                    size_t const len,
                    char const seq[len]);


void
vrd_seq_table_remove(vrd_Seq_Table* const table,
                     size_t const len,
                     char const seq[len]);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
