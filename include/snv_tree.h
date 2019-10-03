#ifndef SNV_TREE_H
#define SNV_TREE_H

#ifdef __cplusplus

#define restrict
extern "C"
{
#endif


#include <stdint.h>     // uint32_t


/**
 * Opaque data structure for SNV nodes.
 */
typedef struct vrd_SNV_Node vrd_SNV_Node;

/**
 * Opaque data structure for an single nucleotide variant (SNV) tree.
 */
typedef struct vrd_SNV_Tree vrd_SNV_Tree;


/**
 * Create and initialize a SNV tree.
 *
 * @param capacity limits the number of nodes in the tree.
 * @return A pointer to the tree on success, otherwise NULL.
 */
vrd_SNV_Tree*
vrd_snv_tree_init(uint32_t const capacity);


/**
 * Destroy a SNV tree.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param tree is the reference to the tree.
 */
void
vrd_snv_tree_destroy(vrd_SNV_Tree* restrict* const tree);


/**
 * Insert an item to the tree.
 *
 * A new node with a given value is created and inserted in the tree.
 *
 * @param tree is the tree.
 * @param position is the positon (zero based) of the SNV.
 * @param sample_id is the ID of the sample that contains the SNV.
 * @param phase is the phase group (position based) to which the SNV
 *              belongs.
 * @param type is the type of the SNV, e.g., A, C, G, or T, currently,
 *             nucleotide IUPAC is supported.
 * @return A pointer to the item in the tree on success, otherwise NULL.
 */
vrd_SNV_Node*
vrd_snv_tree_insert(vrd_SNV_Tree* const tree,
                    uint32_t const position,
                    uint32_t const sample_id,
                    uint32_t const phase,
                    uint32_t const type);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
