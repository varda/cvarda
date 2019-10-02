#include <stddef.h>     // NULL
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*, rand

#include "../include/avl_tree.h"    // vrd_AVL_*, vrd_avl_tree_*
#include "../include/itv_tree.h"    // vrd_Itv_*, vrd_itv_tree_*
#include "../include/mnv_tree.h"    // vrd_MNV_*, vrd_mnv_tree_*
#include "../include/snv_tree.h"    // vrd_SNV_*, vrd_snv_tree_*
#include "../include/ascii_trie.h"    // vrd_ASCII_Trie, vrd_ascii_trie_*


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_ASCII_Trie* trie = vrd_ascii_trie_init(20);
    if (NULL == trie)
    {
        (void) fprintf(stderr, "vrd_ascii_trie_init() failed\n");
        return EXIT_FAILURE;
    } // if

    if (NULL == vrd_ascii_trie_insert(trie, 4, "chr1", (void*) 1))
    {
        (void) fprintf(stderr, "vrd_ascii_trie_insert() failed\n");
        vrd_ascii_trie_destroy(&trie);
        return EXIT_FAILURE;
    } // if

    fprintf(stderr, "find: %p\n", vrd_ascii_trie_find(trie, 4, "chr1"));

    fprintf(stderr, "find: %p\n", vrd_ascii_trie_find(trie, 4, "chr2"));


    vrd_ascii_trie_destroy(&trie);

    return EXIT_SUCCESS;
} // main
