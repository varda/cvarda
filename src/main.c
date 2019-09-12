#include <ctype.h>      // isprint
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // vrd_*


enum
{
    ASCII_SIZE = 95
}; // constants


static inline size_t
ascii_to_idx(char const ch)
{
    if (isprint(ch))
    {
        return ch - ' ';
    } // if
    return ASCII_SIZE;
} // ascii_to_idx


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    int major = 0;
    int minor = 0;
    int patch = 0;
    vrd_version(&major, &minor, &patch);

    (void) fprintf(stderr, "cvarda version %d.%d.%d\n", major,
                                                        minor,
                                                        patch);

    vrd_Pool* restrict pool = vrd_pool_init(1000, sizeof(char*));
    if (NULL == pool)
    {
        (void) fprintf(stderr, "vrd_pool_init() failed\n");
        return EXIT_FAILURE;
    } // if

    vrd_Trie* restrict trie = vrd_trie_init(ASCII_SIZE, ascii_to_idx);
    if (NULL == trie)
    {
        (void) fprintf(stderr, "vrd_trie_init() failed\n");
        vrd_pool_destroy(&pool, free);
        return EXIT_FAILURE;
    } // if

    size_t const el = vrd_pool_alloc(pool);
    if ((size_t) -1 == el)
    {
        (void) fprintf(stderr, "vrd_pool_alloc() failed\n");
        vrd_trie_destroy(&trie);
        vrd_pool_destroy(&pool, free);
        return EXIT_FAILURE;
    } // if

    char* a = vrd_pool_deref(pool, el);
    a = malloc(4);
    a[0] = 'c';
    a[1] = 'h';
    a[2] = 'r';
    a[3] = '1';

    fprintf(stderr, "a = %p\n", (void*) a);

    if ((size_t) -1 == vrd_trie_insert(trie, 4, "chr1", el))
    {
        (void) fprintf(stderr, "vrd_trie_insert() failed\n");
        vrd_trie_destroy(&trie);
        vrd_pool_destroy(&pool, free);
        return EXIT_FAILURE;
    } // if

    vrd_trie_destroy(&trie);
    vrd_pool_destroy(&pool, free);

    return EXIT_SUCCESS;
} // main
