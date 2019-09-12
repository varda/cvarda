#ifndef POOL_H
#define POOL_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t


typedef struct Pool vrd_Pool;


vrd_Pool*
vrd_pool_init(size_t const capacity, size_t const obj_size);


void
vrd_pool_destroy(vrd_Pool* restrict* const restrict pool,
                 void (*dealloc)(void* restrict));


size_t
vrd_pool_alloc(vrd_Pool* const restrict pool);


void*
vrd_pool_deref(vrd_Pool const* const restrict pool, size_t const idx);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
