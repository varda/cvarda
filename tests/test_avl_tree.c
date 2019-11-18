#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // vrd_*


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_AVL_Tree* avl = vrd_AVL_tree_init(1000);
    assert(NULL != avl);

    int ret = vrd_AVL_tree_insert(avl, 1);
    assert(0 == ret);

    ret = vrd_AVL_tree_insert(avl, 1);
    assert(0 == ret);

    vrd_AVL_tree_destroy(&avl);
    assert(NULL == avl);

    return EXIT_SUCCESS;
} // main
