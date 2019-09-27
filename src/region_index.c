#include <stdint.h>     // uint32_t
#include <stdlib.h>     // NULL, malloc, free

#include "../include/alloc.h"       // vrd_Alloc, vrd_pool_*
#include "../include/itv_tree.h"    // vrd_Itv_Node, vrd_Itv_Tree,
                                    // vrd_itv_*
#include "../include/region_index.h"   // vrd_Region_Index,
                                       // vrd_Region_Node,
                                       // vrd_region_index_*


struct Region_Index
{
    vrd_Alloc* restrict alloc;
    vrd_Itv_Tree* restrict tree;
}; // Region_Index


vrd_Region_Index*
vrd_region_index_init(size_t const capacity, size_t const obj_size)
{
    struct Region_Index* const restrict index = malloc(sizeof(*index));
    if (NULL == index)
    {
        return NULL;
    } // if

    index->alloc = vrd_pool_init(capacity, obj_size);
    if (NULL == index->alloc)
    {
        free(index);
        return NULL;
    } // if

    index->tree = vrd_itv_init(index->alloc);
    if (NULL == index->tree)
    {
        vrd_pool_destroy(&index->alloc);
        free(index);
        return NULL;
    } // if

    return index;
} // vrd_region_index_init


void
vrd_region_index_destroy(vrd_Region_Index* restrict* const restrict index)
{
    if (NULL == index || NULL == *index)
    {
        return;
    } // if

    vrd_itv_destroy(&(*index)->tree);
    vrd_pool_destroy(&(*index)->alloc);
    free(*index);
    *index = NULL;
} // vrd_region_index_destroy


vrd_Region_Node*
vrd_region_index_insert(vrd_Region_Index* const restrict index,
                        uint32_t const start,
                        uint32_t const end,
                        uint32_t const sample_id,
                        uint32_t const phase)
{
    if (NULL == index)
    {
        return NULL;
    } // if

    vrd_Region_Node* const restrict node =
        (vrd_Region_Node*) vrd_itv_insert(index->tree, start, end);
    if (NULL == node)
    {
        return NULL;
    } // if

    node->base.extra = sample_id;
    node->phase = phase;

    return node;
} // vrd_region_index_insert
