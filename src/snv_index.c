#include <stdint.h>     // uint32_t

#include "../include/alloc.h"       // vrd_Alloc, vrd_pool_*
#include "../include/avl_tree.h"    // vrd_AVL_Node, vrd_AVL_Tree
#include "../include/snv_index.h"   // vrd_SNV_Table


struct SNV_Node
{
    vrd_AVL_Node base;
    uint32_t phase;
    uint32_t type;
}; // SNV_Node


struct SNV_Index
{
    vrd_Alloc* restrict alloc;
    vrd_AVL_Tree* restrict tree;
}; // SNV_Index

