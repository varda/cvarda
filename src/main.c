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

    vrd_Cov_Table* cov = NULL;
    vrd_Seq_Table* seq = NULL;
    vrd_SNV_Table* snv = NULL;
    vrd_MNV_Table* mnv = NULL;
    FILE* stream = NULL;

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

    stream = fopen("../data/CGND-HDA-02336_coverage.varda", "r");
    if (NULL == stream)
    {
        perror("fopen()");
        goto error;
    } // if

    if (929381 != vrd_coverage_from_file(stream, cov, 1))
    {
        (void) fprintf(stderr, "vrd_coverage_from_file() failed\n");
        goto error;
    } // if

    if (0 != fclose(stream))
    {
        perror("fclose()");
        goto error;
    } // if

    stream = fopen("../data/CGND-HDA-02336_variants.varda", "r");
    if (NULL == stream)
    {
        perror("fopen()");
        goto error;
    } // if

    if (5091015 != vrd_variants_from_file(stream, snv, mnv, seq, 1))
    {
        (void) fprintf(stderr, "vrd_variants_from_file() failed\n");
        goto error;
    } // if

    if (0 != fclose(stream))
    {
        perror("fclose()");
        goto error;
    } // if

    stream = fopen("snv_table_export.varda", "w");
    if (NULL == stream)
    {
        perror("fopen()");
        goto error;
    } // if

    vrd_SNV_table_export(snv, stream);

    if (0 != fclose(stream))
    {
        perror("fclose()");
        goto error;
    } // if

    stream = fopen("mnv_table_export.varda", "w");
    if (NULL == stream)
    {
        perror("fopen()");
        goto error;
    } // if

    vrd_MNV_table_export(mnv, stream, seq);

    if (0 != fclose(stream))
    {
        perror("fclose()");
        goto error;
    } // if

    vrd_Cov_table_destroy(&cov);
    vrd_Seq_table_destroy(&seq);
    vrd_SNV_table_destroy(&snv);
    vrd_MNV_table_destroy(&mnv);

    return EXIT_SUCCESS;

error:
    {
        if (0 != fclose(stream))
        {
            perror("fclose()");
        } // if

        vrd_Cov_table_destroy(&cov);
        vrd_Seq_table_destroy(&seq);
        vrd_SNV_table_destroy(&snv);
        vrd_MNV_table_destroy(&mnv);

        return EXIT_FAILURE;
    }
} // main
