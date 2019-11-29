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

    vrd_Diagnostics* diag = NULL;

    size_t const count = vrd_Cov_table_diagnostics(cov, &diag);
    assert((size_t) -1 != count);

    for (size_t i = 0; i < count; ++i)
    {
        //(void) fprintf(stderr, "reference: %s\n", diag[i].reference);
        //(void) fprintf(stderr, "entries: %zu\n", diag[i].entries);
        //(void) fprintf(stderr, "height: %zu\n", diag[i].height);
        free(diag[i].reference);
    } // for
    free(diag);

    vrd_Cov_table_destroy(&cov);
    assert(NULL == cov);

    return EXIT_SUCCESS;
} // main
