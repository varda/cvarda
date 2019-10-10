/**
 * @file avl_tree.h
 *
 * Defines an AVL tree, i.e., a self-balancing binary tree. See also:
 * https://en.wikipedia.org/wiki/AVL_tree
 *
 * The tree implements a multiset (i.e., duplicate values are allowed) of
 * integers. The supported operations are:
 *   - create an empty tree (vrd_avl_tree_init())
 *   - destroy a tree (vrd_avl_tree_destroy())
 *   - insert a value (vrd_avl_tree_insert())
 *   - check whether a value is in the tree (vrd_avl_tree_is_element())
 * @warning The size of the integers as well as the number of nodes in
 *          the tree may be limited by the implementation.
 *
 * The AVL tree is used to contain a subset of sample identifiers when
 * querying a coverage tables, SNV tables or MNV tables.
 */


#ifndef AVL_TREE_H
#define AVL_TREE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stdbool.h>    // bool


/**
 * Opaque data type for an AVL tree.
 *
 * Provides an opaque reference to an AVL tree. See also:
 * https://en.wikipedia.org/wiki/Opaque_data_type
 */
typedef struct vrd_AVL_Tree vrd_AVL_Tree;


/**
 * Create an empty AVL tree.
 *
 * @param capacity limits the number of nodes in the tree. The actual
 *                 number of nodes in the tree may be further limited by
 *                 the implementation.
 * @return An opaque pointer to the tree on success, otherwise `NULL`.
 */
vrd_AVL_Tree*
vrd_avl_tree_init(size_t const capacity);


/**
 * Destroy an AVL tree.
 *
 * All associated data is deallocated and the reference is set to `NULL`.
 *
 * @param tree is a reference to a tree. The reference may be `NULL`.
 *             Calling this function multiple times is safe.
 */
void
vrd_avl_tree_destroy(vrd_AVL_Tree* restrict* const tree);


/**
 * Insert a value in an AVL tree.
 *
 * A new node with a given value is created and inserted in the tree.
 *
 * @param tree is a valid reference to a tree. The reference to the tree
 *             must be valid, otherwise this function results in
 *             undefined behavior.
 * @param value is the value to insert in the tree. The value is not
 *              bound checked. It is the resposibility of the caller to
 *              make sure that the tree can actually store the value.
 * @return An opaque pointer to the node in the tree on success,
 *         otherwise `NULL`.
 */
void*
vrd_avl_tree_insert(vrd_AVL_Tree* const tree, size_t const value);


/**
 * Check whether a value is in an AVL tree.
 *
 * @param tree is a valid reference to a tree. The reference to the tree
 *             must be valid, otherwise this function results in
 *             undefined behavior.
 * @param value is the value to insert in the tree. The value is not
 *              bound checked. It is the resposibility of the caller to
 *              make sure that the tree can actually store the value.
 * @return `true` if the value is found in the tree, otherwise `false`.
 */
bool
vrd_avl_tree_is_element(vrd_AVL_Tree const* const tree,
                        size_t const value);


#ifndef NDEBUG

#include <stdio.h>  // FILE


/**
 * Print an AVL tree (for debugging).
 *
 * @param stream is a valid output file stream.
 * @param tree is a valid reference to a tree. The reference to the tree
 *             must be valid, otherwise this function results in
 *             undefined behavior.
 * @return The number of characters written on success, otherwise `-1`.
 * @warning This function may be unavailable when `NDEBUG` is defined.
 */
int
vrd_avl_tree_print(FILE* restrict stream,
                   vrd_AVL_Tree const* const restrict tree);


#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif
