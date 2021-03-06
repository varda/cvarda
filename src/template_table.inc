#include "../include/template.h"    // VRD_TEMPLATE
#ifndef VRD_TYPENAME
#error "Undefined template typename"
#endif


#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX
#include <stdio.h>      // FILE, FILENAME_MAX, flcose, fopen, fread
                        // fwrite, snprintf
#include <stdlib.h>     // free, malloc

#include "../include/diagnostics.h"     // vrd_Diagnostics
#include "../include/trie.h"    // vrd_Trie_Node, vrd_Trie, vrd_trie_*


struct VRD_TEMPLATE(VRD_TYPENAME, _Table)
{
    vrd_Trie* trie;

    size_t ref_capacity;
    size_t tree_capacity;

    size_t next;
    vrd_Trie_Node* trees[];
}; // vrd_*_Table


VRD_TEMPLATE(VRD_TYPENAME, _Table)*
VRD_TEMPLATE(VRD_TYPENAME, _table_init)(size_t const ref_capacity,
                                        size_t const tree_capacity)
{
    if ((size_t) UINT32_MAX <= ref_capacity ||
        (size_t) UINT32_MAX <= tree_capacity)
    {
        errno = -1;
        return NULL;
    } // if

    VRD_TEMPLATE(VRD_TYPENAME, _Table)* const table = malloc(sizeof(*table) + sizeof(table->trees[0]) * ref_capacity);
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
} // vrd_*_table_init


void
VRD_TEMPLATE(VRD_TYPENAME, _table_destroy)(VRD_TEMPLATE(VRD_TYPENAME, _Table)** const self)
{
    if (NULL == self || NULL == *self)
    {
        return;
    } // if

    for (size_t i = 0; i < (*self)->next; ++i)
    {
        VRD_TEMPLATE(VRD_TYPENAME, _tree_destroy)((VRD_TEMPLATE(VRD_TYPENAME, _Tree)**) &(*self)->trees[i]->data);
    } // for
    vrd_trie_destroy(&(*self)->trie);
    free(*self);
    *self = NULL;
} // vrd_*_table_destroy


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_remove)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                          vrd_AVL_Tree const* const subset)
{
    assert(NULL != self);
    assert(NULL != subset);

    size_t count = 0;
    for (size_t i = 0; i < self->next; ++i)
    {
        count += VRD_TEMPLATE(VRD_TYPENAME, _tree_remove)(self->trees[i]->data, subset);  // OVERFLOW
    } // for

    return count;
} // vrd_*_table_remove


int
VRD_TEMPLATE(VRD_TYPENAME, _table_reorder)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self)
{
    assert(NULL != self);

    for (size_t i = 0; i < self->next; ++i)
    {
        int const err = VRD_TEMPLATE(VRD_TYPENAME, _tree_reorder)(self->trees[i]->data);
        if (0 != err)
        {
            return err;
        } // if
    } // for

    return 0;
} // vrd_*_table_reorder


static VRD_TEMPLATE(VRD_TYPENAME, _Tree)*
tree_from_reference(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                    size_t const len,
                    char const reference[len])
{
    vrd_Trie_Node* elem = vrd_trie_find(self->trie, len, reference);
    if (NULL != elem)
    {
        return elem->data;
    } // if

    if (self->ref_capacity <= self->next)
    {
        errno = -1;
        return NULL;
    } // if

    VRD_TEMPLATE(VRD_TYPENAME, _Tree)* tree = VRD_TEMPLATE(VRD_TYPENAME, _tree_init)(self->tree_capacity);
    if (NULL == tree)
    {
        return NULL;
    } // if

    elem = vrd_trie_insert(self->trie, len, reference, tree);
    if (NULL == elem)
    {
        VRD_TEMPLATE(VRD_TYPENAME, _tree_destroy)(&tree);
        return NULL;
    } // if

    self->trees[self->next] = elem;
    self->next += 1;

    return elem->data;
} // tree_from_reference


static VRD_TEMPLATE(VRD_TYPENAME, _Tree)*
tree_read(char const* const path,
          size_t const idx,
          size_t const capacity)
{
    char filename[FILENAME_MAX] = {'\0'};
    size_t const buf_size = FILENAME_MAX;
    if (0 >= snprintf(filename, buf_size, "%s_tree_%zu.bin", path, idx))
    {
        return NULL;
    } // if

    VRD_TEMPLATE(VRD_TYPENAME, _Tree)* tree = NULL;

    FILE* stream = fopen(filename, "rb");
    if (NULL == stream)
    {
        goto error;
    } // if

    tree = VRD_TEMPLATE(VRD_TYPENAME, _tree_init)(capacity);
    if (NULL == tree)
    {
        goto error;
    } // if

    int const ret = VRD_TEMPLATE(VRD_TYPENAME, _tree_read)(tree, stream);
    if (0 != ret)
    {
        goto error;
    } // if

    if (0 != fclose(stream))
    {
        goto error;
    } // if

    return tree;

error:
    {
        if (NULL != stream)
        {
            (void) fclose(stream);
        } // if
        VRD_TEMPLATE(VRD_TYPENAME, _tree_destroy)(&tree);

        return NULL;
    }
} // tree_read


int
VRD_TEMPLATE(VRD_TYPENAME, _table_read)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                        char const* const path)
{
    assert(NULL != self);
    assert(NULL != path);

    char filename[FILENAME_MAX] = {'\0'};
    size_t const buf_size = FILENAME_MAX;
    if (0 >= snprintf(filename, buf_size, "%s.idx", path))
    {
        return errno;
    } // if

    FILE* stream = fopen(filename, "rb");
    if (NULL == stream)
    {
        return errno;
    } // if

    char* reference = NULL;
    size_t size = 0;
    size_t count = fread(&size, sizeof(size), 1, stream);
    if (1 != count)
    {
        goto error;
    } // if

    for (size_t i = 0; i < size; ++i)
    {
        size_t len = 0;
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

        count = fread(reference, 1, len, stream);
        if (len != count)
        {
            goto error;
        } // if

        VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const tree = tree_read(path, i, self->tree_capacity);
        if (NULL == tree)
        {
            errno = -1;
            goto error;
        } // if

        vrd_Trie_Node* const elem = vrd_trie_insert(self->trie, len, reference, tree);
        if (NULL == elem)
        {
            errno = -1;
            goto error;
        } // if

        self->trees[self->next] = elem;

        size_t idx = 0;
        count = fread(&idx, sizeof(idx), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        if (idx != self->next)
        {
            errno = -1;
            goto error;
        } // if

        self->next += 1;

        free(reference);
        reference = NULL;
    } // for

    if (0 != fclose(stream))
    {
        return errno;
    } // if

    return 0;

error:
    {
        int const err = errno;
        if (NULL != stream)
        {
            (void) fclose(stream);
        } // if
        free(reference);

        return err;
    }
} // vrd_*_table_read


int
VRD_TEMPLATE(VRD_TYPENAME, _table_write)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                         char const* const path)
{
    assert(NULL != self);
    assert(NULL != path);

    char filename[FILENAME_MAX] = {'\0'};
    size_t const buf_size = FILENAME_MAX;
    if (0 >= snprintf(filename, buf_size, "%s.idx", path))
    {
        return errno;
    } // if

    FILE* stream = fopen(filename, "wb");
    if (NULL == stream)
    {
        return errno;
    } // if

    char* reference = NULL;
    size_t count = fwrite(&self->next, sizeof(self->next), 1, stream);
    if (1 != count)
    {
        goto error;
    } // for

    for (size_t i = 0; i < self->next; ++i)
    {
        size_t const len = vrd_trie_key(self->trees[i], &reference);

        count = fwrite(&len, sizeof(len), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        count = fwrite(reference, 1, len, stream);
        if (len != count)
        {
            goto error;
        } // if

        count = fwrite(&i, sizeof(i), 1, stream);
        if (1 != count)
        {
            goto error;
        } // if

        free(reference);
        reference = NULL;
    } // for

    if (0 != fclose(stream))
    {
        return errno;
    } // if

    for (size_t i = 0; i < self->next; ++i)
    {
        if (0 >= snprintf(filename, buf_size, "%s_tree_%zu.bin", path, i))
        {
            return errno;
        } // if

        stream = fopen(filename, "wb");
        if (NULL == stream)
        {
            goto error;
        } // if

        int const ret = VRD_TEMPLATE(VRD_TYPENAME, _tree_write)(self->trees[i]->data, stream);
        if (0 != ret)
        {
            errno = ret;
            goto error;
        } // if

        if (0 != fclose(stream))
        {
            return errno;
        } // if
    } // for

    return 0;

error:
    {
        int const err = errno;
        if (NULL != stream)
        {
            (void) fclose(stream);
        } // if
        free(reference);

        return err;
    }
} // vrd_*_table_write


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_diagnostics)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                               vrd_Diagnostics** diag)
{
    assert(NULL != self);
    assert(NULL != diag);

    *diag = malloc(sizeof(**diag) * self->next);
    if (NULL == *diag)
    {
        return -1;
    } // if

    for (size_t i = 0; i < self->next; ++i)
    {
        (*diag)[i].reference = NULL;
        (void) vrd_trie_key(self->trees[i], &(*diag)[i].reference);

        vrd_Tree* const tree = self->trees[i]->data;
        (*diag)[i].entries = tree->entries;
        (*diag)[i].entry_size = tree->entry_size;
        (*diag)[i].height = tree->height;
    } // for
    return self->next;
} // vrd_*_table_diagnostics


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_sample_count)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                                size_t count[])
{
    assert(NULL != self);

    size_t max_sample_id = 0;
    for (size_t i = 0; i < self->next; ++i)
    {
        size_t const tree_max_sample_id = VRD_TEMPLATE(VRD_TYPENAME, _tree_sample_count)(self->trees[i]->data, count);
        if (tree_max_sample_id > max_sample_id)
        {
            max_sample_id = tree_max_sample_id;
        } // if
    } // for
    return max_sample_id;
} // vrd_*_table_sample_count
