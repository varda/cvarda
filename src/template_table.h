#include "../include/template.h"    // VRD_TEMPLATE
#ifndef VRD_TYPENAME
#error "Undefined template typename"
#endif


#include <stddef.h>     // size_t

#include "tree.h"   // vrd_Tree


typedef struct VRD_TEMPLATE(VRD_TYPENAME, _Table) VRD_TEMPLATE(VRD_TYPENAME, _Table);


VRD_TEMPLATE(VRD_TYPENAME, _Table)*
VRD_TEMPLATE(VRD_TYPENAME, _table_init)(size_t const ref_capacity,
                                        size_t const tree_capacity);


void
VRD_TEMPLATE(VRD_TYPENAME, _table_destroy)(VRD_TEMPLATE(VRD_TYPENAME, _Table)** const self);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_remove)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                          vrd_AVL_Tree const* const subset);


int
VRD_TEMPLATE(VRD_TYPENAME, _table_reorder)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self);


int
VRD_TEMPLATE(VRD_TYPENAME, _table_read)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                        char const* const path);


int
VRD_TEMPLATE(VRD_TYPENAME, _table_write)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                         char const* const path);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_diagnostics)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                               char*** const reference,
                                               size_t** const used,
                                               size_t** const height);
