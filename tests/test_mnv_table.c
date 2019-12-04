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

    vrd_MNV_Table* mnv = vrd_MNV_table_init(1000, 1 << 24);
    assert(NULL != mnv);

    int ret = vrd_MNV_table_insert(mnv, 5, "chr1", 10, 20, 1, 0, 0, 0);
    assert(0 == ret);

    ret = vrd_MNV_table_insert(mnv, 5, "chr1", 10, 20, 1, 0, 0, 0);
    assert(0 == ret);

    vrd_MNV_table_destroy(&mnv);
    assert(NULL == mnv);

    return EXIT_SUCCESS;
} // main
