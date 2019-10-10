/**
 * @file ascii_trie.h
 *
 * Defines a trie on the printable ASCII domain. See also:
 * https://en.wikipedia.org/wiki/Trie and
 * https://en.wikipedia.org/wiki/ASCII#Printable_characters
 *
 * The trie implements a dictionary (duplicate keys are not allowed) with
 * printable ASCII keys and a pointer to an arbitrary data value. The
 * supported operations are:
 *   - create an empty trie (vrd_ascii_trie_init())
 *   - destroy a trie (vrd_ascii_trie_destroy())
 *   - insert a key with a data value (vrd_ascii_trie_insert())
 *   - find a data value for a key (vrd_ascii_trie_find())
 * @warning The number of nodes in the trie may be limited by the
 *          implementation.
 */


#ifndef ASCII_TRIE_H
#define ASCII_TRIE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t


/**
 * Opaque data type for an ASCII trie.
 *
 * Provides an opaque reference to an ASCII trie. See also:
 * https://en.wikipedia.org/wiki/Opaque_data_type
 */
typedef struct vrd_ASCII_Trie vrd_ASCII_Trie;


/**
 * Create an empty ASCII trie.
 *
 * @param capacity limits the number of nodes in the trie. The actual
 *                 number of nodes in the tree may be further limited by
 *                 the implementation.
 * @return An opaque pointer to the trie on success, otherwise `NULL`.
 */
vrd_ASCII_Trie*
vrd_ascii_trie_init(size_t const capacity);


/**
 * Destroy an ASCII trie.
 *
 * All internal data is deallocated and the reference is set to `NULL`.
 * The data values are not deallocated.
 *
 * @param trie is a reference to a trie. The reference might be `NULL`.
 *             Calling this function mulitple times is safe.
 */
void
vrd_ascii_trie_destroy(vrd_ASCII_Trie* restrict* const trie);


/**
 * Insert a ASCII key with a data value in a trie.
 *
 * New nodes representing the key might be created in the trie. If the
 * key is already present in the trie, the data value is updated. This
 * doesn't deallocate the existing data value.
 *
 * @param trie is a valid reference to a trie. The reference to the
 *             trie must be valid, otherwise this function results in
 *             undefined behavior.
 * @param len is the length of the key. `strlen()` can be used to
 *            calculate the length of a `\0`-terminated string.
 * @param str is the key given as an ASCII string with length `len`.
 * @param data is a pointer to an arbitrary data value.
 * @return A pointer to the data value in the trie on success, otherwise
 *         `NULL`.
 */
void*
vrd_ascii_trie_insert(vrd_ASCII_Trie* const restrict trie,
                      size_t const len,
                      char const str[len],
                      void* const restrict data);


/**
 * Find a data value for a key in an ASCII trie.
 *
 * @param trie is a valid reference to a trie. The reference to the
 *             trie must be valid, otherwise this function results in
 *             undefined behavior.
 * @param len is the length of the key. `strlen()` can be used to
 *            calculate the length of a `\0`-terminated string.
 * @param str is the key given as an ASCII string with length `len`.
 * @return A pointer to the data value in the trie on success, otherwise
 *         `NULL`.
 */
void*
vrd_ascii_trie_find(vrd_ASCII_Trie const* const trie,
                    size_t const len,
                    char const str[len]);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
