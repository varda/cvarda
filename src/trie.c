#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdlib.h>     // malloc, free, realloc
#include <string.h>     // strncpy

#include "trie.h"    // vrd_Trie, vrd_trie_*


struct Node
{
    void* restrict data;
    size_t count;
    char* restrict key;
    size_t len;
    struct Node* restrict par;
    struct Node* restrict link;
    struct Node* restrict next;
}; // Node


struct vrd_Trie
{
    struct Node* root;
}; // vrd_Trie


vrd_Trie*
vrd_trie_init(void)
{
    vrd_Trie* const trie = malloc(sizeof(*trie));
    if (trie == NULL)
    {
        return NULL;
    } // if

    trie->root = NULL;
    return trie;
} // vrd_trie_init


static void
node_destroy(struct Node* const node)
{
    free(node->key);
    free(node);
} // node_destroy


static void
trie_destroy(struct Node* const root)
{
    if (NULL == root)
    {
        return;
    } // if

    trie_destroy(root->link);
    trie_destroy(root->next);
    node_destroy(root);
} // trie_destroy


void
vrd_trie_destroy(vrd_Trie* restrict* const trie)
{
    if (NULL == trie || NULL == *trie)
    {
        return;
    } // if

    trie_destroy((*trie)->root);
    free(*trie);
    *trie = NULL;
} // vrd_trie_destroy


static struct Node*
node_init(size_t const len,
          char const key[len],
          void* const restrict data)
{
    struct Node* const restrict node = malloc(sizeof(*node));
    if (NULL == node)
    {
        return NULL;
    } // if

    node->key = malloc(len);
    if (NULL == node->key)
    {
        free(node);
        return NULL;
    } // if

    (void) strncpy(node->key, key, len);
    node->len = len;
    node->par = NULL;
    node->link = NULL;
    node->next = NULL;

    node->count = 0;
    node->data = data;

    return node;
} // node_init


static inline size_t
prefix(size_t const len_a,
       char const str_a[len_a],
       size_t const len_b,
       char const str_b[len_b])
{
    for (size_t i = 0; i < len_a; ++i)
    {
        if (i >= len_b || str_a[i] != str_b[i])
        {
            return i;
        } // if
    } // for
    return len_a;
} // prefix


static struct Node*
node_split(struct Node* const restrict node, size_t const k)
{
    struct Node* const restrict split = node_init(k, node->key, NULL);
    if (NULL == split)
    {
        return NULL;
    } // if

    char* const restrict key = malloc(node->len - k);
    if (NULL == key)
    {
        free(node);
        return NULL;
    } // if

    (void) strncpy(key, &node->key[k], node->len - k);
    free(node->key);
    node->key = key;
    node->len -= k;

    split->par = node->par;
    split->link = node;
    split->next = node->next;
    node->par = split;
    node->next = NULL;

    return split;
} // node_split


static struct Node*
trie_insert(struct Node* const restrict root,
            size_t const len,
            char const key[len],
            void* const restrict data)
{
    if (NULL == root)
    {
        struct Node* const restrict node = node_init(len, key, data);
        node->count = 1;
        return node;
    } // if

    size_t const k = prefix(len, key, root->len, root->key);
    if (0 == k)
    {
        struct Node* const restrict node =
            trie_insert(root->next, len, key, data);
        if (NULL == node)
        {
            return NULL;
        } // if

        node->par = root->par;

        root->next = node;
        return root;
    } // if

    if (len == k)
    {
        root->count += 1; // OVERFLOW
        return root;
    } // if

    struct Node* restrict sub = root;
    if (root->len > k)
    {
        struct Node* const restrict node = node_split(root, k);
        if (NULL == node)
        {
            return NULL;
        } // if
        sub = node;
    } // if

    struct Node* const restrict node =
        trie_insert(sub->link, len - k, &key[k], data);
    if (NULL == node)
    {
        return NULL;
    } // if
    node->par = sub;

    sub->link = node;
    return sub;
} // trie_insert


static struct Node*
node_join(struct Node* const restrict node)
{
    struct Node* const restrict join = node->link;

    char* const restrict key = malloc(node->len + join->len);
    if (NULL == key)
    {
        return NULL;
    } // if

    (void) strncpy(key, node->key, node->len);
    (void) strncpy(&key[node->len], join->key, join->len);
    free(join->key);

    join->par = node->par;
    join->next = node->next;
    join->key = key;
    join->len += node->len;

    node_destroy(node);

    return join;
} // node_join


struct Node*
trie_remove(struct Node* const restrict root,
            size_t const len,
            char const key[len])
{
    if (NULL == root)
    {
        return NULL;
    } // if

    size_t const k = prefix(len, key, root->len, root->key);
    if (0 == k)
    {
        root->next = trie_remove(root->next, len, key);
        return root;
    } // if

    if (len == k)
    {
        root->count -= 1;
        if (0 == root->count)
        {
            struct Node* const restrict node = root->next;
            node_destroy(root);
            return node;
        } // if
        return root;
    } // if

    if (root->len == k)
    {
        root->link = trie_remove(root->link, len - k, &key[k]);
        if (NULL != root->link && NULL == root->link->next)
        {
            struct Node* const restrict node = node_join(root);
            if (NULL == node)
            {
                return NULL;
            } // if
            return node;
        } // if
    } // if

    return root;
} // trie_remove


struct Node*
trie_find(struct Node* const root,
          size_t const len,
          char const key[len])
{
    if (NULL == root)
    {
        return NULL;
    } // if

    size_t const k = prefix(len, key, root->len, root->key);
    if (0 == k)
    {
        return trie_find(root->next, len, key);
    } // if
    if (len == k)
    {
        return root;
    } // if
    if (root->len == k)
    {
        return trie_find(root->link, len - k, &key[k]);
    } // if
    return NULL;
} // trie_find


void*
vrd_trie_insert(vrd_Trie* const restrict trie,
                size_t const len,
                char const key[len],
                void* const restrict data)
{
    assert(NULL != trie);

    trie->root = trie_insert(trie->root, len, key, data);
    return trie_find(trie->root, len, key);
} // vrd_trie_insert


void
vrd_trie_remove(vrd_Trie* const restrict trie,
                size_t const len,
                char const key[len])
{
    assert(NULL != trie);

    trie->root = trie_remove(trie->root, len, key);
} // vrd_trie_remove


void*
vrd_trie_find(vrd_Trie const* const trie,
              size_t const len,
              char const key[len])
{
    assert(NULL != trie);

    return trie_find(trie->root, len, key);
} // vrd_trie_find


static size_t
trie_key(struct Node const* const restrict node, char** restrict key)
{
    if (NULL == node)
    {
        return 0;
    } // if

    size_t const len = trie_key(node->par, key);
    *key = realloc(*key, len + node->len);
    if (NULL == *key)
    {
        free(*key);
        return 0;
    } // if
    (void) strncpy(&(*key)[len], node->key, node->len);
    return len + node->len;
} // trie_key


size_t
vrd_trie_key(void* const restrict ptr, char** restrict key)
{
    return trie_key(ptr, key);
} // vrd_trie_key
