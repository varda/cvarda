/**
 * @file itv_tree.h
 *
 * Defines an interval (itv) tree. See also:
 * https://en.wikipedia.org/wiki/Interval_tree
 *
 * The tree implements a multiset (i.e., duplicate values are allowed) of
 * intervals, given by start and end posisions. The supported operations
 * are:
 *   - create an empty tree (vrd_itv_tree_init())
 *   - destroy a tree (vrd_itv_tree_destroy())
 *   - insert an interval (vrd_itv_tree_insert())
 *   - count the number of times a query interval is contained within the
 *     intervals in the tree (vrd_itv_tree_query())
 * @warning The size of the integers as well as the number of nodes in
 *          the tree may be limited by the implementation.
 *
 * The interval tree is used to store covered regions for a specific
 * reference sequence.
 */


#ifndef ITV_TREE_H
#define ITV_TREE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "../include/avl_tree.h"   // vrd_AVL_Tree


/**
 * Opaque data type for an interval tree.
 *
 * Provides an opaque reference to an interval tree. See also:
 * https://en.wikipedia.org/wiki/Opaque_data_type
 */
typedef struct vrd_Itv_Tree vrd_Itv_Tree;


/**
 * Create an empty interval tree.
 *
 * @param capacity limits the number of nodes in the tree. The actual
 *                 number of nodes in the tree may be further limited by
 *                 the implementation.
 * @return An opaque pointer to the tree on success, otherwise `NULL`.
 */
vrd_Itv_Tree*
vrd_itv_tree_init(size_t const capacity);


/**
 * Destroy an interval tree.
 *
 * All associated data is deallocated and the reference is set to `NULL`.
 *
 * @param tree is a reference to a tree. The reference may be `NULL`.
 *             Calling this function multiple times is safe.
 */
void
vrd_itv_tree_destroy(vrd_Itv_Tree* restrict* const tree);


/**
 * Insert an interval in a tree.
 *
 * A new node with for the given interval is created and inserted in the
 * tree.
 *
 * @param tree is a valid reference to a tree. The reference to the tree
 *             must be valid, otherwise this function results in
 *             undefined behavior.
 * @param start is the start position of the interval (included).
 *              This value is not bound checked. It is the resposibility
 *              of the caller to make sure that the table can actually
 *              store the value.
 * @param end is the end position of the interval (excluded).
 *            This value is not bound checked. It is the resposibility
 *            of the caller to make sure that the table can actually
 *            store the value. It is the resposibility of the caller
 *            to make sure that: `start` <= `end`.
 * @param sample_id is an sample identifier indicating to which sample
 *                  the interval belongs. This value is not bound
 *                  checked. It is the resposibility of the caller to
 *                  make sure that the table can actually store the
 *                  value.
 * @return An opaque pointer to the node in the tree on success,
 *         otherwise `NULL`.
 */
void*
vrd_itv_tree_insert(vrd_Itv_Tree* const tree,
                    size_t const start,
                    size_t const end,
                    size_t const sample_id);


/**
 * Count the number of times a query interval is contained within the
 * intervals in the tree.
 *
 * @param tree is a valid reference to a tree. The reference to the tree
 *             must be valid, otherwise this function results in
 *             undefined behavior.
 * @param start is the start position of the interval (included).
 *              This value is not bound checked. It is the resposibility
 *              of the caller to make sure that the table can actually
 *              store the value.
 * @param end is the end position of the interval (excluded).
 *            This value is not bound checked. It is the resposibility
 *            of the caller to make sure that the table can actually
 *            store the value. It is the resposibility of the caller
 *            to make sure that: `start` <= `end`.
 * @param subset is an AVL tree holding sample identifiers that define a
 *               subset of the samples in the tree. Only intervals that
 *               belong to a sample in this tree are considered. If the
 *               subset is `NULL`, all intervals in the tree are
 *               considered.
 * @return The number of interval (for the subset) that include
 *         the query interval.
 */
size_t
vrd_itv_tree_query(vrd_Itv_Tree const* const restrict tree,
                   size_t const start,
                   size_t const end,
                   vrd_AVL_Tree const* const restrict subset);


size_t
vrd_itv_tree_remove(vrd_Itv_Tree* const restrict tree,
                    vrd_AVL_Tree const* const restrict subset);


void
vrd_itv_tree_reorder(vrd_Itv_Tree* const tree);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
