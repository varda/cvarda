#ifndef REGION_TABLE_H
#define REGION_TABLE_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t

#include "avl_tree.h"   // vrd_ALV_Tree


/**
 * Opaque data type for a region table
 */
typedef struct Region_Table vrd_Region_Table;


/**
 * Create and initialize a new region table
 *
 * :return: A pointer to an opaque region table object
 */
vrd_Region_Table*
vrd_region_table_init(void);


/**
 * Destroy a region table object and free all associated data
 *
 * :param table: Address of a pointer to an opaque region table object;
 *               the pointer is set to NULL
 */
void
vrd_region_table_destroy(vrd_Region_Table* restrict*
                         const restrict table);


/**
 * Insert a region in a :c:type:`vrd_Region_Table`
 *
 * :param table: A pointer to an opaque region table object;
 * :param len: The string length of the reference sequence ID
 * :param reference: The reference sequence ID
 * :param start: The start position of the region (included)
 * :param end: The end position of the region (excluded)
 * :param sample_id: The sample ID
 * :param phase: The phase group (the start position)
 * :return: 0 on success; -1 on error
 */
int
vrd_region_table_insert(vrd_Region_Table* const restrict table,
                        size_t const len,
                        char const reference[len],
                        uint32_t const start,
                        uint32_t const end,
                        uint32_t const sample_id,
                        uint32_t const phase);


size_t
vrd_region_table_query(vrd_Region_Table const* const restrict table,
                       size_t const len,
                       char const reference[len],
                       uint32_t const start,
                       uint32_t const end,
                       vrd_AVL_Tree const* const restrict subset);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
