#ifndef VRD_EXT_COVERAGE_H
#define VRD_EXT_COVERAGE_H


#define PY_SSIZE_T_CLEAN
#include <Python.h>     // PyObject

#include "../include/cov_table.h"   // vrd_Cov_Table
#include "../include/template.h"    // VRD_TEMPLATE
#include "template.h"   // VRD_PY_TEMPLATE


#define VRD_TYPENAME Cov
#define VRD_OBJNAME CoverageTable
#include "template_table.h"
#undef VRD_OBJNAME
#undef VRD_TYPENAME


#endif
