#include <ctype.h>      // isprint
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*, malloc, free, rand

#include "../include/varda.h"   // vrd_*, VRD_*


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
    (void) fprintf(stderr, "VRD_AVL_NODE_SIZE: %zu\n", VRD_AVL_NODE_SIZE);
    (void) fprintf(stderr, "VRD_ITV_NODE_SIZE: %zu\n", VRD_ITV_NODE_SIZE);
    (void) fprintf(stderr, "VRD_SNV_NODE_SIZE: %zu\n", VRD_SNV_NODE_SIZE);
    (void) fprintf(stderr, "VRD_REGION_NODE_SIZE: %zu\n", VRD_REGION_NODE_SIZE);
    (void) fprintf(stderr, "VRD_MNV_NODE_SIZE: %zu\n", VRD_MNV_NODE_SIZE);


    vrd_Alloc* restrict tree_pool = vrd_pool_init(1000, VRD_REGION_NODE_SIZE);
    if (NULL == tree_pool)
    {
        return EXIT_FAILURE;
    } /// if

    vrd_Region_Index* restrict index = vrd_region_init(tree_pool);
    if (NULL == index)
    {
        return EXIT_FAILURE;
    } // if


    for (int i = 0; i < 10; ++i)
    {
        int const start = rand() % 100;
        int const end = start + rand() % 100;
        (void) fprintf(stderr, "Insert: %d--%d\n", start, end);
        int const res = vrd_region_insert(index, start, end, 0, start);
        if (-1 == res)
        {
            (void) fprintf(stderr, "vrd_region_insert() failed\n");
            break;
        } // if
        (void) vrd_region_print(stderr, index);
    } // for


    vrd_region_destroy(&index);
    vrd_pool_destroy(&tree_pool);

    return EXIT_SUCCESS;
} // main
