#ifndef AVL_TREE_H
#define AVL_TREE_H


#ifdef __cplusplus

#define restrict

extern "C"
{
#endif


#include <stdint.h>     // uint32_t


/**
 * Opaque data structure for AVL nodes.
 */
typedef struct vrd_AVL_Node vrd_AVL_Node;

/**
 * Opaque data structure for an AVL tree.
 */
typedef struct vrd_AVL_Tree vrd_AVL_Tree;


/**
 * Create and initialize an AVL tree.
 *
 * @param capacity limits the number of nodes in the tree.
 * @return A pointer to the tree on success, otherwise NULL.
 */
vrd_AVL_Tree*
vrd_avl_tree_init(uint32_t const capacity);


/**
 * Destroy an AVL tree.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param tree is the reference to the tree.
 */
void
vrd_avl_tree_destroy(vrd_AVL_Tree** const tree);


/**
 * Insert an item to the tree.
 *
 * A new node with a given value is created and inserted in the tree.
 *
 * @param tree is the tree.
 * @param value is the value to insert in the tree.
 * @return A pointer to the item in the tree on success, otherwise NULL.
 */
vrd_AVL_Node*
vrd_avl_tree_insert(vrd_AVL_Tree* const tree, uint32_t const value);


#ifndef NDEBUG

#include <stdio.h>  // FILE


/**
 * Print the tree (for debugging).
 *
 * @param stream write to this output file stream.
 * @param tree is the tree.
 * @return The number of characters written on succes, otherwise -1.
 */
int
vrd_avl_tree_print(FILE* restrict stream,
                   vrd_AVL_Tree const* const restrict tree);


#endif


#ifdef __cplusplus
} // extern "C"
#endif


#endif
