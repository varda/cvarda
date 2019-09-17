#include <ctype.h>      // isprint
#include <stdint.h>     // SIZE_MAX
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*, malloc, free, rand

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

    vrd_Alloc* restrict tree_pool = vrd_pool_init(1000, 16);
    if (NULL == tree_pool)
    {
        return EXIT_FAILURE;
    } /// if

    vrd_AVL_Tree* restrict tree = vrd_avl_init(tree_pool);
    if (NULL == tree)
    {
        vrd_pool_destroy(&tree_pool);
        return EXIT_FAILURE;
    } // if


    vrd_avl_destroy(&tree);
    vrd_pool_destroy(&tree_pool);

    return EXIT_SUCCESS;
} // main
