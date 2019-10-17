#include <errno.h>      // errno
#include <stddef.h>     // NULL
#include <stdio.h>      // FILE, stderr, stdout, fclose, fopen, fprintf,
                        // perror
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

    vrd_Cov_Table* restrict cov = NULL;
    vrd_SNV_Table* restrict snv = NULL;
    vrd_MNV_Table* restrict mnv = NULL;
    vrd_Seq_Table* restrict seq = NULL;
    FILE* restrict stream = NULL;

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

    seq = vrd_seq_table_init(SEQ_CAPACITY, SEQ_SIZE_CAPACITY);
    if (NULL == seq)
    {
        (void) fprintf(stderr, "vrd_seq_table_init() failed\n");
        goto error;
    } // if

    errno = 0;
    stream = fopen("../data/depth10.bed", "r");
    if (NULL == stream)
    {
        perror("fopen()");
        goto error;
    } // if

    size_t const cov_count = vrd_coverage_from_file(stream, cov, 0);

    errno = 0;
    if (0 != fclose(stream))
    {
        perror("fclose()");
        goto error;
    } // if

    (void) fprintf(stderr, "Covered regions: %zu\n", cov_count);

    errno = 0;
    stream = fopen("../data/CGND-HDA-02308_single.varda.csv", "r");
    if (NULL == stream)
    {
        perror("fopen()");
        goto error;
    } // if

    size_t const var_count = vrd_variants_from_file(stream, snv, mnv, seq, 0);

    errno = 0;
    if (0 != fclose(stream))
    {
        perror("fclose()");
        goto error;
    } // if

    (void) fprintf(stderr, "Variants: %zu\n", var_count);

    errno = 0;
    stream = fopen("../data/CGND-HDA-02308_single.varda.csv", "r");
    if (NULL == stream)
    {
        perror("fopen()");
        goto error;
    } // if

    size_t const ann_count = vrd_annotate_from_file(stdout, stream, cov, snv, mnv, seq, NULL);

    errno = 0;
    if (0 != fclose(stream))
    {
        perror("fclose()");
        goto error;
    } // if

    (void) fprintf(stderr, "Annotated: %zu\n", ann_count);

    vrd_cov_table_destroy(&cov);
    vrd_snv_table_destroy(&snv);
    vrd_mnv_table_destroy(&mnv);
    vrd_seq_table_destroy(&seq);

    return EXIT_SUCCESS;

error:

    vrd_cov_table_destroy(&cov);
    vrd_snv_table_destroy(&snv);
    vrd_mnv_table_destroy(&mnv);
    vrd_seq_table_destroy(&seq);

    return EXIT_FAILURE;
} // main
