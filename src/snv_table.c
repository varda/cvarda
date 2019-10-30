#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX
#include <stdio.h>      // FILE, FILENAME_MAX, fopen, fread, fwrite,
                        // snprintf
#include <stdlib.h>     // malloc, free

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*
#include "snv_tree.h"   // vrd_SNV_Tree, vrd_snv_tree_*
#include "trie.h"       // vrd_Trie, vrd_trie_*


struct vrd_SNV_Table
{
    vrd_Trie* restrict trie;

    size_t ref_capacity;
    size_t tree_capacity;
    size_t next;
    void* restrict tree[];
}; // vrd_SNV_Table


vrd_SNV_Table*
vrd_snv_table_init(size_t const ref_capacity,
                   size_t const tree_capacity)
{
    if ((size_t) UINT32_MAX <= ref_capacity ||
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
} // vrd_snv_table_init


void
vrd_snv_table_destroy(vrd_SNV_Table* restrict* const table)
{
    if (NULL == table || NULL == *table)
    {
        return;
    } // if

    for (size_t i = 0; i < (*table)->next; ++i)
    {
        vrd_snv_tree_destroy((vrd_SNV_Tree**) (*table)->tree[i]);
    } // for
    vrd_trie_destroy(&(*table)->trie);
    free(*table);
    *table = NULL;
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

    vrd_SNV_Tree* restrict tree = NULL;
    void* restrict elem = vrd_trie_find(table->trie, len, reference);
    if (NULL == elem)
    {
        if (table->ref_capacity <= table->next)
        {
            return -1;
        } // if

        tree = vrd_snv_tree_init(table->tree_capacity);
        if (NULL == tree)
        {
            return -1;
        } // if

        elem = vrd_trie_insert(table->trie, len, reference, tree);
        if (NULL == elem)
        {
            return -1;
        } // if

        table->tree[table->next] = elem;
        table->next += 1;
    } // if
    else
    {
        tree = *(vrd_SNV_Tree**) elem;
    } // else

    if (NULL == vrd_snv_tree_insert(tree, position, sample_id, phase, inserted))
    {
        return -1;
    } // if

    return 0;
} // vrd_snv_table_insert


size_t
vrd_snv_table_query(vrd_SNV_Table const* const restrict table,
                    size_t const len,
                    char const reference[len],
                    size_t const position,
                    size_t const inserted,
                    vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != table);

    void* const restrict elem = vrd_trie_find(table->trie, len, reference);
    if (NULL == elem)
    {
        return 0;
    } // if

    return vrd_snv_tree_query(*(vrd_SNV_Tree**) elem, position, inserted, subset);
} // vrd_snv_table_query


size_t
vrd_snv_table_remove(vrd_SNV_Table* const restrict table,
                     vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != table);
    assert(NULL != subset);

    size_t count = 0;
    for (size_t i = 0; i < table->next; ++i)
    {
        count += vrd_snv_tree_remove(*(vrd_SNV_Tree**) table->tree[i], subset);  // OVERFLOW
    } // for

    return count;
} // vrd_snv_table_remove


static vrd_SNV_Tree*
read_tree(char const* const restrict path,
          size_t const idx,
          size_t const capacity)
{
    char filename[FILENAME_MAX] = {'\0'};

    if (0 >= snprintf(filename, FILENAME_MAX, "%s_tree_%zu.bin", path, idx))
    {
        return NULL;
    } // if

    vrd_SNV_Tree* restrict tree = NULL;

    errno = 0;
    FILE* restrict stream = fopen(filename, "rb");
    if (NULL == stream)
    {
        goto error;
    } // if

    tree = vrd_snv_tree_init(capacity);
    if (NULL == tree)
    {
        goto error;
    } // if

    int const ret = vrd_snv_tree_read(tree, stream);
    if (0 != ret)
    {
        goto error;
    } // if

    errno = 0;
    if (0 != fclose(stream))
    {
        goto error;
    } // if

    return tree;

error:
    (void) fclose(stream);
    vrd_snv_tree_destroy(&tree);

    return NULL;
} // read_tree


int
vrd_snv_table_read(vrd_SNV_Table* const restrict table,
                   char const* const restrict path)
{
    assert(NULL != table);

    char filename[FILENAME_MAX] = {'\0'};
    if (0 >= snprintf(filename, FILENAME_MAX, "%s.idx", path))
    {
        return -1;
    } // if

    char* reference = NULL;

    errno = 0;
    FILE* restrict stream = fopen(filename, "rb");
    if (NULL == stream)
    {
        goto error;
    } // if

    size_t size = 0;

    errno = 0;
    size_t count = fread(&size, sizeof(size), 1, stream);
    if (1 != count)
    {
        goto error;
    } // if

    for (size_t i = 0; i < size; ++i)
    {
        size_t len = 0;

        errno = 0;
        count = fread(&len, sizeof(len), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        reference = malloc(len);
        if (NULL == reference)
        {
            goto error;
        } // if

        errno = 0;
        count = fread(reference, 1, len, stream);
        if (len != count)
        {
            goto error;
        } // if

        vrd_SNV_Tree* restrict tree = read_tree(path, i, table->tree_capacity);
        if (NULL == tree)
        {
            errno = -1;
            goto error;
        } // if

        void* const restrict elem = vrd_trie_insert(table->trie, len, reference, tree);
        if (NULL == elem)
        {
            errno = -1;
            goto error;
        } // if

        table->tree[table->next] = elem;

        size_t idx = 0;

        errno = 0;
        count = fread(&idx, sizeof(idx), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        if (idx != table->next)
        {
            errno = -1;
            goto error;
        } // if

        table->next += 1;

        free(reference);
        reference = NULL;
    } // for

    errno = 0;
    if (0 != fclose(stream))
    {
        return errno;
    } // if

    return 0;

error:
    {
        int const err = errno;

        errno = 0;
        (void) fclose(stream);
        free(reference);

        return err;
    }
} // vrd_snv_table_read


int
vrd_snv_table_write(vrd_SNV_Table const* const restrict table,
                    char const* const restrict path)
{
    assert(NULL != table);

    char filename[FILENAME_MAX] = {'\0'};
    if (0 >= snprintf(filename, FILENAME_MAX, "%s.idx", path))
    {
        return -1;
    } // if

    char* reference = NULL;

    errno = 0;
    FILE* restrict stream = fopen(filename, "wb");
    if (NULL == stream)
    {
        goto error;
    } // if

    errno = 0;
    size_t count = fwrite(&table->next, sizeof(table->next), 1, stream);
    if (1 != count)
    {
        goto error;
    } // for

    for (size_t i = 0; i < table->next; ++i)
    {
        size_t const len = vrd_trie_key(table->tree[i], &reference);

        errno = 0;
        count = fwrite(&len, sizeof(len), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        errno = 0;
        count = fwrite(reference, 1, len, stream);
        if (len != count)
        {
            goto error;
        } // if

        errno = 0;
        count = fwrite(&i, sizeof(i), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        free(reference);
        reference = NULL;
    } // for

    errno = 0;
    if (0 != fclose(stream))
    {
        return errno;
    } // if

    for (size_t i = 0; i < table->next; ++i)
    {
        if (0 >= snprintf(filename, FILENAME_MAX, "%s_tree_%zu.bin", path, i))
        {
            return -1;
        } // if

        errno = 0;
        stream = fopen(filename, "wb");
        if (NULL == stream)
        {
            goto error;
        } // if

        int const ret = vrd_snv_tree_write(*(vrd_SNV_Tree**) table->tree[i], stream);
        if (0 != ret)
        {
            errno = ret;
            goto error;
        } // if

        errno = 0;
        if (0 != fclose(stream))
        {
            return errno;
        } // if
    } // for

    return 0;

error:
    {
        int const err = errno;

        errno = 0;
        (void) fclose(stream);
        free(reference);

        return err;
    }
} // vrd_snv_table_write
