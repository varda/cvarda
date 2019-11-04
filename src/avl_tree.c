#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stdbool.h>    // bool, false, true
#include <stddef.h>     // NULL, size_t
#include <stdint.h>     // UINT32_MAX, int32_t, uint32_t

#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_AVL_tree_*
#include "../include/template.h"    // VRD_TEMPLATE


#define VRD_TYPENAME AVL


struct VRD_TEMPLATE(VRD_TYPENAME, _Node)
{
    uint32_t child[2];
    uint32_t key;
    int32_t  balance   :  3;  // [-4, ..., 3], we use [-2, ..., 2]
    uint32_t sample_id : 29;
}; // vrd_AVL_Node


#include "template_tree.inc"    // vrd_AVL_tree_*, insert


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                         size_t const key)
{
    assert(NULL != self);

    if (UINT32_MAX == self->next || self->capacity < self->next)
    {
        return -1;
    } // if

    uint32_t const ptr = self->next;
    self->next += 1;

    self->nodes[ptr].child[LEFT] = NULLPTR;
    self->nodes[ptr].child[RIGHT] = NULLPTR;
    self->nodes[ptr].key = key;
    self->nodes[ptr].balance = 0;

    insert(self, ptr);

    return 0;
} // vrd_AVL_tree_insert


bool
VRD_TEMPLATE(VRD_TYPENAME, _tree_is_element)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                             size_t const key)
{
    assert(NULL != self);

    uint32_t tmp = self->root;
    while (NULLPTR != tmp)
    {
        if (key == self->nodes[tmp].key)
        {
            return true;
        } // if
        tmp = self->nodes[tmp].child[key > self->nodes[tmp].key];
    } // while

    return false;
} // vrd_AVL_tree_is_element


#undef VRD_TYPENAME
