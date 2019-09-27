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

    vrd_Alloc* restrict pool = vrd_pool_init(100, sizeof(void*));
    if (NULL == pool)
    {
        (void) fprintf(stderr, "vrd_pool_init() failed\n");
        return EXIT_FAILURE;
    } // if

    vrd_Trie* restrict trie = vrd_trie_init(&vrd_malloc, VRD_ASCII_SIZE, vrd_ascii_to_idx);
    if (NULL == trie)
    {
        (void) fprintf(stderr, "vrd_trie_init() failed\n");
        vrd_pool_destroy(&pool);
        return EXIT_FAILURE;
    } // if

    vrd_SNV_Index* restrict index = vrd_snv_index_init(1000);
    if (NULL == index)
    {
        (void) fprintf(stderr, "vrd_snv_index_init() failed");
        vrd_trie_destroy(&trie);
        vrd_pool_destroy(&pool);
        return EXIT_FAILURE;
    } // if

    void* const restrict ptr = vrd_alloc(pool, sizeof(vrd_SNV_Index*));

    fprintf(stderr, "ptr = %p\n", ptr);
    fprintf(stderr, "deref = %p\n", vrd_deref(pool, ptr));
    //fprintf(stderr, "value = %zu\n", *(size_t*) vrd_deref(pool, ptr));

    *(vrd_SNV_Index**) vrd_deref(pool, ptr) = index;

    fprintf(stderr, "ptr = %p\n", ptr);
    fprintf(stderr, "deref = %p\n", vrd_deref(pool, ptr));
    fprintf(stderr, "value = %zu\n", *(size_t*) vrd_deref(pool, ptr));
    fprintf(stderr, "index = %p\n", (void*) index);

    for (size_t i = 1; i < vrd_pool_size(pool); ++i)
    {
        vrd_snv_index_destroy((vrd_SNV_Index**) vrd_deref(pool, (void*) i));
    } // for

    vrd_trie_destroy(&trie);
    vrd_pool_destroy(&pool);

    return EXIT_SUCCESS;
} // main
