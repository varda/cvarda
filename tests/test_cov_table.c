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

    FILE* stream = fopen("../python_ext/tests/test_diag_coverage.varda", "r");
    assert(NULL != stream);

    vrd_coverage_from_file(stream, cov, 1);

    fclose(stream);

    char** reference = NULL;
    size_t* used = NULL;
    size_t* height = NULL;

    size_t const count = vrd_Cov_table_diagnostics(cov, &reference, &used, &height);
    assert((size_t) -1 != count);

    for (size_t i = 0; i < count; ++i)
    {
        (void) fprintf(stderr, "ref: %s\n", reference[i]);
        (void) fprintf(stderr, "used: %zu\n", used[i]);
        (void) fprintf(stderr, "height: %zu\n", height[i]);
        free(reference[i]);
    } // for
    free(reference);
    free(used);
    free(height);

/*
    int ret = vrd_Cov_table_insert(cov, 5, "chr1", 10, 20, 0);
    assert(0 == ret);

    ret = vrd_Cov_table_insert(cov, 5, "chr2", 10, 20, 0);
    assert(0 == ret);

    ret = vrd_Cov_table_insert(cov, 5, "chr2", 10, 20, 0);
    assert(0 == ret);

*/
    vrd_Cov_table_destroy(&cov);
    assert(NULL == cov);

    return EXIT_SUCCESS;
} // main
