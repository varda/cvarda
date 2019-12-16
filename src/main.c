#include <errno.h>      // errno
#include <stddef.h>     // NULL
#include <stdio.h>      // FILE, FILENAME_MAX, stderr, stdout, fclose,
                        // fopen, fprintf, perror, snprintf
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // VRD_*, vrd_*


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_AVL_Tree* subset = NULL;
    vrd_Cov_Table* cov = NULL;
    vrd_Seq_Table* seq = NULL;
    vrd_SNV_Table* snv = NULL;
    vrd_MNV_Table* mnv = NULL;
    FILE* istream = NULL;
    FILE* ostream = NULL;

    cov = vrd_Cov_table_init(100, 1 << 20);
    if (NULL == cov)
    {
        (void) fprintf(stderr, "vrd_Cov_table_init() failed\n");
        goto error;
    } // if

    seq = vrd_Seq_table_init(100000);
    if (NULL == seq)
    {
        (void) fprintf(stderr, "vrd_Seq_table_init() failed\n");
        goto error;
    } // if

    snv = vrd_SNV_table_init(100, 1 << 20);
    if (NULL == cov)
    {
        (void) fprintf(stderr, "vrd_SNV_table_init() failed\n");
        goto error;
    } // if

    mnv = vrd_MNV_table_init(100, 1 << 20);
    if (NULL == cov)
    {
        (void) fprintf(stderr, "vrd_MNV_table_init() failed\n");
        goto error;
    } // if

    istream = fopen("../data/CGND-HDA-02336_coverage.varda", "r");
    if (NULL == istream)
    {
        perror("fopen()");
        goto error;
    } // if

    if (929381 != vrd_coverage_from_file(istream, cov, 1))
    {
        (void) fprintf(stderr, "vrd_coverage_from_file() failed\n");
        goto error;
    } // if

    if (0 != fclose(istream))
    {
        perror("fclose()");
        goto error;
    } // if

    istream = fopen("../data/CGND-HDA-02336_variants.varda", "r");
    if (NULL == istream)
    {
        perror("fopen()");
        goto error;
    } // if

    if (5091015 != vrd_variants_from_file(istream, snv, mnv, seq, 1))
    {
        (void) fprintf(stderr, "vrd_variants_from_file() failed\n");
        goto error;
    } // if

    if (0 != fclose(istream))
    {
        perror("fclose()");
        goto error;
    } // if

    ostream = fopen("snv_table_export.varda", "w");
    if (NULL == ostream)
    {
        perror("fopen()");
        goto error;
    } // if

    if (4094652 != vrd_SNV_table_export(snv, ostream))
    {
        (void) fprintf(stderr, "vrd_SNV_table_export failed\n");
        goto error;
    } // if

    if (0 != fclose(ostream))
    {
        perror("fclose()");
        goto error;
    } // if

    ostream = fopen("mnv_table_export.varda", "w");
    if (NULL == ostream)
    {
        perror("fopen()");
        goto error;
    } // if

    if (996363 != vrd_MNV_table_export(mnv, ostream, seq))
    {
        (void) fprintf(stderr, "vrd_MNV_table_export failed\n");
        goto error;
    } // if

    if (0 != fclose(ostream))
    {
        perror("fclose()");
        goto error;
    } // if

    subset = vrd_AVL_tree_init(1);
    if (NULL == subset)
    {
        (void) fprintf(stderr, "vrd_AVL_tree_init() failed\n");
        goto error;
    } // if

    if (0 != vrd_AVL_tree_insert(subset, 1))
    {
        (void) fprintf(stderr, "vrd_AVL_tree_insert() failed\n");
        goto error;
    } // if

    istream = fopen("snv_table_export.varda", "r");
    if (NULL == istream)
    {
        perror("fopen()");
        goto error;
    } // if

    ostream = fopen("snv_table_anno.varda", "w");
    if (NULL == ostream)
    {
        perror("fopen()");
        goto error;
    } // if

    if (4094652 != vrd_annotate_from_file(ostream, istream, cov, snv, mnv, seq, subset))
    {
        (void) fprintf(stderr, "vrd_annotate_from_file() failed\n");
        goto error;
    } // if

    if (0 != fclose(istream))
    {
        perror("fclose()");
        goto error;
    } // if

    if (0 != fclose(ostream))
    {
        perror("fclose()");
        goto error;
    } // if

    istream = fopen("mnv_table_export.varda", "r");
    if (NULL == istream)
    {
        perror("fopen()");
        goto error;
    } // if

    ostream = fopen("mnv_table_anno.varda", "w");
    if (NULL == ostream)
    {
        perror("fopen()");
        goto error;
    } // if

    if (996363 != vrd_annotate_from_file(ostream, istream, cov, snv, mnv, seq, subset))
    {
        (void) fprintf(stderr, "vrd_annotate_from_file() failed\n");
        goto error;
    } // if

    if (0 != fclose(istream))
    {
        perror("fclose()");
        goto error;
    } // if

    if (0 != fclose(ostream))
    {
        perror("fclose()");
        goto error;
    } // if

    vrd_AVL_tree_destroy(&subset);
    vrd_Cov_table_destroy(&cov);
    vrd_Seq_table_destroy(&seq);
    vrd_SNV_table_destroy(&snv);
    vrd_MNV_table_destroy(&mnv);

    return EXIT_SUCCESS;

error:
    {
        if (0 != fclose(istream))
        {
            perror("fclose()");
        } // if

        if (0 != fclose(ostream))
        {
            perror("fclose()");
        } // if

        vrd_AVL_tree_destroy(&subset);
        vrd_Cov_table_destroy(&cov);
        vrd_Seq_table_destroy(&seq);
        vrd_SNV_table_destroy(&snv);
        vrd_MNV_table_destroy(&mnv);

        return EXIT_FAILURE;
    }
} // main
