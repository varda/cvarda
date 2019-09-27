#ifndef MNV_TABLE_H
#define MNV_TABLE_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t


typedef struct MNV_Table vrd_MNV_Table;


vrd_MNV_Table*
vrd_mnv_table_init(void);


void
vrd_mnv_table_destroy(vrd_MNV_Table* restrict* const restrict table);


int
vrd_mnv_table_insert(vrd_MNV_Table* const restrict table,
                     size_t const len,
                     char const reference[len],
                     uint32_t const start,
                     uint32_t const end,
                     uint32_t const sample_id,
                     uint32_t const phase,
                     void* const restrict inserted);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
