#ifndef VRD_EXT_SEQUENCE_H
#define VRD_EXT_SEQUENCE_H


#define PY_SSIZE_T_CLEAN
#include <Python.h>     // PyObject

#include "../include/seq_table.h"   // vrd_Seq_Table
#include "../include/template.h"    // VRD_TEMPLATE
#include "template.h"   // VRD_PY_TEMPLATE


#define VRD_TYPENAME Seq
#define VRD_OBJNAME SequenceTable
#include "template_table.h"
#undef VRD_OBJNAME
#undef VRD_TYPENAME


#endif
