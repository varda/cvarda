/**
 * @file iupac_trie.h
 *
 * Defines a trie on the IUPAC nucleotide domain. See also:
 * https://en.wikipedia.org/wiki/Trie and
 * https://en.wikipedia.org/wiki/Nucleic_acid_notation
 *
 * The trie implements a dictionary (duplicate keys are not allowed) with
 * IUPAC nucleotide keys and a pointer to a IUPAC nucleotide sequence.
 * The supported operations are:
 *   - create an empty trie (vrd_iupac_trie_init())
 *   - destroy a trie (vrd_iupac_trie_destroy())
 *   - insert a key with a sequence (vrd_iupac_trie_insert())
 *   - find a sequence for a key (vrd_iupac_trie_find())
 * @warning The number of nodes in the trie may be limited by the
 *          implementation.
 *
 * The IUPAC trie is used to store (and retrieve) nucleotide sequences
 * that are inserted as part of a MNV.
 */


#ifndef IUPAC_TRIE_H
#define IUPAC_TRIE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t


/**
 * Opaque data type for a IUPAC trie.
 *
 * Provides an opaque reference to a IUPAC trie. See also:
 * https://en.wikipedia.org/wiki/Opaque_data_type
 */
typedef struct vrd_IUPAC_Trie vrd_IUPAC_Trie;


/**
 * Create an empty IUPAC trie.
 *. 
 * @param capacity limits the number of nodes in the trie. The actual
 *                 number of nodes in the tree may be further limited by
 *                 the implementation.
 * @return An opaque pointer to the trie on success, otherwise `NULL`.
 */
vrd_IUPAC_Trie*
vrd_iupac_trie_init(size_t const capacity);


/**
 * Destroy a IUPAC trie.
 *
 * All internal data is deallocated and the reference is set to `NULL`.
 * The sequences are not deallocated.
 *
 * @param trie is a reference to a trie. The reference may be `NULL`.
 *             Calling this function multiple times is safe.
 */
void
vrd_iupac_trie_destroy(vrd_IUPAC_Trie* restrict* const trie);


/**
 * Insert a IUPAC key with a sequence in a trie.
 *
 * New nodes representing the key may be created in the trie. If the
 * key is already present in the trie, the existing sequence is returned.
 *
 * @param trie is a valid reference to a trie. The reference to the
 *             trie must be valid, otherwise this function results in
 *             undefined behavior.
 * @param len is the length of the key (`str`). `strlen()` may be used to
 *            calculate the length of a `\0`-terminated string.
 * @param str is the key given as a IUPAC string with length `len`.
 * @param data is a pointer to an arbitrary data value.  Storing `NULL`
 *             data values is problematic, beacuse there is no way to
 *             distinguish between successful or failed insertion.
 * @return A pointer to the sequence in the trie on success, otherwise
 *         `NULL`.
 */
char const*
vrd_iupac_trie_insert(vrd_IUPAC_Trie* const restrict trie,
                      size_t const len,
                      char const str[len],
                      char const* const restrict data);


/**
 * Find a sequence for a key in a IUPAC trie.
 *
 * @param trie is a valid reference to a trie. The reference to the
 *             trie must be valid, otherwise this function results in
 *             undefined behavior.
 * @param len is the length of the key (`str`). `strlen()` may be used to
 *            calculate the length of a `\0`-terminated string.
 * @param str is the key given as an ASCII string with length `len`.
 * @return A pointer to the sequence in the trie if the key is found
 *         succesfully, otherwise `NULL`. Storing `NULL` data values is
 *         problematic, beacuse there is no way to distinguish between
 *         found and unfound keys.
 */
char const*
vrd_iupac_trie_find(vrd_IUPAC_Trie const* const trie,
                    size_t const len,
                    char const str[len]);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
