#ifndef VRD_TREE_H
#define VRD_TREE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdint.h>     // uint32_t


static uint32_t const NULLPTR = 0;

static unsigned int const LEFT = 0;
static unsigned int const RIGHT = 1;


typedef struct vrd_Tree
{
    uint32_t entries;
    uint32_t entry_size;
    uint32_t height;
} vrd_Tree;


#ifdef __cplusplus
} // extern "C"
#endif

#endif
