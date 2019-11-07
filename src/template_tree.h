#include "../include/template.h"    // VRD_TEMPLATE
#ifndef VRD_TYPENAME
#error "Undefined template typename"
#endif


#include <stddef.h>     // size_t
#include <stdio.h>      // FILE


VRD_TEMPLATE(VRD_TYPENAME, _Tree)*
VRD_TEMPLATE(VRD_TYPENAME, _tree_init)(size_t const capacity);


void
VRD_TEMPLATE(VRD_TYPENAME, _tree_destroy)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* restrict* const self);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _tree_remove)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const restrict self,
                                         vrd_AVL_Tree const* const restrict subset);


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_reorder)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const self);


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_read)(VRD_TEMPLATE(VRD_TYPENAME, _Tree)* const restrict self,
                                       FILE* restrict stream);


int
VRD_TEMPLATE(VRD_TYPENAME, _tree_write)(VRD_TEMPLATE(VRD_TYPENAME, _Tree) const* const restrict self,
                                        FILE* restrict stream);
