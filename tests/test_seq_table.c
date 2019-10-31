#include <assert.h>     // assert
#include <stddef.h>     // NULL
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // vrd_*


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_Seq_Table* seq = vrd_seq_table_init(1000);
    assert(NULL != seq);

    void* const a = vrd_seq_table_insert(seq, 4, "ACGT");
    assert(NULL != a);

    void* const b = vrd_seq_table_insert(seq, 4, "ACGT");
    assert(NULL != b);

    assert(a == b);

    vrd_seq_table_destroy(&seq);

    return EXIT_SUCCESS;
} // main
