#include <stddef.h>     // size_t
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*, malloc, free, rand

#include "../include/varda.h"   // vrd_*, VRD_*


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


    vrd_SNV_Table* restrict snv = vrd_snv_table_init();
    if (NULL == snv)
    {
        (void) fprintf(stderr, "vrd_snv_table_init() failed\n");
        return EXIT_FAILURE;
    } // if

    for (size_t i = 0; i < 1; ++i)
    {
        if (-1 == vrd_snv_table_insert(snv, 4, "chr1", 0, 0, 0, 0))
        {
            (void) fprintf(stderr, "vrd_snv_table_insert() failed\n");
        } // if
    } // for

    vrd_snv_table_destroy(&snv);

    return EXIT_SUCCESS;
} // main
