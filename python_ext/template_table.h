#include "../include/template.h"    // VRD_TEMPLATE
#include "template.h"   // VRD_PY_TEMPLATE
#ifndef VRD_TYPENAME
#error "Undefined template typename"
#endif
#ifndef VRD_OBJNAME
#error "Undefined template Python object name"
#endif


typedef struct
{
    PyObject_HEAD
    VRD_TEMPLATE(VRD_TYPENAME, _Table)* table;
} VRD_PY_TEMPLATE(VRD_OBJNAME, Object);


extern PyTypeObject VRD_OBJNAME;
