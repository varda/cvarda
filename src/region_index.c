#include <stdint.h>     // uint32_t
#include <stdlib.h>     // NULL, malloc, free

#include "../include/alloc.h"       // vrd_Alloc
#include "../include/itv_tree.h"    // vrd_Itv_Node, vrd_Itv_Tree,
                                    // vrd_itv_*
#include "../include/region_index.h"   // vrd_Region_Index,
                                       // vrd_Region_Node, vrd_region_*


struct Region_Index
{
    vrd_Itv_Tree* restrict tree;
}; // Region_Index


vrd_Region_Index*
vrd_region_init(vrd_Alloc* const restrict alloc)
{
    if (&vrd_malloc == alloc)
    {
        return NULL;
    } // if

    struct Region_Index* const restrict index = malloc(sizeof(*index));
    if (NULL == index)
    {
        return NULL;
    } // if

    index->tree = vrd_itv_init(alloc);
    if (NULL == index->tree)
    {
        free(index);
        return NULL;
    } // if

    return index;
} // vrd_region_init


void
vrd_region_destroy(vrd_Region_Index* restrict* const restrict index)
{
    if (NULL == index || NULL == *index)
    {
        return;
    } // if

    vrd_itv_destroy(&(*index)->tree);
    free(*index);
    *index = NULL;
} // vrd_region_destroy


int
vrd_region_insert(vrd_Region_Index* const restrict index,
                  uint32_t const start,
                  uint32_t const end,
                  uint32_t const sample_id,
                  uint32_t const phase)
{
    if (NULL == index)
    {
        return -1;
    } // if

    vrd_Region_Node* const restrict node =
        (vrd_Region_Node*) vrd_itv_insert(index->tree, start, end);
    if (NULL == node)
    {
        return -1;
    } // if

    // FIXME: range check on phase and type

    node->base.extra = sample_id;
    node->phase = phase;

    return 0;
} // vrd_region_insert
