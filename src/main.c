#include <stddef.h>     // NULL
#include <stdio.h>      // stderr, fprintf
                        // perror
#include <stdlib.h>     // EXIT_*, rand

#include "../include/varda.h"   // VRD_*, vrd_*


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_AVL_Tree* tree = vrd_avl_tree_init(100);
    if (NULL == tree)
    {
        (void) fprintf(stderr, "vrd_avl_tree_init() failed\n");
        goto error;
    } // if

    for (int i = 0; i < 22; ++i)
    {
        int const value = rand() % 100;
        (void) fprintf(stderr, "Inserting: %d\n", value);
        if (NULL == vrd_avl_tree_insert(tree, value))
        {
            (void) fprintf(stderr, "vrd_avl_tree_insert() failed\n");
            goto error;
        } // if
    } // for

    vrd_avl_tree_destroy(&tree);

    return EXIT_SUCCESS;

error:
    vrd_avl_tree_destroy(&tree);

    return EXIT_FAILURE;
} // main
