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

    vrd_Cov_Table* cov = vrd_Cov_table_init(1000, 1 << 24);
    assert(NULL != cov);

    int ret = vrd_Cov_table_insert(cov, 5, "chr1", 10, 20, 0);
    assert(0 == ret);

    ret = vrd_Cov_table_insert(cov, 5, "chr1", 10, 20, 0);
    assert(0 == ret);

    vrd_Cov_table_destroy(&cov);
    assert(NULL == cov);

    return EXIT_SUCCESS;
} // main
