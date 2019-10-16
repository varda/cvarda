#include <stddef.h>     // NULL
#include <stdio.h>      // FILE, fprintf, stderr, stdout
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // VRD_*, vrd_*


enum
{
    SEQ_CAPACITY = 100000,
    SEQ_SIZE_CAPACITY = 1000000,
    REF_CAPACITY = 1000,
    REF_SIZE_CAPACITY = 10000,
    TREE_CAPACITY = 1 << 24
}; // constants


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_SNV_Table* restrict snv = NULL;
    vrd_MNV_Table* restrict mnv = NULL;
    vrd_Seq_Table* restrict seq = NULL;

    snv = vrd_snv_table_init(REF_CAPACITY, REF_SIZE_CAPACITY, TREE_CAPACITY);
    if (NULL == snv)
    {
        (void) fprintf(stderr, "vrd_snv_table_insert() failed\n");
        goto error;
    } // if

    mnv = vrd_mnv_table_init(REF_CAPACITY, REF_SIZE_CAPACITY, TREE_CAPACITY);
    if (NULL == mnv)
    {
        (void) fprintf(stderr, "vrd_mnv_table_insert() failed\n");
        goto error;
    } // if

    seq = vrd_seq_table_init(SEQ_CAPACITY, SEQ_SIZE_CAPACITY);
    if (NULL == seq)
    {
        (void) fprintf(stderr, "vrd_seq_table_insert() failed\n");
        goto error;
    } // if

    size_t const count = vrd_variants_from_file(stdin, snv, mnv, seq, 0);

    (void) fprintf(stderr, "variants added %zu\n", count);

    vrd_snv_table_destroy(&snv);
    vrd_mnv_table_destroy(&mnv);
    vrd_seq_table_destroy(&seq);

    return EXIT_SUCCESS;

error:

    vrd_snv_table_destroy(&snv);
    vrd_mnv_table_destroy(&mnv);
    vrd_seq_table_destroy(&seq);

    return EXIT_FAILURE;
} // main
