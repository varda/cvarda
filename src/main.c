#include <errno.h>      // errno
#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, stderr, stdout, fclose, fopen, fprintf,
                        // perror
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // VRD_*, vrd_*
#include "../src/itv_tree.h"


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_Itv_Tree* restrict tree = NULL;
    vrd_AVL_Tree* restrict subset = NULL;

    tree = vrd_itv_tree_init(1000);
    if (NULL == tree)
    {
        (void) fprintf(stderr, "vrd_itv_tree_init() failed\n");
        goto error;
    } // if

    subset = vrd_avl_tree_init(100);
    if (NULL == subset)
    {
        (void) fprintf(stderr, "vrd_avl_tree_init() failed\n");
        goto error;
    } // if

    for (int i = 0; i < 22; ++i)
    {
        int start = rand() % 50;
        int end = start + rand() % 50;

        (void) fprintf(stderr, "insert: %d--%d\n", start, end);

        if (NULL == vrd_itv_tree_insert(tree, start, end, i))
        {
            (void) fprintf(stderr, "vrd_itv_tree_insert() failed\n");
            goto error;
        } // if
    } // for

    vrd_avl_tree_insert(subset, 4);

    size_t const count = vrd_itv_tree_remove(tree, subset);

    (void) fprintf(stderr, "Deleted: %zu\n", count);

    vrd_avl_tree_destroy(&subset);
    vrd_itv_tree_destroy(&tree);

    return EXIT_SUCCESS;

error:
    vrd_avl_tree_destroy(&subset);
    vrd_itv_tree_destroy(&tree);

    return EXIT_FAILURE;
} // main
