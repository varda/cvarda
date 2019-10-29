#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX
#include <stdlib.h>     // malloc, free
#include <string.h>     // strncpy

#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
#include "trie.h"  // vrd_Trie, vrd_trie_*


struct vrd_Seq_Table
{
    vrd_Trie* restrict trie;

    size_t capacity;
    size_t next;
    void* restrict sequences[];
}; // vrd_Seq_Table


vrd_Seq_Table*
vrd_seq_table_init(size_t const capacity)
{
    if ((size_t) UINT32_MAX <= capacity)
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

    table->trie = vrd_trie_init();
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
    if (NULL == table)
    {
        return;
    } // if

    vrd_trie_destroy(&(*table)->trie);
    free(*table);
    *table = NULL;
} // vrd_seq_table_destroy


void*
vrd_seq_table_insert(vrd_Seq_Table* const table,
                     size_t const len,
                     char const seq[len])
{
    assert(NULL != table);

    void* restrict elem = vrd_trie_find(table->trie, len, seq);
    if (NULL != elem)
    {
        if (NULL == vrd_trie_insert(table->trie, len, seq, elem))
        {
            return NULL;
        } // if
        return elem;
    } // if

    if (table->capacity <= table->next)
    {
        return NULL;
    } // if

    elem = vrd_trie_insert(table->trie, len, seq, (void*) table->next);
    if (NULL == elem)
    {
        return NULL;
    } // if

    elem = vrd_trie_find(table->trie, len, seq);
    if (NULL == elem)
    {
        return NULL;
    } // if

    table->sequences[table->next] = elem;
    table->next += 1;

    return elem;
} // vrd_seq_table_insert


void*
vrd_seq_table_query(vrd_Seq_Table const* const table,
                    size_t const len,
                    char const seq[len])
{
    assert(NULL != table);

    return vrd_trie_find(table->trie, len, seq);
} // vrd_seq_table_query


void
vrd_seq_table_remove(vrd_Seq_Table* const table,
                     size_t const len,
                     char const seq[len])
{
    assert(NULL != table);

    (void) vrd_trie_remove(table->trie, len, seq);
} // vrd_seq_table_remove
