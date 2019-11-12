#ifndef VRD_SEQ_TABLE_H
#define VRD_SEQ_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct vrd_Seq_Table vrd_Seq_Table;


vrd_Seq_Table*
vrd_Seq_table_init(size_t const capacity);


void
vrd_Seq_table_destroy(vrd_Seq_Table** const self);


void*
vrd_Seq_table_insert(vrd_Seq_Table* const self,
                     size_t const len,
                     char const sequence[len]);


void*
vrd_Seq_table_query(vrd_Seq_Table const* const self,
                    size_t const len,
                    char const sequence[len]);


int
vrd_Seq_table_remove(vrd_Seq_Table* const self, size_t const elem);


int
vrd_Seq_table_read(vrd_Seq_Table* const self,
                   char const* const path);


int
vrd_Seq_table_write(vrd_Seq_Table const* const self,
                    char const* const path);


// DEBUG
void
vrd_Seq_table_free_list_print(vrd_Seq_Table const* const self);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
