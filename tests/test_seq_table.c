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

    vrd_Seq_Table* seq = vrd_Seq_table_init(1000);
    assert(NULL != seq);

    void* const a = vrd_Seq_table_insert(seq, 4, "ACGT");
    assert(NULL != a);

    void* const b = vrd_Seq_table_insert(seq, 4, "ACGT");
    assert(NULL != b);

    assert(a == b);

    void* c = vrd_Seq_table_query(seq, 4, "ACGT");
    assert(c == a);

    size_t const idx = *(size_t*) c;

    int ret = vrd_Seq_table_remove(seq, idx);
    assert(0 == ret);

    c = vrd_Seq_table_query(seq, 4, "ACGT");
    assert(c == a);

    ret = vrd_Seq_table_remove(seq, idx);
    assert(0 == ret);

    ret = vrd_Seq_table_remove(seq, idx);
    assert(0 == ret);

    c = vrd_Seq_table_query(seq, 4, "ACGT");
    assert(NULL == c);

    vrd_Seq_table_destroy(&seq);
    assert(NULL == seq);

    return EXIT_SUCCESS;
} // main
