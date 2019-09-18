#ifndef AVL_TREE_H
#define AVL_TREE_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t, int32_t

#include <stdio.h>      // FIXME: DEBUG FILE*


typedef struct AVL_Tree vrd_AVL_Tree;

typedef struct vrd_AVL_Node
{
    uint32_t child[2];
    uint32_t value;
    int32_t balance :  3;  // [-2, .., 2]
    uint32_t extra  : 29;
} vrd_AVL_Node;



vrd_AVL_Tree*
vrd_avl_init(vrd_Alloc* const restrict alloc);


void
vrd_avl_destroy(vrd_AVL_Tree* restrict* const restrict tree);


vrd_AVL_Node*
vrd_avl_insert(vrd_AVL_Tree* const restrict tree,
               uint32_t const value,
               uint32_t const extra);


// FIXME: DEBUG
size_t
vrd_avl_print(FILE* const restrict stream,
              vrd_AVL_Tree const* const restrict tree);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
