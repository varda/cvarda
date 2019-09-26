#include <stdint.h>     // uintptr_t
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // NULL, malloc, free

#include "../include/alloc.h"       // vrd_Alloc, vrd_malloc, vrd_pool_*
#include "../include/snv_index.h"   // vrd_snv_index_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*
#include "../include/trie.h"        // vrd_Trie, vrd_trie_*,
                                    // VRD_ASCII_SIZE, vrd_ascii_to_idx


struct SNV_Table
{
    vrd_Alloc* restrict alloc;
    vrd_Trie* restrict trie;
}; // SNV_Table


vrd_SNV_Table*
vrd_snv_table_init(void)
{
    struct SNV_Table* const restrict table = malloc(sizeof(*table));
    if (NULL == table)
    {
        return NULL;
    } // if

    table->alloc = vrd_pool_init(100, sizeof(vrd_SNV_Index*));
    if (NULL == table->alloc)
    {
        free(table);
        return NULL;
    } // if

    table->trie = vrd_trie_init(&vrd_malloc,
                                VRD_ASCII_SIZE,
                                vrd_ascii_to_idx);
    if (NULL == table->trie)
    {
        vrd_pool_destroy(&table->alloc);
        free(table);
        return NULL;
    } // if

    return table;
} // vrd_snv_table_init


void
vrd_snv_table_destroy(vrd_SNV_Table* restrict* const restrict table)
{
    if (NULL == table || NULL == *table)
    {
        return;
    } // if

    for (uintptr_t i = 1; i < vrd_pool_size((*table)->alloc); ++i)
    {
        vrd_snv_index_destroy(vrd_deref((*table)->alloc, (void*) i));
    } // for

    vrd_trie_destroy(&(*table)->trie);
    vrd_pool_destroy(&(*table)->alloc);
    free(*table);
    *table = NULL;
} // vrd_snv_table_destroy


int
vrd_snv_table_insert(vrd_SNV_Table* const restrict table,
                     size_t const len,
                     char const reference[len],
                     uint32_t const position,
                     uint32_t const sample_id,
                     uint32_t const phase,
                     uint32_t const type)
{
    if (NULL == table)
    {
        return -1;
    } // if

    vrd_SNV_Index* restrict index = vrd_trie_find(table->trie,
                                                  len,
                                                  reference);
    if (NULL == index)
    {
        index = vrd_alloc(table->alloc, sizeof(vrd_SNV_Index*));
        vrd_deref(table->alloc, index) = vrd_snv_index_init(100);
        if (NULL == index)
        {
            fprintf(stderr, "could not init index\n");
            return -1;
        } // if
        void* const restrict ret = vrd_trie_insert(table->trie,
                                                   len,
                                                   reference,
                                                   index);
        if (NULL == ret)
        {
            free(index);
            return -1;
        } // if
    } // if

    return vrd_snv_index_insert(index, position, sample_id, phase, type);
} // vrd_snv_table_insert
