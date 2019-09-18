#ifndef ALLOC_H
#define ALLOC_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t


typedef struct Alloc vrd_Alloc;


void*
vrd_alloc(vrd_Alloc* const restrict alloc,
          size_t const size);


void
vrd_dealloc(vrd_Alloc* const restrict alloc,
            void* const restrict ptr);


void*
vrd_deref(vrd_Alloc const* const restrict alloc,
          void* const restrict ptr);


extern vrd_Alloc const vrd_malloc;


vrd_Alloc*
vrd_pool_init(size_t const capacity,
              size_t const obj_size);


void
vrd_pool_destroy(vrd_Alloc* restrict* const restrict alloc);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
