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


typedef struct Region_Table vrd_Region_Table;


vrd_Region_Table*
vrd_region_table_init(void);


void
vrd_region_table_destroy(vrd_Region_Table* restrict*
                            const restrict table);


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
