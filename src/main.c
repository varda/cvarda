#include <stddef.h>     // NULL
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*, malloc, free
#include <string.h>     // strlen

#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_cov_table_*
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*

#include "../include/iupac_trie.h"  // vrd_IUPAC_Trie, vrd_iupac_trie_*


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

    vrd_IUPAC_Trie* restrict trie = vrd_iupac_trie_init(10000000);
    if (NULL == trie)
    {
        (void) fprintf(stderr, "vrd_iupac_trie_init() failed\n");
        return EXIT_FAILURE;
    } // if

    char** const restrict str_table = malloc(1000000 * sizeof(*str_table));

    str_table[0] = malloc(4);
    str_table[0][0] = REFSEQ[0][0];
    str_table[0][1] = REFSEQ[0][1];
    str_table[0][2] = REFSEQ[0][2];
    str_table[0][3] = REFSEQ[0][3];

    if (NULL == vrd_iupac_trie_insert(trie, 4, REFSEQ[0], str_table[0]))
    {
        (void) fprintf(stderr, "vrd_iupac_trie_init() failed\n");
        free(str_table[0]);
        free(str_table);
        vrd_iupac_trie_destroy(&trie);
        return EXIT_FAILURE;
    } // if

    fprintf(stderr, "found: %p\n", vrd_iupac_trie_find(trie, 4, REFSEQ[0]));
    fprintf(stderr, "found: %.*s\n", 4, (char*) vrd_iupac_trie_find(trie, 4, REFSEQ[0]));

    free(str_table[0]);
    free(str_table);

    vrd_iupac_trie_destroy(&trie);

    return EXIT_SUCCESS;
} // main
