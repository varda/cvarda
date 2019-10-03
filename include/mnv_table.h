#ifndef MNV_TABLE_H
#define MNV_TABLE_H

#ifdef __cplusplus
#define restrict

extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t


/**
 * Opaque data structure for a multi nucleotide variant (MNV) table.
 */
typedef struct vrd_MNV_Table vrd_MNV_Table;


/**
 * Create and initialize a MNV table.
 *
 * @return A pointer to the table on success, otherwise NULL.
 */
vrd_MNV_Table*
vrd_mnv_table_init(void);


/**
 * Destroy a MNV table.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param trie is the reference to the table.
 */
void
vrd_mnv_table_destroy(vrd_MNV_Table* restrict* const table);


/**
 * Insert a MNV in the table.
 *
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
vrd_mnv_table_insert(vrd_MNV_Table* const table,
                     size_t const len,
                     char const reference[len],
                     uint32_t const start,
                     uint32_t const end,
                     uint32_t const sample_id,
                     uint32_t const phase,
                     void* const inserted);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
