#ifndef ITV_TREE_H
#define ITV_TREE_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif

#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t, int32_t

#include <stdio.h>      // FIXME: DEBUG FILE*

#include "alloc.h"      // vrd_Alloc


typedef struct Itv_Tree vrd_Itv_Tree;

typedef struct vrd_Itv_Node
{
    uint32_t child[2];
    uint32_t start;
    uint32_t end;
    uint32_t max;
    int32_t balance :  3;  // [-2, .., 2]
    uint32_t extra  : 29;
} vrd_Itv_Node;


vrd_Itv_Tree*
vrd_itv_init(vrd_Alloc* const restrict alloc);


void
vrd_itv_destroy(vrd_Itv_Tree* restrict* const restrict tree);


vrd_Itv_Node*
vrd_itv_insert(vrd_Itv_Tree* const restrict tree,
               uint32_t const start,
               uint32_t const end);


// FIXME: DEBUG
size_t
vrd_itv_print(FILE* const restrict stream,
              vrd_Itv_Tree const* const restrict tree);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
