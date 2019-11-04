#ifndef VRD_EXT_SNV_H
#define VRD_EXT_SNV_H


#define PY_SSIZE_T_CLEAN
#include <Python.h>     // PyObject

#include "../include/snv_table.h"   // vrd_SNV_Table
#include "../include/template.h"    // VRD_TEMPLATE
#include "template.h"   // VRD_PY_TEMPLATE


#define VRD_TYPENAME SNV
#define VRD_OBJNAME SNVTable
#include "template_table.h"
#undef VRD_OBJNAME
#undef VRD_TYPENAME


#endif
