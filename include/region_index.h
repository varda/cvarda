#ifndef REGION_INDEX_H
#define REGION_INDEX_H

#ifdef __cplusplus

#define restrict

extern "C"
{

#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // uint32_t

#include "avl_tree.h"   // vrd_AVL_Tree
#include "itv_tree.h"   // vrd_Itv_Node


typedef struct Region_Index vrd_Region_Index;

typedef struct vrd_Region_Node
{
    vrd_Itv_Node base;
    uint32_t phase;
} vrd_Region_Node;


static size_t const VRD_REGION_NODE_SIZE = sizeof(vrd_Region_Node);


vrd_Region_Index*
vrd_region_index_init(size_t const capacity, size_t const obj_size);


void
vrd_region_index_destroy(vrd_Region_Index* restrict*
                            const restrict index);


vrd_Region_Node*
vrd_region_index_insert(vrd_Region_Index* const restrict index,
                        uint32_t const start,
                        uint32_t const end,
                        uint32_t const sample_id,
                        uint32_t const phase);


size_t
vrd_region_index_query(vrd_Region_Index const* const restrict index,
                       uint32_t const start,
                       uint32_t const end,
                       vrd_AVL_Tree const* const restrict subset);


#ifdef __cplusplus

} // extern "C"

#endif

#endif
