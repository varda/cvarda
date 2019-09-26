#ifndef SNV_TABLE_H
#define SNV_TABLE_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t


typedef struct SNV_Table vrd_SNV_Table;


vrd_SNV_Table*
vrd_snv_table_init(void);


void
vrd_snv_table_destroy(vrd_SNV_Table* restrict* const restrict table);


int
vrd_snv_table_insert(vrd_SNV_Table* const restrict table,
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
