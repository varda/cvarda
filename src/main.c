#include <stddef.h>     // NULL
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_cov_table_*
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*


enum
{
    SEQ_CAPACITY = 100000,
    SEQ_SIZE_CAPACITY = 1000000,
    REF_CAPACITY = 1000,
    REF_SIZE_CAPACITY = 10000,
    TREE_CAPACITY = 1 << 28
}; // constants


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_Seq_Table* seq = NULL;
    vrd_Cov_Table* cov = NULL;
    vrd_SNV_Table* snv = NULL;
    vrd_MNV_Table* mnv = NULL;

    seq = vrd_seq_table_init(SEQ_CAPACITY, SEQ_SIZE_CAPACITY);
    if (NULL == seq)
    {
        (void) fprintf(stderr, "vrd_seq_table_init() failed\n");
        goto error;
    } // if

    cov = vrd_cov_table_init(REF_CAPACITY, REF_SIZE_CAPACITY, TREE_CAPACITY);
    if (NULL == cov)
    {
        (void) fprintf(stderr, "vrd_cov_table_init() failed\n");
        goto error;
    } // if

    snv = vrd_snv_table_init(REF_CAPACITY, REF_SIZE_CAPACITY, TREE_CAPACITY);
    if (NULL == snv)
    {
        (void) fprintf(stderr, "vrd_snv_table_init() failed\n");
        goto error;
    } // if

    mnv = vrd_mnv_table_init(REF_CAPACITY, REF_SIZE_CAPACITY, TREE_CAPACITY);
    if (NULL == mnv)
    {
        (void) fprintf(stderr, "vrd_mnv_table_init() failed\n");
        goto error;
    } // if

    vrd_seq_table_destroy(&seq);
    vrd_cov_table_destroy(&cov);
    vrd_snv_table_destroy(&snv);
    vrd_mnv_table_destroy(&mnv);

    return EXIT_SUCCESS;

error:
    vrd_seq_table_destroy(&seq);
    vrd_cov_table_destroy(&cov);
    vrd_snv_table_destroy(&snv);
    vrd_mnv_table_destroy(&mnv);

    return EXIT_FAILURE;
} // main
