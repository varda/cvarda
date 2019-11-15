#ifndef VRD_EXT_MNV_H
#define VRD_EXT_MNV_H


#define PY_SSIZE_T_CLEAN
#include <Python.h>     // PyObject

#include "../include/mnv_table.h"   // vrd_MNV_Table
#include "../include/template.h"    // VRD_TEMPLATE
#include "template.h"   // VRD_PY_TEMPLATE


#define VRD_TYPENAME MNV
#define VRD_OBJNAME MNVTable
#include "template_table.h"     // MNVTableObject
#undef VRD_OBJNAME
#undef VRD_TYPENAME


#endif
