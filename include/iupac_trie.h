#ifndef IUPAC_TRIE_H
#define IUPAC_TRIE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t


enum
{
    VRD_IUPAC_SIZE = 16
}; // constants


static inline size_t
vrd_iupac_to_idx(char const ch)
{
    switch (ch)
    {
        case '.':
        case 'Z':
        case 'z':
            return 0;
        case 'A':
        case 'a':
            return 1;
        case 'C':
        case 'c':
            return 2;
        case 'M':
        case 'm':
            return 3;
        case 'G':
        case 'g':
            return 4;
        case 'R':
        case 'r':
            return 5;
        case 'S':
        case 's':
            return 6;
        case 'V':
        case 'v':
            return 7;
        case 'T':
        case 't':
        case 'U':
        case 'u':
            return 8;
        case 'W':
        case 'w':
            return 9;
        case 'Y':
        case 'y':
            return 10;
        case 'H':
        case 'h':
            return 11;
        case 'K':
        case 'k':
            return 12;
        case 'D':
        case 'd':
            return 13;
        case 'B':
        case 'b':
            return 14;
        case 'N':
        case 'n':
            return 15;
    } // switch
    return VRD_IUPAC_SIZE;
} // vrd_iupac_to_idx


/**
 * Opaque data structure for a trie in the printable IUPAC domain.
 */
typedef struct vrd_IUPAC_Trie vrd_IUPAC_Trie;


/**
 * Create and initialize a trie.
 *
 * @param capacity limits the number of nodes in the trie.
 * @return A pointer to the trie on success, otherwise NULL.
 */
vrd_IUPAC_Trie*
vrd_iupac_trie_init(size_t const capacity);


/**
 * Destroy a trie.
 *
 * All associated data is deallocated and the reference is set to NULL.
 *
 * @param trie is the reference to the trie.
 */
void
vrd_iupac_trie_destroy(vrd_IUPAC_Trie* restrict* const trie);


/**
 * Insert a string with an associated data pointer to the trie.
 *
 * @param trie the trie.
 * @param len the length of the string (excluding the '\0').
 * @param str the string.
 * @param data points to the data.
 * @return A pointer to the inserted data on success, otherwise NULL.
 */
void*
vrd_iupac_trie_insert(vrd_IUPAC_Trie* const restrict trie,
                      size_t const len,
                      char const str[len],
                      void* restrict data);


/**
 * Find a string with an associated data pointer.
 *
 * @param trie the trie.
 * @param len the length of the string (excluding the '\0').
 * @param str the string.
 * @return A pointer to the data on success, otherwise NULL.
 */
void*
vrd_iupac_trie_find(vrd_IUPAC_Trie const* const trie,
                    size_t const len,
                    char const str[len]);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
