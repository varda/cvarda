#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, fopen, fclose, fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // vrd_*
#include "../src/mnv_tree.h"    // vrd_MNV_unpack


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_Seq_Table* seq = vrd_Seq_table_init(1000);
    assert(NULL != seq);

    vrd_MNV_Table* mnv = vrd_MNV_table_init(1000, 1 << 24);
    assert(NULL != mnv);

    vrd_Trie_Node* const elem = vrd_Seq_table_insert(seq, 5, "AAAA");
    assert(NULL != elem);

    int ret = vrd_MNV_table_insert(mnv, 5, "chr1", 10, 20, 1, 1, 10, *(size_t*) elem);
    assert(0 == ret);

    ret = vrd_MNV_table_insert(mnv, 5, "chr1", 5, 6, 1, 1, 10, *(size_t*) elem);
    assert(0 == ret);

    void* result[10] = {0};

    size_t const region_count = vrd_MNV_table_query_region(mnv, 5, "chr1", 0, 40, NULL, 10, result);
    for (size_t i = 0; i < region_count; ++i)
    {
        size_t start = 0;
        size_t end = 0;
        size_t allele_count = 0;
        size_t sample_id = 0;
        size_t phase = 0;
        size_t inserted = 0;
        vrd_MNV_unpack(result[i], &start, &end, &allele_count, &sample_id, &phase, &inserted);

        (void) fprintf(stderr, "%zu: %zu--%zu\n", i, start, end);
    } // for



/*
    FILE* stream = fopen("mnv_export.varda", "w");
    assert(NULL != stream);

    vrd_MNV_table_export(mnv, stream, seq);

    fclose(stream);
*/
    vrd_MNV_table_destroy(&mnv);
    assert(NULL == mnv);

    vrd_Seq_table_destroy(&seq);
    assert(NULL == seq);

    return EXIT_SUCCESS;
} // main
