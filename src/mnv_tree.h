#ifndef MNV_TREE_H
#define MNV_TREE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdio.h>      // FILE

#include "../include/avl_tree.h"    // vrd_AVL_Tree
#include "../include/seq_table.h"   // vrd_Seq_Table


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
vrd_mnv_tree_init(size_t const capacity);


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
 * @param inserted is a reference to the inserted sequence.
 * @return A pointer to the item in the tree on success, otherwise NULL.
 */
vrd_MNV_Node*
vrd_mnv_tree_insert(vrd_MNV_Tree* const restrict tree,
                    size_t const start,
                    size_t const end,
                    size_t const sample_id,
                    size_t const phase,
                    size_t const inserted);


/**
 * Query MNVs in the tree.
 *
 * @param tree is the tree.
 * @param start is the start position of the deleted part of the MNV
 *              (included).
 * @param end is the end position of the deleted part of the MNV
 *            (excluded).
 * @param inserted is a reference to the inserted sequence.
 * @param subset is the subset of sample IDs.
 * @return The count of reported MNVs.
 */
size_t
vrd_mnv_tree_query(vrd_MNV_Tree const* const restrict tree,
                   size_t const start,
                   size_t const end,
                   size_t const inserted,
                   vrd_AVL_Tree const* const restrict subset);


size_t
vrd_mnv_tree_remove(vrd_MNV_Tree* const restrict tree,
                    vrd_AVL_Tree const* const restrict subset,
                    vrd_Seq_Table* const restrict seq_table);


int
vrd_mnv_tree_reorder(vrd_MNV_Tree* const tree);


int
vrd_mnv_tree_read(vrd_MNV_Tree* const restrict tree,
                  FILE* restrict stream);


int
vrd_mnv_tree_write(vrd_MNV_Tree const* const restrict tree,
                   FILE* restrict stream);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
