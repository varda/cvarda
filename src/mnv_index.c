#include <stdint.h>     // uint32_t
#include <stdlib.h>     // NULL, malloc, free

#include "../include/alloc.h"       // vrd_Alloc
#include "../include/region_index.h"    // vrd_Region_Node,
                                        // vrd_Region_Tree,
                                        // vrd_region_index_*
#include "../include/mnv_index.h"   // vrd_MNV_Index, vrd_mnv_index_*


struct MNV_Node
{
    vrd_Region_Node base;
    void* restrict inserted;
}; // MNV_Node


size_t const VRD_MNV_NODE_SIZE = sizeof(struct MNV_Node);


struct MNV_Index
{
    vrd_Region_Index* restrict index;
}; // MNV_Index


vrd_MNV_Index*
vrd_mnv_index_init(size_t const capacity)
{
    struct MNV_Index* const restrict index = malloc(sizeof(*index));
    if (NULL == index)
    {
        return NULL;
    } // if

    index->index = vrd_region_index_init(capacity, VRD_MNV_NODE_SIZE);
    if (NULL == index->index)
    {
        free(index);
        return NULL;
    } // if

    return index;
} // vrd_mnv_index_init


void
vrd_mnv_index_destroy(vrd_MNV_Index* restrict* const restrict index)
{
    if (NULL == index || NULL == *index)
    {
        return;
    } // if

    vrd_region_index_destroy(&(*index)->index);
    free(*index);
    *index = NULL;
} // vrd_mnv_index_destroy


int
vrd_mnv_index_insert(vrd_MNV_Index* const restrict index,
                     uint32_t const start,
                     uint32_t const end,
                     uint32_t const sample_id,
                     uint32_t const phase,
                     void* const restrict inserted)
{
    if (NULL == index)
    {
        return -1;
    } // if

    struct MNV_Node* const restrict node =
        (struct MNV_Node*) vrd_region_index_insert(index->index,
                                                   start,
                                                   end,
                                                   sample_id,
                                                   phase);
    if (NULL == node)
    {
        return -1;
    } // if

    node->inserted = inserted;

    return 0;
} // vrd_mnv_index_insert
