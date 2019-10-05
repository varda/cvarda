#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdlib.h>     // malloc, free

#include "../include/iupac_trie.h"  // vrd_IUPAC_Trie
#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
#include "../include/varda.h"   // VRD_*


struct vrd_Seq_Table
{
    vrd_IUPAC_Trie* restrict trie;

    size_t next;
    char* restrict table[];
}; // vrd_Seq_Table


vrd_Seq_Table*
vrd_seq_table_init(void)
{
    vrd_Seq_Table* const table = malloc(sizeof(*table) +
                                        sizeof(table->table[0]) *
                                        VRD_MAX_SEQS);
    if (NULL == table)
    {
        return NULL;
    } // if

    table->trie = vrd_iupac_trie_init(VRD_MAX_TRIE_SIZE);
    if (NULL == table->trie)
    {
        free(table);
        return NULL;
    } // if

    table->next = 0;

    return table;
} // vrd_seq_table_init


void
vrd_seq_table_destroy(vrd_Seq_Table* restrict* const table)
{
    if (NULL != table)
    {
        for (size_t i = 0; i < (*table)->next; ++i)
        {
            free(table[i]);
        } // for
        vrd_iupac_trie_destroy(&(*table)->trie);
        free(*table);
        *table = NULL;
    } // if
} // vrd_seq_table_destroy
