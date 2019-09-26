#include <stdint.h>     // uint32_t
#include <stdio.h>      // FILE*
#include <stdlib.h>     // NULL, malloc, free

#include "../include/alloc.h"       // vrd_Alloc, vrd_pool_*
#include "../include/avl_tree.h"    // vrd_AVL_Node, vrd_AVL_Tree,
                                    // vrd_avl_*
#include "../include/snv_index.h"   // vrd_SNV_Index, vrd_snv_index_*


struct SNV_Node
{
    vrd_AVL_Node base;
    uint32_t phase : 28;
    uint32_t type  :  4;
}; // SNV_Node


struct SNV_Index
{
    vrd_Alloc* restrict alloc;
    vrd_AVL_Tree* restrict tree;
}; // SNV_Index


size_t const VRD_SNV_NODE_SIZE = sizeof(struct SNV_Node);
size_t const VRD_SNV_INDEX_SIZE = sizeof(struct SNV_Index);


vrd_SNV_Index*
vrd_snv_index_init(size_t const capacity)
{
    struct SNV_Index* const restrict index = malloc(sizeof(*index));
    if (NULL == index)
    {
        return NULL;
    } // if

    index->alloc = vrd_pool_init(capacity, VRD_SNV_NODE_SIZE);
    if (NULL == index->alloc)
    {
        free(index);
        return NULL;
    } // if

    index->tree = vrd_avl_init(index->alloc);
    if (NULL == index->tree)
    {
        vrd_pool_destroy(&index->alloc);
        free(index);
        return NULL;
    } // if

    return index;
} // vrd_snv_index_init


void
vrd_snv_index_destroy(vrd_SNV_Index* restrict* const restrict index)
{
    if (NULL == index || NULL == *index)
    {
        return;
    } // if

    vrd_avl_destroy(&(*index)->tree);
    vrd_pool_destroy(&(*index)->alloc);
    free(*index);
    *index = NULL;
} // vrd_snv_index_destroy


int
vrd_snv_index_insert(vrd_SNV_Index* const restrict index,
                     uint32_t const position,
                     uint32_t const sample_id,
                     uint32_t const phase,
                     uint32_t const type)
{
    if (NULL == index)
    {
        return -1;
    } // if

    struct SNV_Node* const restrict node =
        (struct SNV_Node*) vrd_avl_insert(index->tree, position);
    if (NULL == node)
    {
        return -1;
    } // if

    node->base.extra = sample_id;
    node->phase = phase;
    node->type = type;

    return 0;
} // vrd_snv_index_insert


size_t
vrd_snv_index_print(FILE* const restrict stream,
                    vrd_SNV_Index const* const restrict index)
{
    if (NULL == index)
    {
        return 0;
    } // if
    return vrd_avl_print(stream, index->tree);
} // vrd_snv_index_print
