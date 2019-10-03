#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdlib.h>     // malloc, free

#include "../include/ascii_trie.h"  // vrd_ASCII_Trie, vrd_ascii_trie_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*
#include "../include/snv_tree.h"    // vrd_SNV_Tree, vrd_snv_tree_*


enum
{
    MAX_REFS = 1000,
    MAX_TREE_NODES = 1000000,
    MAX_TRIE_NODES = 1000
}; // constants


struct vrd_SNV_Table
{
    vrd_ASCII_Trie* restrict trie;

    size_t next;
    vrd_SNV_Tree* restrict tree[];
}; // vrd_SNV_Table


vrd_SNV_Table*
vrd_snv_table_init(void)
{
    vrd_SNV_Table* const table = malloc(sizeof(*table) + sizeof(table->tree[0]) * MAX_REFS);
    if (NULL == table)
    {
        return NULL;
    } // if

    table->trie = vrd_ascii_trie_init(MAX_TRIE_NODES);
    if (NULL == table->trie)
    {
        free(table);
        return NULL;
    } // if

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
                     uint32_t const position,
                     uint32_t const sample_id,
                     uint32_t const phase,
                     uint32_t const type)
{
    assert(NULL != table);

    vrd_SNV_Tree* restrict tree = vrd_ascii_trie_find(table->trie, len, reference);
    if (NULL == tree)
    {
        if (MAX_REFS <= table->next)
        {
            return -1;
        } // if

        table->tree[table->next] = vrd_snv_tree_init(MAX_TREE_NODES);
        if (NULL == table->tree[table->next])
        {
            return -1;
        } // if

        tree = table->tree[table->next];
        table->next += 1;

        if (NULL == vrd_ascii_trie_insert(table->trie, len, reference, tree))
        {
            return -1;
        } // if
    } // if

    if (NULL == vrd_snv_tree_insert(tree, position, sample_id, phase, type))
    {
        return -1;
    } // if

    return 0;
} // vrd_snv_table_insert
