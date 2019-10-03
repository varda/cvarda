#include <stddef.h>     // size_t
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*, malloc, free

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

    vrd_Region_Table* restrict cov = vrd_region_table_init();

    if (-1 == vrd_region_table_insert(cov, 4, "chr1", 1, 10, 0, 0))
    {
        fprintf(stderr, "vrd_region_table_insert failed\n");
    } // if

    size_t const n = vrd_region_table_query(cov, 4, "chr1", 0, 100, NULL);
    fprintf(stderr, "result: %zu\n", n);

    vrd_region_table_destroy(&cov);

    return EXIT_SUCCESS;
} // main
