#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX
#include <stdlib.h>     // malloc, free

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "mnv_tree.h"   // vrd_MNV_Tree, vrd_mnv_tree_*
#include "trie.h"       // vrd_Trie, vrd_trie_*


struct vrd_MNV_Table
{
    vrd_Trie* restrict trie;

    size_t ref_capacity;
    size_t tree_capacity;
    size_t next;
    vrd_MNV_Tree* restrict tree[];
}; // vrd_MNV_Table


vrd_MNV_Table*
vrd_mnv_table_init(size_t const ref_capacity,
                   size_t const tree_capacity)
{
    if ((size_t) UINT32_MAX <= ref_capacity ||
        (size_t) UINT32_MAX <= tree_capacity)
    {
        return NULL;
    } // if

    vrd_MNV_Table* const table = malloc(sizeof(*table) +
                                        sizeof(table->tree[0]) *
                                        ref_capacity);
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

    table->ref_capacity = ref_capacity;
    table->tree_capacity = tree_capacity;
    table->next = 0;

    return table;
} // vrd_mnv_table_init


void
vrd_mnv_table_destroy(vrd_MNV_Table* restrict* const table)
{
    if (NULL == table)
    {
        return;
    } // if

    for (size_t i = 0; i < (*table)->next; ++i)
    {
        vrd_mnv_tree_destroy(&(*table)->tree[i]);
    } // for
    vrd_trie_destroy(&(*table)->trie);
    free(*table);
    *table = NULL;
} // vrd_mnv_table_destroy


int
vrd_mnv_table_insert(vrd_MNV_Table* const restrict table,
                     size_t const len,
                     char const reference[len],
                     size_t const start,
                     size_t const end,
                     size_t const sample_id,
                     size_t const phase,
                     size_t const inserted)
{
    assert(NULL != table);

    vrd_MNV_Tree* restrict tree = NULL;
    void* const restrict elem = vrd_trie_find(table->trie, len, reference);
    if (NULL == elem)
    {
        if (table->ref_capacity <= table->next)
        {
            return -1;
        } // if

        table->tree[table->next] = vrd_mnv_tree_init(table->tree_capacity);
        if (NULL == table->tree[table->next])
        {
            return -1;
        } // if

        tree = table->tree[table->next];
        table->next += 1;

        if (NULL == vrd_trie_insert(table->trie, len, reference, tree))
        {
            return -1;
        } // if
    } // if
    else
    {
        tree = *(vrd_MNV_Tree**) elem;
    } // else

    if (NULL == vrd_mnv_tree_insert(tree, start, end, sample_id, phase, inserted))
    {
        return -1;
    } // if

    return 0;
} // vrd_mnv_table_insert


size_t
vrd_mnv_table_query(vrd_MNV_Table const* const restrict table,
                    size_t const len,
                    char const reference[len],
                    size_t const start,
                    size_t const end,
                    size_t const inserted,
                    vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != table);

    void* const restrict elem = vrd_trie_find(table->trie, len, reference);
    if (NULL == elem)
    {
        return 0;
    } // if

    return vrd_mnv_tree_query(*(vrd_MNV_Tree**) elem, start, end, inserted, subset);
} // vrd_mnv_table_query


size_t
vrd_mnv_table_remove(vrd_MNV_Table* const restrict table,
                     vrd_AVL_Tree const* const restrict subset,
                     vrd_Seq_Table* const restrict seq_table)
{
    assert(NULL != table);
    assert(NULL != subset);

    size_t count = 0;
    for (size_t i = 0; i < table->next; ++i)
    {
        count += vrd_mnv_tree_remove(table->tree[i], subset, seq_table); // OVERFLOW
    } // for

    return count;
} // vrd_mnv_table_remove
