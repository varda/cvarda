#include <assert.h>     // assert
#include <errno.h>      // errno
#include <stddef.h>     // NULL
#include <stdio.h>      // FILE, fprintf, stderr, stdout, fscanf
#include <stdlib.h>     // EXIT_*
#include <string.h>     // strlen

#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_cov_table_*
#include "../include/iupac.h"       // vrd_iupac_to_idx
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*


enum
{
    SEQ_CAPACITY = 100000,
    SEQ_SIZE_CAPACITY = 1000000,
    REF_CAPACITY = 1000,
    REF_SIZE_CAPACITY = 10000,
    TREE_CAPACITY = 1 << 24
}; // constants


size_t
variants_from_file(FILE* restrict stream,
                   vrd_SNV_Table* const restrict snv,
                   vrd_MNV_Table* const restrict mnv,
                   vrd_Seq_Table* const restrict seq,
                   size_t const sample_id)
{
    assert(NULL != stream);
    assert(NULL != snv);
    assert(NULL != mnv);
    assert(NULL != seq);

    static char reference[128] = {'\0'};
    int start = 0;
    int end = 0;
    int phase = 0;
    int len = 0;
    static char inserted[1024] = {'\0'};

    size_t count = 0;
    while (6 == fscanf(stream, "%127s %d %d %d %d %1023s", reference,
                                                           &start,
                                                           &end,
                                                           &phase,
                                                           &len,
                                                           inserted))
    {
        if (1 == len && inserted[0] != '.' && 1 == end - start)
        {
            if (-1 == vrd_snv_table_insert(snv,
                                           strlen(reference),
                                           reference,
                                           start,
                                           sample_id,
                                           phase,
                                           vrd_iupac_to_idx(inserted[0])))
            {
                (void) fprintf(stderr, "vrd_snv_table_insert() failed\n");
                break;
            } // if
        } // if
        else
        {
            char const* const restrict ins_ptr =
                vrd_seq_table_insert(seq, len, inserted);
            if (0 != len && NULL == ins_ptr)
            {
                (void) fprintf(stderr, "vrd_seq_table_insert() failed\n");
                break;
            } // if

            if (-1 == vrd_mnv_table_insert(mnv,
                                           strlen(reference),
                                           reference,
                                           start,
                                           end,
                                           sample_id,
                                           phase,
                                           ins_ptr))
            {
                (void) fprintf(stderr, "vrd_mnv_table_insert() failed\n");
                break;
            } // if

        } // else
        count += 1;
    } // while

    return count;
} // variants_from_file


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

    size_t const count = variants_from_file(stdin, snv, mnv, seq, 0);

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
