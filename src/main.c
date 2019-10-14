#include <errno.h>      // errno
#include <stddef.h>     // NULL
#include <stdio.h>      // FILE, fprintf, stderr, stdout, fscanf
#include <stdlib.h>     // EXIT_*
#include <string.h>     // strlen

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

    FILE* restrict fdb = NULL;
    FILE* restrict fsample = NULL;

    vrd_Cov_Table* cov = vrd_cov_table_init(REF_CAPACITY, REF_SIZE_CAPACITY, TREE_CAPACITY);
    if (NULL == cov)
    {
        (void) fprintf(stderr, "vrd_cov_table_init() failed\n");
        goto error;
    } // if

    errno = 0;
    fdb = fopen("../data/chr1_encode_faststab.bed", "r");
    if (NULL == fdb)
    {
        perror("fopen()");
        goto error;
    } // if

    char reference[128] = {0};
    int start = 0;
    int end = 0;
    while (3 == fscanf(fdb, "%127s %d %d", reference, &start, &end))
    {
        if (-1 == vrd_cov_table_insert(cov, strlen(reference), reference, start, end, 0))
        {
            (void) fprintf(stderr, "vrd_cov_table_insert() failed\n");
            goto error;
        } // if
    } // while

    fclose(fdb);
    fdb = NULL;

    errno = 0;
    fsample = fopen("../data/gnomad.genomes.r2.0.1.sites.chr1.bed", "r");
    if (NULL == fsample)
    {
        perror("fopen()");
        goto error;
    } // if

    while (3 == fscanf(fsample, "%127s %d %d", reference, &start, &end))
    {
        size_t const count = vrd_cov_table_query(cov, strlen(reference), reference, start, end, NULL);
        (void) fprintf(stdout, "%s\t%i\t%i\t%zu\t0\n", reference, start, end, count);
    } // while

    fclose(fsample);
    vrd_cov_table_destroy(&cov);

    return EXIT_SUCCESS;

error:
    fclose(fdb);
    fclose(fsample);
    vrd_cov_table_destroy(&cov);

    return EXIT_FAILURE;
} // main
