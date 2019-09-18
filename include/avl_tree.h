#ifndef AVL_TREE_H
#define AVL_TREE_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t


typedef struct AVL_Tree vrd_AVL_Tree;


vrd_AVL_Tree*
vrd_avl_init(vrd_Alloc* const restrict alloc);


void
vrd_avl_destroy(vrd_AVL_Tree* restrict* const restrict tree);


void*
vrd_avl_insert(vrd_AVL_Tree* const restrict tree,
               uint32_t const value,
               uint32_t const sample);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
