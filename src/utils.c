#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, fprintf, fscanf
#include <string.h>     // strlen

#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_avl_tree_*
#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_cov_table_*
#include "../include/iupac.h"       // vrd_iupac_to_idx
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_mnv_table_*
#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_seq_table_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_snv_table_*
#include "../include/utils.h"       // vrd_coverage_from_file,
                                    // vrd_variants_from_file
#include "tree.h"   // HOMOZYGOUS


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
    while (3 == fscanf(stream, "%127s %d %d", reference, &start, &end)) // UNSAFE
    {
        if (-1 == vrd_cov_table_insert(cov,
                                       strlen(reference),
                                       reference,
                                       start,
                                       end,
                                       sample_id))
        {
            vrd_AVL_Tree* restrict subset = vrd_avl_tree_init(1);
            if (NULL == subset)
            {
                return count;
            } // if
            if (NULL == vrd_avl_tree_insert(subset, sample_id))
            {
                return count;
            } // if

            count -= vrd_cov_table_remove(cov, subset);
            vrd_avl_tree_destroy(&subset);
            return count;
        } // if
        count += 1; // OVERFLOW
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
                                                           inserted)) // UNSAFE
    {
        if (1023 < len)
        {
            goto error;
        } // if

        if (-1 == phase)
        {
            phase = HOMOZYGOUS;
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
                goto error;
            } // if
        } // if
        else
        {
            char const* const restrict ins_ptr =
                vrd_seq_table_insert(seq, len, inserted);
            if (0 != len && NULL == ins_ptr)
            {
                goto error;
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
                goto error;
            } // if

        } // else
        count += 1; // OVERFLOW
    } // while

    return count;

error:
    {
        vrd_AVL_Tree* restrict subset = vrd_avl_tree_init(1);
        if (NULL == subset)
        {
            return count;
        } // if
        if (NULL == vrd_avl_tree_insert(subset, sample_id))
        {
            return count;
        } // if

        count -= vrd_snv_table_remove(snv, subset);
        count -= vrd_mnv_table_remove(mnv, subset);
        vrd_avl_tree_destroy(&subset);
    }
    return count;
} // vrd_variants_from_file


size_t
vrd_annotate_from_file(FILE* restrict ostream,
                       FILE* restrict istream,
                       vrd_Cov_Table const* const restrict cov,
                       vrd_SNV_Table const* const restrict snv,
                       vrd_MNV_Table const* const restrict mnv,
                       vrd_Seq_Table const* const restrict seq,
                       vrd_AVL_Tree const* const restrict subset)
{
    assert(NULL != ostream);
    assert(NULL != istream);
    assert(NULL != cov);
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
    while (6 == fscanf(istream, "%127s %d %d %d %d %1023s", reference,
                                                            &start,
                                                            &end,
                                                            &phase,
                                                            &len,
                                                            inserted)) // UNSAFE
    {
        if (1023 < len)
        {
            break;
        } // if

        size_t num = 0;
        if (1 == len && inserted[0] != '.' && 1 == end - start)
        {
            num = vrd_snv_table_query(snv,
                                      strlen(reference),
                                      reference,
                                      start,
                                      vrd_iupac_to_idx(inserted[0]),
                                      subset);
        } // if
        else
        {
            char const* const restrict ins_ptr =
                vrd_seq_table_query(seq, len, inserted);

            num = vrd_mnv_table_query(mnv,
                                      strlen(reference),
                                      reference,
                                      start,
                                      end,
                                      ins_ptr,
                                      subset);
        } // else

        size_t const den = vrd_cov_table_query(cov,
                                               strlen(reference),
                                               reference,
                                               start,
                                               end,
                                               subset) * 2;

        (void) fprintf(ostream, "%s\t%d\t%d\t%d\t%d\t%s\t%zu:%zu\n",
                       reference,
                       start,
                       end,
                       phase,
                       len,
                       inserted,
                       num,
                       den);

        count += 1; // OVERFLOW
    } // while

    return count;
} // vrd_annotate_from_file
