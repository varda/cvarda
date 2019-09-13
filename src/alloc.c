#include <stddef.h>     // size_t
#include <stdint.h>     // SIZE_MAX
#include <stdlib.h>     // malloc, free

#include "../include/alloc.h"   // vrd_Alloc, vrd_alloc, vrd_dealloc,
                                // vrd_deref, vrd_pool_*


struct Alloc
{
    void* (*alloc)(struct Alloc* const restrict,
                   size_t const);
    void (*dealloc)(struct Alloc* const restrict,
                    void* const restrict);
    void* (*deref)(struct Alloc const* const restrict,
                   void* const restrict);
}; // Alloc


static inline void*
std_malloc(struct Alloc* const restrict alloc,
           size_t const size)
{
    (void) alloc;
    return malloc(size);
} // std_malloc


static inline void
std_free(struct Alloc* const restrict alloc,
         void* const restrict ptr)
{
    (void) alloc;
    free(ptr);
} // std_free


static inline void*
std_deref(struct Alloc const* const restrict alloc,
          void* const restrict ptr)
{
    (void) alloc;
    return ptr;
} // std_deref


vrd_Alloc vrd_malloc =
{
    .alloc = std_malloc,
    .dealloc = std_free,
    .deref = std_deref
}; // vrd_malloc


void*
vrd_alloc(struct Alloc* const restrict alloc,
          size_t const size)
{
    return alloc->alloc(alloc, size);
} // vrd_alloc


void
vrd_dealloc(struct Alloc* const restrict alloc,
            void* const restrict ptr)
{
    alloc->dealloc(alloc, ptr);
} // vrd_dealloc


void*
vrd_deref(struct Alloc const* const restrict alloc,
          void* const restrict ptr)
{
    return alloc->deref(alloc, ptr);
} // vrd_deref


struct Pool
{
    vrd_Alloc base;
    size_t capacity;
    size_t obj_size;
    size_t next;
    void* restrict pool;
}; // Pool


static inline void*
pool_alloc(struct Alloc* const restrict alloc,
           size_t const num)
{
    (void) num;
    struct Pool* const restrict pool = (struct Pool*) alloc;

    if (NULL == pool || pool->capacity <= pool->next)
    {
        return NULL;
    } // if

    void* const restrict ptr = (void*) pool->next;
    pool->next += 1;
    return ptr;
} // pool_alloc


static inline void
pool_dealloc(struct Alloc* const restrict alloc,
             void* const restrict ptr)
{
    (void) alloc;
    (void) ptr;
} // pool_dealloc


static inline void*
pool_deref(struct Alloc const* const restrict alloc,
           void* const restrict ptr)
{
    struct Pool* const restrict pool = (struct Pool*) alloc;
    size_t const idx = (size_t) ptr;

    if (NULL == pool || pool->next <= idx)
    {
        return NULL;
    } // if

    return (char*) pool->pool + idx * pool->obj_size;
} // pool_alloc


vrd_Alloc*
vrd_pool_init(size_t const capacity, size_t const obj_size)
{
    if (SIZE_MAX / obj_size <= capacity)
    {
        return NULL;
    } // if
    size_t const size = capacity * obj_size;

    struct Pool* const restrict alloc = malloc(sizeof(*alloc));
    if (NULL == alloc)
    {
        return NULL;
    } // if

    alloc->base.alloc = pool_alloc;
    alloc->base.dealloc = pool_dealloc;
    alloc->base.deref = pool_deref;

    alloc->pool = malloc(size);
    if (NULL == alloc->pool)
    {
        free(alloc);
        return NULL;
    } // if

    alloc->capacity = capacity;
    alloc->obj_size = obj_size;
    alloc->next = 0;

    return (vrd_Alloc*) alloc;
} // vrd_pool_init


void
vrd_pool_destroy(vrd_Alloc* restrict* const restrict alloc)
{
    struct Pool* restrict* const restrict pool = (struct Pool**) alloc;

    if (NULL == pool || NULL == *pool)
    {
        return;
    } // if

    free((*pool)->pool);
    free(*pool);
    *pool = NULL;
} // vrd_pool_destroy
