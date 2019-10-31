#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX
#include <stdio.h>      // FILE, FILENAME_MAX, fopen, fread, fwrite,
                        // snprintf
#include <stdlib.h>     // malloc, free
#include <string.h>     // strncpy

#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
#include "trie.h"  // vrd_Trie, vrd_trie_*


struct vrd_Seq_Table
{
    vrd_Trie* restrict trie;

    size_t capacity;
    size_t next;
    void* restrict sequence[];
}; // vrd_Seq_Table


vrd_Seq_Table*
vrd_seq_table_init(size_t const capacity)
{
    if ((size_t) UINT32_MAX <= capacity)
    {
        return NULL;
    } // if

    vrd_Seq_Table* const table = malloc(sizeof(*table) +
                                        sizeof(table->sequence[0]) *
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

    table->sequence[table->next] = elem;
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
vrd_seq_table_remove(vrd_Seq_Table* const restrict table,
                     size_t const elem)
{
    assert(NULL != table);

    if (table->next < elem)
    {
        return;
    } // if

    char* seq = NULL;
    size_t const len = vrd_trie_key(table->sequence[elem], &seq);
    (void) vrd_trie_remove(table->trie, len, seq);
    free(seq);
} // vrd_seq_table_remove


int
vrd_seq_table_read(vrd_Seq_Table* const restrict table,
                   char const* const restrict path)
{
    assert(NULL != table);

    char filename[FILENAME_MAX] = {'\0'};
    if (0 >= snprintf(filename, FILENAME_MAX, "%s.idx", path))
    {
        return -1;
    } // if

    char* sequence = NULL;

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
    } // for

    for (size_t i = 0; i < size; ++i)
    {
        size_t len = 0;

        errno = 0;
        count = fread(&len, sizeof(len), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        sequence = malloc(len);
        if (NULL == sequence)
        {
            goto error;
        } // if

        errno = 0;
        count = fread(sequence, 1, len, stream);
        if (len != count)
        {
            goto error;
        } // if

        if (NULL == vrd_seq_table_insert(table, len, sequence))
        {
            errno = -1;
            goto error;
        } // if

        free(sequence);
        sequence = NULL;
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
        free(sequence);

        return err;
    }
} // vrd_seq_table_read


int
vrd_seq_table_write(vrd_Seq_Table const* const restrict table,
                    char const* const restrict path)
{
    assert(NULL != table);

    char filename[FILENAME_MAX] = {'\0'};
    if (0 >= snprintf(filename, FILENAME_MAX, "%s.idx", path))
    {
        return -1;
    } // if

    char* sequence = NULL;

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
        size_t const len = vrd_trie_key(table->sequence[i], &sequence);

        errno = 0;
        count = fwrite(&len, sizeof(len), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        errno = 0;
        count = fwrite(sequence, 1, len, stream);
        if (len != count)
        {
            goto error;
        } // if

        free(sequence);
        sequence = NULL;
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
        free(sequence);

        return err;
    }
} // vrd_seq_table_write
