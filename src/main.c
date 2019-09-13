#include <ctype.h>      // isprint
#include <stdint.h>     // SIZE_MAX
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*, malloc, free

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

    vrd_Alloc* restrict pool = vrd_pool_init(100, 8 + ASCII_SIZE * 8);
    if (NULL == pool)
    {
        (void) fprintf(stderr, "vrd_pool_init() failed\n");
        return EXIT_FAILURE;
    } // if

    vrd_Trie* restrict trie = vrd_trie_init(pool, ASCII_SIZE, ascii_to_idx);
    if (NULL == trie)
    {
        (void) fprintf(stderr, "vrd_trie_init() failed\n");
        vrd_pool_destroy(&pool);
        return EXIT_FAILURE;
    } // if

    int a = 4;

    //vrd_trie_insert(trie, 4, "chr1", &a);

    vrd_trie_destroy(&trie);
    vrd_pool_destroy(&pool);

    return EXIT_SUCCESS;
} // main
