#include <stddef.h>     // NULL
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_cov_table_*
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
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

    vrd_Seq_Table* seq = vrd_seq_table_init(1000, 100000);
    if (NULL == seq)
    {
        (void) fprintf(stderr, "vrd_seq_table_init() failed\n");
        return EXIT_FAILURE;
    } // if

    fprintf(stderr, "insert: %p\n", vrd_seq_table_insert(seq, 4, "ACGT"));
    fprintf(stderr, "insert: %p\n", vrd_seq_table_insert(seq, 4, "ACGT"));

    vrd_seq_table_destroy(&seq);

    return EXIT_SUCCESS;
} // main
