#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX
#include <stdio.h>      // FILE, FILENAME_MAX, fclose, fopen, fread
                        // fwrite, snprintf
#include <stdlib.h>     // free, malloc

#include "../include/seq_table.h"   // vrd_Seq_Table
#include "trie.h"   // vrd_Trie, vrd_trie_*


struct vrd_Seq_Table
{
    vrd_Trie* restrict trie;

    size_t capacity;

    size_t next;
    void* restrict sequences[];
}; // vrd_Seq_Table


vrd_Seq_Table*
vrd_Seq_table_init(size_t const capacity)
{
    if ((size_t) UINT32_MAX <= capacity)
    {
        errno = -1;
        return NULL;
    } // if

    vrd_Seq_Table* const table =
        malloc(sizeof(*table) + sizeof(table->sequences[0]) * capacity);
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
} // vrd_Seq_table_init


void
vrd_Seq_table_destroy(vrd_Seq_Table* restrict* const self)
{
    if (NULL == self)
    {
        return;
    } // if

    vrd_trie_destroy(&(*self)->trie);
    free(*self);
    *self = NULL;
} // vrd_Seq_table_destroy


void*
vrd_Seq_table_insert(vrd_Seq_Table* const self,
                     size_t const len,
                     char const sequence[len])
{
    assert(NULL != self);

    void* restrict elem = vrd_trie_find(self->trie, len, sequence);
    if (NULL != elem)
    {
        if (NULL == vrd_trie_insert(self->trie, len, sequence, elem))
        {
            errno = -1;
            return NULL;
        } // if
        return elem;
    } // if

    if (self->capacity <= self->next)
    {
        errno = -1;
        return NULL;
    } // if

    elem = vrd_trie_insert(self->trie, len, sequence, (void*) self->next);
    if (NULL == elem)
    {
        errno = -1;
        return NULL;
    } // if

    self->sequences[self->next] = elem;
    self->next += 1;

    return elem;
} // vrd_Seq_table_insert


void*
vrd_Seq_table_query(vrd_Seq_Table const* const self,
                    size_t const len,
                    char const sequence[len])
{
    assert(NULL != self);

    return vrd_trie_find(self->trie, len, sequence);
} // vrd_Seq_table_query


int
vrd_Seq_table_remove(vrd_Seq_Table* const restrict self, size_t const elem)
{
    assert(NULL != self);

    if (self->next < elem)
    {
        return -1;
    } // if

    char* sequence = NULL;
    size_t const len = vrd_trie_key(self->sequences[elem], &sequence);
    (void) vrd_trie_remove(self->trie, len, sequence);
    free(sequence);
    return 0;
} // vrd_Seq_table_remove


int
vrd_seq_table_read(vrd_Seq_Table* const restrict self,
                   char const* const restrict path)
{
    assert(NULL != self);

    char filename[FILENAME_MAX] = {'\0'};
    if (0 >= snprintf(filename, FILENAME_MAX, "%s.idx", path))
    {
        return errno;
    } // if

    char* sequence = NULL;

    FILE* restrict stream = fopen(filename, "rb");
    if (NULL == stream)
    {
        goto error;
    } // if

    size_t size = 0;

    size_t count = fread(&size, sizeof(size), 1, stream);
    if (1 != count)
    {
        goto error;
    } // for

    for (size_t i = 0; i < size; ++i)
    {
        size_t len = 0;

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

        count = fread(sequence, 1, len, stream);
        if (len != count)
        {
            goto error;
        } // if

        if (NULL == vrd_Seq_table_insert(self, len, sequence))
        {
            goto error;
        } // if

        free(sequence);
        sequence = NULL;
    } // for

    if (0 != fclose(stream))
    {
        return errno;
    } // if

    return 0;

error:
    {
        int const err = errno;
        (void) fclose(stream);
        free(sequence);

        return err;
    }
} // vrd_seq_table_read


int
vrd_Seq_table_write(vrd_Seq_Table const* const restrict self,
                    char const* const restrict path)
{
    assert(NULL != self);

    char filename[FILENAME_MAX] = {'\0'};
    if (0 >= snprintf(filename, FILENAME_MAX, "%s.idx", path))
    {
        return errno;
    } // if

    char* sequence = NULL;

    FILE* restrict stream = fopen(filename, "wb");
    if (NULL == stream)
    {
        goto error;
    } // if

    size_t count = fwrite(&self->next, sizeof(self->next), 1, stream);
    if (1 != count)
    {
        goto error;
    } // for

    for (size_t i = 0; i < self->next; ++i)
    {
        size_t const len = vrd_trie_key(self->sequences[i], &sequence);

        count = fwrite(&len, sizeof(len), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        count = fwrite(sequence, 1, len, stream);
        if (len != count)
        {
            goto error;
        } // if

        free(sequence);
        sequence = NULL;
    } // for

    if (0 != fclose(stream))
    {
        return errno;
    } // if

    return 0;

error:
    {
        int const err = errno;
        (void) fclose(stream);
        free(sequence);

        return err;
    }
} // vrd_Seq_table_write
