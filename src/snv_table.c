#include <stdint.h>     // uintptr_t
#include <stdlib.h>     // NULL, malloc, free

#include "../include/alloc.h"       // vrd_Alloc, vrd_malloc, vrd_pool_*
#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/snv_index.h"   // vrd_snv_index_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*
#include "../include/trie.h"        // vrd_Trie, vrd_trie_*,
                                    // VRD_ASCII_SIZE, vrd_ascii_to_idx
#include "../include/varda.h"       // VRD_MAX_*


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
        vrd_snv_index_destroy((vrd_SNV_Index**)
                                  vrd_deref((*table)->alloc, (void*) i));
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

    // FIXME: range checks on data

    void* restrict ptr = vrd_trie_find(table->trie, len, reference);
    if (NULL == ptr)
    {
        if (VRD_MAX_REFERENCES <= vrd_pool_size(table->alloc))
        {
            return -1;
        } // if
        ptr = vrd_alloc(table->alloc, sizeof(void*));

        vrd_SNV_Index* const restrict index =
            vrd_snv_index_init(VRD_MAX_INDEX_SIZE);
        if (NULL == index)
        {
            return -1;
        } // if

        *(vrd_SNV_Index**) vrd_deref(table->alloc, ptr) = index;

        void* const restrict ret = vrd_trie_insert(table->trie,
                                                   len,
                                                   reference,
                                                   ptr);
        if (NULL == ret)
        {
            return -1;
        } // if
    } // if

    return vrd_snv_index_insert(*(vrd_SNV_Index**)
                                    vrd_deref(table->alloc, ptr),
                                position,
                                sample_id,
                                phase,
                                type);
} // vrd_snv_table_insert


size_t
vrd_snv_table_query(vrd_SNV_Table const* const restrict table,
                    size_t const len,
                    char const reference[len],
                    uint32_t const start,
                    uint32_t const type,
                    vrd_AVL_Tree const* const restrict subset)
{
    if (NULL == table)
    {
        return 0;
    } // if

    void* restrict ptr = vrd_trie_find(table->trie, len, reference);
    if (NULL == ptr)
    {
        return 0;
    } // if

    return vrd_snv_index_query(*(vrd_SNV_Index**)
                               vrd_deref(table->alloc, ptr),
                               start,
                               type,
                               subset);
} // vrd_snv_table_query
