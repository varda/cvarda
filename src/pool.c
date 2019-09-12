#include <stddef.h>     // size_t
#include <stdint.h>     // SIZE_MAX
#include <stdlib.h>     // NULL, malloc, free

#include "../include/pool.h"    // vrd_pool_*


struct Pool
{
    size_t capacity;
    size_t obj_size;
    size_t next;
    void* restrict data;
}; // Pool


struct Pool*
vrd_pool_init(size_t const capacity, size_t const obj_size)
{
    if (SIZE_MAX / obj_size <= capacity)
    {
        return NULL;
    } // if
    size_t const size = capacity * obj_size;

    struct Pool* const restrict pool = malloc(sizeof(*pool));
    if (NULL == pool)
    {
        return NULL;
    } // if

    pool->data = malloc(size);
    if (NULL == pool->data)
    {
        free(pool);
        return NULL;
    } // if

    pool->capacity = capacity;
    pool->obj_size = obj_size;
    pool->next = 0;

    return pool;
} // vrd_pool_init


void
vrd_pool_destroy(struct Pool* restrict* const restrict pool,
                 void (*dealloc)(void* restrict))
{
    if (NULL == pool || NULL == *pool)
    {
        return;
    } // if

    if (NULL != dealloc)
    {
        for (size_t i = 0; i < (*pool)->next; ++i)
        {
            dealloc(vrd_pool_deref(*pool, i));
        } // for
    } // if
    free((*pool)->data);
    free(*pool);
    *pool = NULL;
} // vrd_pool_destroy


size_t
vrd_pool_alloc(vrd_Pool* const restrict pool)
{
    if (NULL == pool || pool->capacity <= pool->next)
    {
        return -1;
    } // if

    size_t const idx = pool->next;
    pool->next += 1;
    return idx;
} // vrd_pool_alloc


void*
vrd_pool_deref(vrd_Pool const* const restrict pool, size_t const idx)
{
    if (NULL == pool || idx > pool->next)
    {
        return NULL;
    } // if

    return (char*) pool->data + idx * pool->obj_size;
} // vrd_pool_deref
