#include <errno.h>      // errno
#include <stddef.h>     // NULL
#include <stdio.h>      // FILE, FILENAME_MAX, stderr, stdout, fclose,
                        // fopen, fprintf, perror, snprintf
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // VRD_*, vrd_*


enum
{
    REF_CAPACITY = 1000,
    SEQ_CAPACITY = 100000,
    TREE_CAPACITY = 1 << 24
}; // constants


static int
load_giab(vrd_Cov_Table* cov,
          vrd_SNV_Table* snv,
          vrd_MNV_Table* mnv,
          vrd_Seq_Table* seq)
{
    for (int i = 0; i < 7; ++i)
    {
        (void) fprintf(stderr, "Loading sample: HG00%d\n", i + 1);

        char filename[FILENAME_MAX] = {'\0'};
        size_t const buf_size = FILENAME_MAX;
        if (0 >= snprintf(filename, buf_size, "../data/giab/HG00%d.bed", i + 1))
        {
            perror("snprintf()");
            return -1;
        } // if

        FILE* stream = fopen(filename, "r");
        if (NULL == stream)
        {
            perror("fopen()");
            return -1;
        } // if

        size_t const cov_count = vrd_coverage_from_file(stream, cov, i);

        if(0 != fclose(stream))
        {
            perror("fclose()");
            return -1;
        } // if

        if (0 >= snprintf(filename, buf_size, "../data/giab/HG00%d.csv", i + 1))
        {
            perror("snprintf()");
            return -1;
        } // if

        stream = fopen(filename, "r");
        if (NULL == stream)
        {
            perror("fopen()");
            return -1;
        } // if

        size_t const var_count = vrd_variants_from_file(stream, snv, mnv, seq, i);

        if(0 != fclose(stream))
        {
            perror("fclose()");
            return -1;
        } // if

        (void) fprintf(stderr, "Covered regions: %zu\n", cov_count);
        (void) fprintf(stderr, "Variants       : %zu\n", var_count);
    } // for

    return 0;
} // load_giab


static int
remove_sample(vrd_Cov_Table* cov,
              vrd_SNV_Table* snv,
              vrd_MNV_Table* mnv,
              vrd_Seq_Table* seq,
              int const idx)
{
    vrd_AVL_Tree* subset = vrd_AVL_tree_init(1);
    if (NULL == subset)
    {
        (void) fprintf(stderr, "vrd_AVL_tree_init() failed\n");
        goto error;
    } // if

    if (0 != vrd_AVL_tree_insert(subset, idx))
    {
        (void) fprintf(stderr, "vrd_AVL_tree_insert() failed\n");
        goto error;
    } // if

    (void) vrd_Cov_table_remove(cov, subset);
    (void) vrd_SNV_table_remove(snv, subset);
    (void) vrd_MNV_table_remove_seq(mnv, subset, seq);

    vrd_AVL_tree_destroy(&subset);

    return 0;

error:
    {
        vrd_AVL_tree_destroy(&subset);
        return -1;
    }
} // remove_sample


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_Cov_Table* cov = NULL;
    vrd_SNV_Table* snv = NULL;
    vrd_MNV_Table* mnv = NULL;
    vrd_Seq_Table* seq = NULL;

    cov = vrd_Cov_table_init(REF_CAPACITY, TREE_CAPACITY);
    if (NULL == cov)
    {
        (void) fprintf(stderr, "vrd_Cov_table_init() failed\n");
        goto error;
    } // if

    snv = vrd_SNV_table_init(REF_CAPACITY, TREE_CAPACITY);
    if (NULL == snv)
    {
        (void) fprintf(stderr, "vrd_SNV_table_init() failed\n");
        goto error;
    } // if

    mnv = vrd_MNV_table_init(REF_CAPACITY, TREE_CAPACITY);
    if (NULL == mnv)
    {
        (void) fprintf(stderr, "vrd_MNV_table_init() failed\n");
        goto error;
    } // if

    seq = vrd_Seq_table_init(SEQ_CAPACITY);
    if (NULL == seq)
    {
        (void) fprintf(stderr, "vrd_Seq_table_init() failed\n");
        goto error;
    } // if


    if (0 != load_giab(cov, snv, mnv, seq))
    {
        (void) fprintf(stderr, "load_giab() failed\n");
        goto error;
    } // if
/*
    if (0 != remove_sample(cov, snv, mnv, seq, 0))
    {
        (void) fprintf(stderr, "remove_sample() failed\n");
        goto error;
    } // if
*/
    FILE* stream = fopen("../data/CGND-HDA-02308_single.varda.csv", "r");
    if (NULL == stream)
    {
        perror("fopen()");
        goto error;
    } // if

    size_t const ann_count = vrd_annotate_from_file(stdout, stream, cov, snv, mnv, seq, NULL);

    if (0 != fclose(stream))
    {
        perror("fclose()");
        goto error;
    } // if

    (void) fprintf(stderr, "Annotated: %zu\n", ann_count);

    vrd_Cov_table_destroy(&cov);
    vrd_SNV_table_destroy(&snv);
    vrd_MNV_table_destroy(&mnv);
    vrd_Seq_table_destroy(&seq);

    return EXIT_SUCCESS;

error:
    {
        vrd_Cov_table_destroy(&cov);
        vrd_SNV_table_destroy(&snv);
        vrd_MNV_table_destroy(&mnv);
        vrd_Seq_table_destroy(&seq);

        return EXIT_FAILURE;
    }
} // main
