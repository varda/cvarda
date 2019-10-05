#ifndef SNV_TABLE_H
#define SNV_TABLE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t


/**
 * Opaque data structure for a single nucleotide variant (SNV) table.
 */
typedef struct vrd_SNV_Table vrd_SNV_Table;


/**
 * Create and initialize a SNV table.
 *
 * @return A pointer to the table on success, otherwise NULL.
 */
vrd_SNV_Table*
vrd_snv_table_init(void);


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
 * @param type is the type of the SNV, e.g., A, C, G, or T, currently,
 *             nucleotide IUPAC is supported.
 * @return 0 on success; otherwise -1.
 */
int
vrd_snv_table_insert(vrd_SNV_Table* const table,
                     size_t const len,
                     char const reference[len],
                     uint32_t const position,
                     uint32_t const sample_id,
                     uint32_t const phase,
                     uint32_t const type);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
