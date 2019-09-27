#include <stdint.h>     // uintptr_t
#include <stdlib.h>     // NULL, malloc, free

#include "../include/alloc.h"       // vrd_Alloc, vrd_malloc, vrd_pool_*
#include "../include/mnv_index.h"   // vrd_mnv_index_*
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "../include/trie.h"        // vrd_Trie, vrd_trie_*,
                                    // VRD_ASCII_SIZE, vrd_ascii_to_idx
#include "../include/varda.h"       // VRD_MAX_*


struct MNV_Table
{
    vrd_Alloc* restrict alloc;
    vrd_Trie* restrict trie;
}; // MNV_Table


vrd_MNV_Table*
vrd_mnv_table_init(void)
{
    struct MNV_Table* const restrict table = malloc(sizeof(*table));
    if (NULL == table)
    {
        return NULL;
    } // if

    table->alloc = vrd_pool_init(VRD_MAX_REFERENCES, sizeof(void*));
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
} // vrd_mnv_table_init


void
vrd_mnv_table_destroy(vrd_MNV_Table* restrict* const restrict table)
{
    if (NULL == table || NULL == *table)
    {
        return;
    } // if

    for (uintptr_t i = 1; i < vrd_pool_size((*table)->alloc); ++i)
    {
        vrd_mnv_index_destroy((vrd_MNV_Index**)
                                  vrd_deref((*table)->alloc, (void*) i));
    } // for

    vrd_trie_destroy(&(*table)->trie);
    vrd_pool_destroy(&(*table)->alloc);
    free(*table);
    *table = NULL;
} // vrd_mnv_table_destroy


int
vrd_mnv_table_insert(vrd_MNV_Table* const restrict table,
                     size_t const len,
                     char const reference[len],
                     uint32_t const start,
                     uint32_t const end,
                     uint32_t const sample_id,
                     uint32_t const phase,
                     void* const restrict inserted)
{
    if (NULL == table)
    {
        return -1;
    } // if

    // FIXME: range checks on data

    void* restrict ptr = vrd_trie_find(table->trie, len, reference);
    if (NULL == ptr)
    {
        if (VRD_MAX_REFERENCES <= vrd_pool_size(table->alloc))
        {
            return -1;
        } // if
        ptr = vrd_alloc(table->alloc, sizeof(void*));

        vrd_MNV_Index* const restrict index =
            vrd_mnv_index_init(VRD_MAX_INDEX_SIZE);
        if (NULL == index)
        {
            return -1;
        } // if

        *(vrd_MNV_Index**) vrd_deref(table->alloc, ptr) = index;

        void* const restrict ret = vrd_trie_insert(table->trie,
                                                   len,
                                                   reference,
                                                   ptr);
        if (NULL == ret)
        {
            return -1;
        } // if
    } // if

    return vrd_mnv_index_insert(*(vrd_MNV_Index**)
                                    vrd_deref(table->alloc, ptr),
                                start,
                                end,
                                sample_id,
                                phase,
                                inserted);
} // vrd_mnv_table_insert
