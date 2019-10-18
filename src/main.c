#include <errno.h>      // errno
#include <stddef.h>     // NULL
#include <stdio.h>      // FILE, stderr, stdout, fclose, fopen, fprintf,
                        // perror
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // VRD_*, vrd_*

#include "../include/snv_tree.h"

int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_SNV_Tree* restrict tree = vrd_snv_tree_init(1000);
    if (NULL == tree)
    {
        (void) fprintf(stderr, "vrd_snv_tree_init() failed\n");
        goto error;
    } // if

    for (int i = 0; i < 22; ++i)
    {
        int position = rand() % 100;

        (void) fprintf(stderr, "insert: %d\n", position);

        if (NULL == vrd_snv_tree_insert(tree, position, 0, 0, 0))
        {
            (void) fprintf(stderr, "vrd_snv_tree_insert() failed\n");
            goto error;
        } // if
    } // for

    vrd_snv_tree_remove(tree, 49);

    vrd_snv_tree_destroy(&tree);

    return EXIT_SUCCESS;

error:
    vrd_snv_tree_destroy(&tree);

    return EXIT_FAILURE;
} // main
