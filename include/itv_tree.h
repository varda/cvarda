#ifndef ITV_TREE_H
#define ITV_TREE_H


#ifdef __cplusplus

#define restrict

extern "C"
{
#endif


#include <stdint.h>     // uint32_t


/**
 * Opaque data structure for Itv nodes.
 */
typedef struct vrd_Itv_Node vrd_Itv_Node;

/**
 * Opaque data structure for an Itv tree.
 */
typedef struct vrd_Itv_Tree vrd_Itv_Tree;


/**
 * Create and initialize an interval tree.
 *
 * @param capacity limits the number of nodes in the tree.
 * @return A pointer to the tree on success, otherwise NULL.
 */
vrd_Itv_Tree*
vrd_itv_tree_init(uint32_t const capacity);


/**
 * Destroy an interval tree.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param tree is the reference to the tree.
 */
void
vrd_itv_tree_destroy(vrd_Itv_Tree** const tree);


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
                    uint32_t const start,
                    uint32_t const end,
                    uint32_t const sample_id);


#ifdef __cplusplus
} // extern "C"
#endif


#endif
