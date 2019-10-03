#include <stddef.h>     // NULL
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*
#include <string.h>     // strlen


#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_cov_table_*
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*


static char const* const REFSEQ[24] =
{
    "chr1",
    "chr2",
    "chr3",
    "chr4",
    "chr5",
    "chr6",
    "chr7",
    "chr8",
    "chr9",
    "chr10",
    "chr11",
    "chr12",
    "chr13",
    "chr14",
    "chr15",
    "chr16",
    "chr17",
    "chr18",
    "chr19",
    "chr20",
    "chr21",
    "chrX",
    "chrY",
    "chrM"
}; // REFSEQ


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_Cov_Table* restrict cov = vrd_cov_table_init();
    if (NULL == cov)
    {
        (void) fprintf(stderr, "vrd_cov_table_init() failed\n");
        return EXIT_FAILURE;
    } // if

    if (-1 == vrd_cov_table_insert(cov, 4, REFSEQ[0], 42, 84, 0))
    {
        (void) fprintf(stderr, "vrd_cov_table_insert() failed\n");
        vrd_cov_table_destroy(&cov);
        return EXIT_FAILURE;
    } // if

    (void) fprintf(stderr, "query: %zu\n", vrd_cov_table_query(cov, 4, REFSEQ[0], 42, 84, NULL));
    (void) fprintf(stderr, "query: %zu\n", vrd_cov_table_query(cov, 4, REFSEQ[0], 0, 10, NULL));

    vrd_cov_table_destroy(&cov);

    return EXIT_SUCCESS;
} // main
