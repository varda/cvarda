#ifndef VRD_SEQ_TABLE_H
#define VRD_SEQ_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "diagnostics.h"    // vrd_Diagnostics
#include "trie.h"   // vrd_Trie_Node


typedef struct vrd_Seq_Table vrd_Seq_Table;


vrd_Seq_Table*
vrd_Seq_table_init(size_t const capacity);


void
vrd_Seq_table_destroy(vrd_Seq_Table** const self);


vrd_Trie_Node*
vrd_Seq_table_insert(vrd_Seq_Table* const self,
                     size_t const len,
                     char const sequence[len]);


vrd_Trie_Node*
vrd_Seq_table_query(vrd_Seq_Table const* const self,
                    size_t const len,
                    char const sequence[len]);


size_t
vrd_Seq_table_key(vrd_Seq_Table const* const self,
                  size_t const elem,
                  char** key);


int
vrd_Seq_table_remove(vrd_Seq_Table* const self, size_t const elem);


int
vrd_Seq_table_read(vrd_Seq_Table* const self, char const* const path);


int
vrd_Seq_table_write(vrd_Seq_Table const* const self,
                    char const* const path);


size_t
vrd_Seq_table_diagnostics(vrd_Seq_Table const* const self,
                          vrd_Diagnostics** diag);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
