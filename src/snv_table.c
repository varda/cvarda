#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX
#include <stdio.h>      // FILE, fscanf
#include <stdlib.h>     // malloc, free
#include <string.h>     // strlen

#include "../include/ascii_trie.h"  // vrd_ASCII_Trie, vrd_ascii_trie_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*
#include "../include/snv_tree.h"    // vrd_SNV_Tree, vrd_snv_tree_*


struct vrd_SNV_Table
{
    vrd_ASCII_Trie* restrict trie;

    size_t ref_capacity;
    size_t tree_capacity;
    size_t next;
    vrd_SNV_Tree* restrict tree[];
}; // vrd_SNV_Table


vrd_SNV_Table*
vrd_snv_table_init(size_t const ref_capacity,
                   size_t const ref_size_capacity,
                   size_t const tree_capacity)
{
    if ((size_t) UINT32_MAX <= ref_capacity ||
        (size_t) UINT32_MAX <= ref_size_capacity ||
        (size_t) UINT32_MAX <= tree_capacity)
    {
        return NULL;
    } // if

    vrd_SNV_Table* const table = malloc(sizeof(*table) +
                                        sizeof(table->tree[0]) *
                                        ref_capacity);
    if (NULL == table)
    {
        return NULL;
    } // if

    table->trie = vrd_ascii_trie_init(ref_size_capacity);
    if (NULL == table->trie)
    {
        free(table);
        return NULL;
    } // if

    table->ref_capacity = ref_capacity;
    table->tree_capacity = tree_capacity;
    table->next = 0;

    return table;
} // vrd_snv_table_init


void
vrd_snv_table_destroy(vrd_SNV_Table* restrict* const table)
{
    if (NULL != table)
    {
        for (size_t i = 0; i < (*table)->next; ++i)
        {
            vrd_snv_tree_destroy(&(*table)->tree[i]);
        } // for
        vrd_ascii_trie_destroy(&(*table)->trie);
        free(*table);
        *table = NULL;
    } // if
} // vrd_snv_table_destroy


int
vrd_snv_table_insert(vrd_SNV_Table* const table,
                     size_t const len,
                     char const reference[len],
                     size_t const position,
                     size_t const sample_id,
                     size_t const phase,
                     size_t const inserted)
{
    assert(NULL != table);

    vrd_SNV_Tree* restrict tree = vrd_ascii_trie_find(table->trie,
                                                      len,
                                                      reference);
    if (NULL == tree)
    {
        if (table->ref_capacity <= table->next)
        {
            return -1;
        } // if

        table->tree[table->next] =
            vrd_snv_tree_init(table->tree_capacity);
        if (NULL == table->tree[table->next])
        {
            return -1;
        } // if

        tree = table->tree[table->next];
        table->next += 1;

        if (NULL == vrd_ascii_trie_insert(table->trie,
                                          len,
                                          reference,
                                          tree))
        {
            return -1;
        } // if
    } // if

    if (NULL == vrd_snv_tree_insert(tree,
                                    position,
                                    sample_id,
                                    phase,
                                    inserted))
    {
        return -1;
    } // if

    return 0;
} // vrd_snv_table_insert


size_t
vrd_snv_table_from_file(vrd_SNV_Table* const restrict table,
                        FILE* restrict stream,
                        size_t const sample_id)
{
    assert(NULL != table);
    assert(NULL != stream);

    // NOTE: fscanf is unsafe
    // NOTE: strlen is unsafe

    char reference[128] = {0};
    int position = 0;
    char inserted = '\0';
    int phase = 0;

    size_t lines = 0;
    while (3 == fscanf(stream, "%127s %d %c %d", reference,
                                                 &position,
                                                 &inserted,
                                                 &phase))
    {
        if (-1 == vrd_snv_table_insert(table,
                                       strlen(reference),
                                       reference,
                                       position,
                                       sample_id,
                                       phase,
                                       inserted))
        {
            break;
        } // if
        lines += 1;
    } // while
    return lines;
} // vrd_snv_table_from_file


size_t
vrd_snv_table_query(vrd_SNV_Table const* const restrict table,
                    size_t const len,
                    char const reference[len],
                    size_t const position,
                    size_t const inserted,
                    vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != table);

    vrd_SNV_Tree const* const restrict tree =
        vrd_ascii_trie_find(table->trie, len, reference);
    if (NULL == tree)
    {
        return 0;
    } // if

    return vrd_snv_tree_query(tree, position, inserted, subset);
} // vrd_snv_table_query
