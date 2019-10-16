#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, fscanf
#include <string.h>     // strlen

#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_cov_table_*
#include "../include/iupac.h"       // vrd_iupac_to_idx
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*
#include "../include/utils.h"       // vrd_coverage_from_file,
                                    // vrd_variants_from_file


size_t
vrd_coverage_from_file(FILE* restrict stream,
                       vrd_Cov_Table* const restrict cov,
                       size_t const sample_id)
{
    assert(NULL != stream);
    assert(NULL != cov);

    char reference[128] = {'\0'};
    int start = 0;
    int end = 0;

    size_t count = 0;
    while (3 == fscanf(stream, "%127s %d %d", reference, &start, &end))
    {
        if (-1 == vrd_cov_table_insert(cov,
                                       strlen(reference),
                                       reference,
                                       start,
                                       end,
                                       sample_id))
        {
            break;
        } // if
        count += 1;
    } // while

    return count;
} // vrd_coverage_from_file


size_t
vrd_variants_from_file(FILE* restrict stream,
                       vrd_SNV_Table* const restrict snv,
                       vrd_MNV_Table* const restrict mnv,
                       vrd_Seq_Table* const restrict seq,
                       size_t const sample_id)
{
    assert(NULL != stream);
    assert(NULL != snv);
    assert(NULL != mnv);
    assert(NULL != seq);

    char reference[128] = {'\0'};
    int start = 0;
    int end = 0;
    int phase = 0;
    int len = 0;
    char inserted[1024] = {'\0'};

    size_t count = 0;
    while (6 == fscanf(stream, "%127s %d %d %d %d %1023s", reference,
                                                           &start,
                                                           &end,
                                                           &phase,
                                                           &len,
                                                           inserted))
    {
        if (len > 1023)
        {
            break;
        } // if

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
                break;
            } // if
        } // if
        else
        {
            char const* const restrict ins_ptr =
                vrd_seq_table_insert(seq, len, inserted);
            if (0 != len && NULL == ins_ptr)
            {
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
                break;
            } // if

        } // else
        count += 1;
    } // while

    return count;
} // vrd_variants_from_file
