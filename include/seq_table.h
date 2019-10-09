#ifndef SEQ_TABLE_H
#define SEQ_TABLE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t


/**
 * Opaque data structure for an IUPAC sequences table.
 */
typedef struct vrd_Seq_Table vrd_Seq_Table;


/**
 * Create and initialize a table.
 *
 * @param capacity limits the number of sequences in the table.
 * @param size_capacity limits the combined length of the sequences in
 *                      the table.
 * @return A pointer to the table on success, otherwise NULL.
 */
vrd_Seq_Table*
vrd_seq_table_init(size_t const capacity,
                   size_t const size_capacity);


/**
 * Destroy a table.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param table is the reference to the table.
 */
void
vrd_seq_table_destroy(vrd_Seq_Table* restrict* const table);


/**
 * Insert a sequence to the table.
 *
 * @param table is the table.
 * @param len the length of the string (excluding the '\0').
 * @param str the string.
 * @return A pointer to the inserted data on success, otherwise NULL.
 */
char*
vrd_seq_table_insert(vrd_Seq_Table* const table,
                     size_t const len,
                     char const str[len]);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
