#ifndef VRD_SEQ_TABLE_H
#define VRD_SEQ_TABLE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


typedef struct vrd_Seq_Table vrd_Seq_Table;


vrd_Seq_Table*
vrd_Seq_table_init(size_t const capacity);


void
vrd_Seq_table_destroy(vrd_Seq_Table* restrict* const self);


void*
vrd_Seq_table_insert(vrd_Seq_Table* const self,
                     size_t const len,
                     char const sequence[len]);


void*
vrd_Seq_table_query(vrd_Seq_Table const* const self,
                    size_t const len,
                    char const sequence[len]);


int
vrd_Seq_table_remove(vrd_Seq_Table* const restrict self, size_t const elem);


int
vrd_Seq_table_read(vrd_Seq_Table* const restrict self,
                   char const* const restrict path);


int
vrd_Seq_table_write(vrd_Seq_Table const* const restrict self,
                    char const* const restrict path);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
