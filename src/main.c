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

    vrd_MNV_Table* restrict indel = vrd_mnv_table_init();

    vrd_mnv_table_insert(indel, 4, "chr1", 0, 10, 0, 0, NULL);

    vrd_mnv_table_destroy(&indel);

    return EXIT_SUCCESS;
} // main
