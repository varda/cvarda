#ifndef VRD_AVL_TREE_H
#define VRD_AVL_TREE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdbool.h>    // bool

#include "template.h"   // VRD_TEMPLATE


#define VRD_TYPENAME AVL


typedef struct VRD_TEMPLATE(VRD_TYPENAME, _Tree) VRD_TEMPLATE(VRD_TYPENAME, _Tree);


#include "../src/template_tree.h"   // vrd_AVL_tree_*


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self,
                                         size_t const key);


bool
VRD_TEMPLATE(VRD_TYPENAME, _tree_is_element)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const self,
                                             size_t const key);


#undef VRD_TYPENAME


#ifdef __cplusplus
} // extern "C"
#endif

#endif
