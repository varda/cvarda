#include "../include/template.h"
#ifndef VRD_TYPENAME
#error "Undefined template typename"
#endif


typedef struct VRD_TEMPLATE(VRD_TYPENAME, _Table) VRD_TEMPLATE(VRD_TYPENAME, _Table);


VRD_TEMPLATE(VRD_TYPENAME, _Table)*
VRD_TEMPLATE(VRD_TYPENAME, _table_init)(size_t const ref_capacity,
                                        size_t const tree_capacity);


void
VRD_TEMPLATE(VRD_TYPENAME, _table_destroy)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* restrict* const self);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_remove)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const restrict self,
                                          vrd_AVL_Tree const* const restrict subset);


int
VRD_TEMPLATE(VRD_TYPENAME, _table_reorder)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self);


int
VRD_TEMPLATE(VRD_TYPENAME, _table_read)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const restrict self,
                                        char const* const restrict path);


int
VRD_TEMPLATE(VRD_TYPENAME, _table_write)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const restrict self,
                                         char const* const restrict path);
