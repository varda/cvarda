#ifndef ITV_TREE_H
#define ITV_TREE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t

#include "avl_tree.h"   // vrd_AVL_Tree


/**
 * Opaque data structure for Itv nodes.
 */
typedef struct vrd_Itv_Node vrd_Itv_Node;

/**
 * Opaque data structure for an interval (Itv) tree.
 */
typedef struct vrd_Itv_Tree vrd_Itv_Tree;


/**
 * Create and initialize an Itv tree.
 *
 * @param capacity limits the number of nodes in the tree.
 * @return A pointer to the tree on success, otherwise NULL.
 */
vrd_Itv_Tree*
vrd_itv_tree_init(size_t const capacity);


/**
 * Destroy an interval tree.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param tree is the reference to the tree.
 */
void
vrd_itv_tree_destroy(vrd_Itv_Tree* restrict* const tree);


/**
 * Insert an item to the tree.
 *
 * A new node with a given value is created and inserted in the tree.
 *
 * @param tree is the tree.
 * @param start is the start position of the interval (included).
 * @param end is the end position of the interval (excluded).
 * @param sample_id is the ID of the sample that contains the interval.
 * @return A pointer to the item in the tree on success, otherwise NULL.
 */
vrd_Itv_Node*
vrd_itv_tree_insert(vrd_Itv_Tree* const tree,
                    size_t const start,
                    size_t const end,
                    size_t const sample_id);


/**
 * Query intevals in the tree.
 *
 * @param tree is the tree.
 * @param start is the start position of the interval (included).
 * @param end is the end position of the interval (excluded).
 * @param subset is the subset of sample IDs.
 * @return The count of reported intervals.
 */
size_t
vrd_itv_tree_query(vrd_Itv_Tree const* const restrict tree,
                   size_t const start,
                   size_t const end,
                   vrd_AVL_Tree const* const restrict subset);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
