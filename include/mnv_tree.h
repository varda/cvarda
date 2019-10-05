#ifndef MNV_TREE_H
#define MNV_TREE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stdint.h>     // uint32_t


/**
 * Opaque data structure for MNV nodes.
 */
typedef struct vrd_MNV_Node vrd_MNV_Node;

/**
 * Opaque data structure for an multi nucleotide variant (MNV) tree.
 */
typedef struct vrd_MNV_Tree vrd_MNV_Tree;


/**
 * Create and initialize a MNV tree.
 *
 * @param capacity limits the number of nodes in the tree.
 * @return A pointer to the tree on success, otherwise NULL.
 */
vrd_MNV_Tree*
vrd_mnv_tree_init(uint32_t const capacity);


/**
 * Destroy a MNV tree.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param tree is the reference to the tree.
 */
void
vrd_mnv_tree_destroy(vrd_MNV_Tree* restrict* const tree);


/**
 * Insert an item to the tree.
 *
 * A new node with a given value is created and inserted in the tree.
 *
 * @param tree is the tree.
 * @param start is the start position of the deleted part of the MNV
 *              (included).
 * @param end is the end position of the deleted part of the MNV
 *            (excluded).
 * @param sample_id is the ID of the sample that contains the MNV.
 * @param phase is the phase group (position based) to which the MNV
 *              belongs.
 * @param inserted is the inserted sequence.
 * @return A pointer to the item in the tree on success, otherwise NULL.
 */
vrd_MNV_Node*
vrd_mnv_tree_insert(vrd_MNV_Tree* const tree,
                    uint32_t const start,
                    uint32_t const end,
                    uint32_t const sample_id,
                    uint32_t const phase,
                    void* const inserted);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
