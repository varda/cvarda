#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, fclose, fopen, fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // vrd_*


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_SNV_Table* snv = vrd_SNV_table_init(10, 1000);
    assert(NULL != snv);

    vrd_MNV_Table* mnv = vrd_MNV_table_init(10, 1000);
    assert(NULL != mnv);

    vrd_Seq_Table* seq = vrd_Seq_table_init(1000);
    assert(NULL != seq);

    FILE* const stream = fopen("../python_ext/tests/test_variants_small.varda", "r");
    assert(NULL != stream);

    size_t const ret = vrd_variants_from_file(stream, snv, mnv, seq, 1);
    assert(3 == ret);

    fclose(stream);

    vrd_Seq_table_destroy(&seq);
    vrd_MNV_table_destroy(&mnv);
    vrd_SNV_table_destroy(&snv);

    return EXIT_SUCCESS;
} // main
