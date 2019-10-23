#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX
#include <stdlib.h>     // malloc, free
#include <string.h>     // strncpy

#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
#include "iupac_trie.h"  // vrd_IUPAC_Trie


struct vrd_Seq_Table
{
    vrd_IUPAC_Trie* restrict trie;

    size_t capacity;
    size_t next;
    char* restrict sequences[];
}; // vrd_Seq_Table


vrd_Seq_Table*
vrd_seq_table_init(size_t const capacity,
                   size_t const size_capacity)
{
    if ((size_t) UINT32_MAX <= capacity ||
        (size_t) UINT32_MAX <= size_capacity)
    {
        return NULL;
    } // if

    vrd_Seq_Table* const table = malloc(sizeof(*table) +
                                        sizeof(table->sequences[0]) *
                                        capacity);
    if (NULL == table)
    {
        return NULL;
    } // if

    table->trie = vrd_iupac_trie_init(size_capacity);
    if (NULL == table->trie)
    {
        free(table);
        return NULL;
    } // if

    table->capacity = capacity;
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
            free((*table)->sequences[i]);
        } // for
        vrd_iupac_trie_destroy(&(*table)->trie);
        free(*table);
        *table = NULL;
    } // if
} // vrd_seq_table_destroy


char const*
vrd_seq_table_insert(vrd_Seq_Table* const table,
                     size_t const len,
                     char const str[len])
{
    assert(NULL != table);

    char const* const restrict result =
        vrd_iupac_trie_find(table->trie, len, str);
    if (NULL != result)
    {
        return result;
    } // if

    if (table->capacity <= table->next)
    {
        return NULL;
    } // if

    table->sequences[table->next] = malloc(len);
    if (NULL == table->sequences[table->next])
    {
        return NULL;
    } // if

    char* const restrict ptr = table->sequences[table->next];
    (void) strncpy(ptr, str, len);

    table->next += 1;

    return vrd_iupac_trie_insert(table->trie, len, str, ptr);
} // vrd_seq_table_insert


char const*
vrd_seq_table_query(vrd_Seq_Table const* const table,
                    size_t const len,
                    char const str[len])
{
    assert(NULL != table);

    return vrd_iupac_trie_find(table->trie, len, str);
} // vrd_seq_table_query
