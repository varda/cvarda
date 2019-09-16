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
vrd_avl_init(size_t const capacity);


void
vrd_avl_destroy(vrd_AVL_Tree* restrict* const restrict tree);


uint32_t
vrd_avl_insert(vrd_AVL_Tree* const restrict tree,
               uint32_t const value,
               uint32_t const sample);


#include <stdio.h>      // FILE*


size_t
vrd_avl_print(FILE* const restrict stream,
              vrd_AVL_Tree const* const restrict tree);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
