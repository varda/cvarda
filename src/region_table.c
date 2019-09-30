#include <stdint.h>     // uintptr_t
#include <stdlib.h>     // NULL, malloc, free

#include "../include/alloc.h"       // vrd_Alloc, vrd_malloc, vrd_pool_*
#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/region_index.h"   // vrd_region_index_*
#include "../include/region_table.h"   // vrd_Region_Table,
                                       // vrd_region_table_*
#include "../include/trie.h"        // vrd_Trie, vrd_trie_*,
                                    // VRD_ASCII_SIZE, vrd_ascii_to_idx
#include "../include/varda.h"       // VRD_MAX_*


struct Region_Table
{
    vrd_Alloc* restrict alloc;
    vrd_Trie* restrict trie;
}; // Region_Table


vrd_Region_Table*
vrd_region_table_init(void)
{
    struct Region_Table* const restrict table = malloc(sizeof(*table));
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
} // vrd_region_table_init


void
vrd_region_table_destroy(vrd_Region_Table* restrict*
                            const restrict table)
{
    if (NULL == table || NULL == *table)
    {
        return;
    } // if

    for (uintptr_t i = 1; i < vrd_pool_size((*table)->alloc); ++i)
    {
        vrd_region_index_destroy((vrd_Region_Index**)
                                  vrd_deref((*table)->alloc, (void*) i));
    } // for

    vrd_trie_destroy(&(*table)->trie);
    vrd_pool_destroy(&(*table)->alloc);
    free(*table);
    *table = NULL;
} // vrd_region_table_destroy


int
vrd_region_table_insert(vrd_Region_Table* const restrict table,
                        size_t const len,
                        char const reference[len],
                        uint32_t const start,
                        uint32_t const end,
                        uint32_t const sample_id,
                        uint32_t const phase)
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

        vrd_Region_Index* const restrict index =
            vrd_region_index_init(VRD_MAX_INDEX_SIZE,
                                  VRD_REGION_NODE_SIZE);
        if (NULL == index)
        {
            return -1;
        } // if

        *(vrd_Region_Index**) vrd_deref(table->alloc, ptr) = index;

        void* const restrict ret = vrd_trie_insert(table->trie,
                                                   len,
                                                   reference,
                                                   ptr);
        if (NULL == ret)
        {
            return -1;
        } // if
    } // if

    vrd_Region_Node const* const restrict node =
        vrd_region_index_insert(*(vrd_Region_Index**)
                                    vrd_deref(table->alloc, ptr),
                                start,
                                end,
                                sample_id,
                                phase);
    if (NULL == node)
    {
        return -1;
    } // if

    return 0;
} // vrd_region_table_insert


size_t
vrd_region_table_query(vrd_Region_Table const* const restrict table,
                       size_t const len,
                       char const reference[len],
                       uint32_t const start,
                       uint32_t const end,
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

    return vrd_region_index_query(*(vrd_Region_Index**)
                                    vrd_deref(table->alloc, ptr),
                                  start,
                                  end,
                                  subset);
} // vrd_region_table_query
