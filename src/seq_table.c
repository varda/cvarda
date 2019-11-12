#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX
#include <stdio.h>      // FILE, FILENAME_MAX, fclose, fopen, fread
                        // fwrite, snprintf
#include <stdlib.h>     // free, malloc

#include "../include/seq_table.h"   // vrd_Seq_Table
#include "trie.h"   // vrd_Trie, vrd_trie_*


struct Free_Node
{
    size_t start;
    size_t end;
    struct Free_Node* next;
}; // Free_Node


struct vrd_Seq_Table
{
    vrd_Trie* trie;

    size_t capacity;
    struct Free_Node* free_list;

    void* sequences[];
}; // vrd_Seq_Table


static struct Free_Node*
free_node_init(size_t const start,
               size_t const end,
               struct Free_Node* const next)
{
    struct Free_Node* const node = malloc(sizeof(*node));
    if (NULL == node)
    {
        return NULL;
    } // if

    node->start = start;
    node->end = end;
    node->next = next;

    return node;
} // free_node_init


static void
free_list_destroy(struct Free_Node** const free_list)
{
    if (NULL == free_list)
    {
        return;
    } // if

    while (NULL != *free_list)
    {
        struct Free_Node* const tmp = *free_list;
        *free_list = (*free_list)->next;
        free(tmp);
    } // while

    *free_list = NULL;
} // free_list_destroy


static size_t
free_list_alloc(struct Free_Node** const free_list)
{
    if (NULL == *free_list)
    {
        return -1;
    } // if

    size_t const idx = (*free_list)->start;

    (*free_list)->start += 1;
    if ((*free_list)->start >= (*free_list)->end)
    {
        struct Free_Node* const tmp = (*free_list)->next;
        free(*free_list);
        *free_list = tmp;
    } // if
    return idx;
} // free_list_alloc


static struct Free_Node*
free_list_dealloc(struct Free_Node* const free_list, size_t const idx)
{
    if (NULL == free_list || free_list->start >= idx)
    {
        if (NULL != free_list && idx >= free_list->start && idx < free_list->end)
        {
            return free_list;
        } // if

        if (NULL != free_list && idx + 1 == free_list->start)
        {
            free_list->start -= 1;
            return free_list;
        } // if

        return free_node_init(idx, idx + 1, free_list);
    } // if

    struct Free_Node* tmp = free_list;
    while (NULL != tmp->next && tmp->next->start < idx)
    {
        tmp = tmp->next;
    } // while

    if (idx >= tmp->start && idx < tmp->end)
    {
        return free_list;
    } // if

    if (idx == tmp->end)
    {
        tmp->end += 1;
        return free_list;
    } // if

    if (NULL != tmp->next && idx + 1 == tmp->next->start)
    {
        tmp->next->start -= 1;
        return free_list;
    } // if

    if (NULL != tmp->next && idx >= tmp->next->start && idx < tmp->next->end)
    {
        return free_list;
    } // if

    if (NULL != tmp->next && idx == tmp->next->end)
    {
        tmp->next->end += 1;
        return free_list;
    } // if

    tmp->next = free_node_init(idx, idx + 1, tmp->next);

    return free_list;
} // free_list_dealloc


static size_t
free_list_max(struct Free_Node const* const head, size_t const capacity)
{
    if (NULL == head)
    {
        return capacity;
    } // if

    struct Free_Node const* tmp = head;

    size_t idx = tmp->start;
    while (NULL != tmp->next)
    {
        tmp = tmp->next;
        idx = tmp->start;
    } // while

    return idx;
} // free_list_max


// DEBUG >>>
static void
free_list_print(struct Free_Node const* const head)
{
    struct Free_Node const* tmp = head;

    (void) fprintf(stderr, "[\n");
    while (NULL != tmp)
    {
        (void) fprintf(stderr, "  %zu--%zu\n", tmp->start, tmp->end);
        tmp = tmp->next;
    } // while
    (void) fprintf(stderr, "]\n");
} // free_list_print


void
vrd_Seq_table_free_list_print(vrd_Seq_Table const* const self)
{
    assert(NULL != self);

    free_list_print(self->free_list);
} // vrd_Seq_table_free_list_print
// <<< END DEBUG


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
    table->free_list = free_node_init(0, capacity, NULL);
    if (NULL == table->free_list)
    {
        vrd_trie_destroy(&table->trie);
        free(table);
        return NULL;
    } // if

    return table;
} // vrd_Seq_table_init


void
vrd_Seq_table_destroy(vrd_Seq_Table** const self)
{
    if (NULL == self)
    {
        return;
    } // if

    vrd_trie_destroy(&(*self)->trie);
    free_list_destroy(&(*self)->free_list);
    free(*self);
    *self = NULL;
} // vrd_Seq_table_destroy


void*
vrd_Seq_table_insert(vrd_Seq_Table* const self,
                     size_t const len,
                     char const sequence[len])
{
    assert(NULL != self);

    void* elem = vrd_trie_find(self->trie, len, sequence);
    if (NULL != elem)
    {
        if (NULL == vrd_trie_insert(self->trie, len, sequence, elem))
        {
            errno = -1;
            return NULL;
        } // if
        return elem;
    } // if

    size_t const idx = free_list_alloc(&self->free_list);
    if ((size_t) -1 == idx)
    {
        errno = -1;
        return NULL;
    } // if

    elem = vrd_trie_insert(self->trie, len, sequence, (void*) idx);
    if (NULL == elem)
    {
        errno = -1;
        return NULL;
    } // if

    self->sequences[idx] = elem;

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
vrd_Seq_table_remove(vrd_Seq_Table* const self, size_t const elem)
{
    assert(NULL != self);

    if (self->capacity <= elem)
    {
        return -1;
    } // if

    self->free_list = free_list_dealloc(self->free_list, elem);

    char* sequence = NULL;
    size_t const len = vrd_trie_key(self->sequences[elem], &sequence);
    (void) vrd_trie_remove(self->trie, len, sequence);
    free(sequence);
    self->sequences[elem] = NULL;
    return 0;
} // vrd_Seq_table_remove


int
vrd_Seq_table_read(vrd_Seq_Table* const self,
                   char const* const path)
{
    assert(NULL != self);

    char filename[FILENAME_MAX] = {'\0'};
    size_t const buf_size = FILENAME_MAX;
    if (0 >= snprintf(filename, buf_size, "%s.idx", path))
    {
        return errno;
    } // if

    char* sequence = NULL;
    FILE* stream = fopen(filename, "rb");
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

    if (size > self->capacity)
    {
        errno = -1;
        goto error;
    } // if

    size_t last_idx = 0;
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

        size_t idx = 0;
        count = fread(&idx, sizeof(idx), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        for (; last_idx < idx; ++last_idx)
        {
            self->free_list = free_list_dealloc(self->free_list, last_idx);
        } // for

        void* const elem = vrd_trie_insert(self->trie, len, sequence, (void*) idx);
        if (NULL == elem)
        {
            errno = -1;
            goto error;
        } // if

        self->sequences[idx] = elem;

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
} // vrd_Seq_table_read


int
vrd_Seq_table_write(vrd_Seq_Table const* const self,
                    char const* const path)
{
    assert(NULL != self);

    char filename[FILENAME_MAX] = {'\0'};
    size_t const buf_size = FILENAME_MAX;
    if (0 >= snprintf(filename, buf_size, "%s.idx", path))
    {
        return errno;
    } // if

    char* sequence = NULL;
    FILE* stream = fopen(filename, "wb");
    if (NULL == stream)
    {
        goto error;
    } // if

    size_t const size = free_list_max(self->free_list, self->capacity);

    size_t count = fwrite(&size, sizeof(size), 1, stream);
    if (1 != count)
    {
        goto error;
    } // for

    for (size_t i = 0; i < size; ++i)
    {
        if (NULL != self->sequences[i])
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

            count = fwrite(&i, sizeof(i), 1, stream);
            if (1 != count)
            {
                goto error;
            } // if

            free(sequence);
            sequence = NULL;
        } // if
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
