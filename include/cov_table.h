/**
 * @file: cov_table.h
 *
 * Defines a table to store covered regions on a reference sequence,
 * referred to as coverage table. A covered region is by described by a
 * start and an end coordinate on a reference sequence identifier. The
 * table is indexed primarily on the reference sequence identifier, and
 * secondary on the intervals defined by the start and end coordinates. A
 * sample identifier is included as extra annotation.
 *
 * This uses a form of templating to define the common interfaces across
 * similar tables (see: ../src/template_table.h). This file only
 * specifies the template specialization for this table:
 *   - Insert a covered region into the table: vrd_Cov_table_insert()
 *   - Query (count, stab) the table for a given interval:
 *     vrd_Cov_table_query_stab()
 *
 * The interface uses a platform specific integer for most data points
 * (size_t), however the implementation may limit the range of these data
 * points for performance and storage reasons. No bounds checking is
 * performed as part of this interface. It is therefore the
 * responsibility of the caller to make sure that the data can be stored
 * in the table. The constants VRD_MAX_* from constants.h are useful.
 */


#ifndef VRD_COV_TABLE_H
#define VRD_COV_TABLE_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "avl_tree.h"   // vrd_AVL_Tree
#include "template.h"   // VRD_TEMPLATE


// from here VRD_TEMPLATE(...) expands to vrd_Cov
#define VRD_TYPENAME Cov


// defines the common interface for the coverage table
#include "../src/template_table.h"  // vrd_Cov_table_*


/**
 * Insert a covered region into the coverage table.
 *
 * @param self refers to a coverage table. Must be a valid reference
 *             otherwise this function results in undefined behavior.
 * @param len the length of the reference sequence identifier
 *            (`reference`)
 * @param reference
 * @param start
 * @param end
 * @param allele_count
 * @param sample_id
 */
int
VRD_TEMPLATE(VRD_TYPENAME, _table_insert)(VRD_TEMPLATE(VRD_TYPENAME, _Table)* const self,
                                          size_t const len,
                                          char const reference[len],
                                          size_t const start,
                                          size_t const end,
                                          size_t const allele_count,
                                          size_t const sample_id);


size_t
VRD_TEMPLATE(VRD_TYPENAME, _table_query_stab)(VRD_TEMPLATE(VRD_TYPENAME, _Table) const* const self,
                                              size_t const len,
                                              char const reference[len],
                                              size_t const start,
                                              size_t const end,
                                              vrd_AVL_Tree const* const subset);


#undef VRD_TYPENAME


#ifdef __cplusplus
} // extern "C"
#endif

#endif
