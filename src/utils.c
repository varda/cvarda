#include <assert.h>     // assert
#include <stddef.h>     // NULL, size_t
#include <stdio.h>      // FILE, fprintf, fscanf
#include <string.h>     // strlen

#include "../include/avl_tree.h"    // vrd_AVL_Tree, vrd_AVL_tree_*
#include "../include/constants.h"   // VRD_HOMOZYGOUS
#include "../include/cov_table.h"   // vrd_Cov_Table, vrd_Cov_table_*
#include "../include/iupac.h"       // vrd_iupac_to_idx
#include "../include/mnv_table.h"   // vrd_MNV_Table, vrd_MNV_table_*
#include "../include/seq_table.h"   // vrd_Seq_Table, vrd_Seq_table_*
#include "../include/snv_table.h"   // vrd_SNV_Table, vrd_SNV_table_*
#include "../include/trie.h"        // vrd_Trie_Node
#include "../include/utils.h"       // vrd_coverage_from_file,
                                    // vrd_variants_from_file,
                                    // vrd_annotate_from_file


size_t
vrd_coverage_from_file(FILE* stream,
                       vrd_Cov_Table* const cov,
                       size_t const sample_id)
{
    assert(NULL != stream);
    assert(NULL != cov);

    char reference[128] = {'\0'};
    size_t start = 0;
    size_t end = 0;
    size_t allele_count = 0;

    size_t line_count = 0;
    while (4 == fscanf(stream, "%127s %zu %zu %zu", reference, &start, &end, &allele_count))  // UNSAFE
    {
        if (0 != vrd_Cov_table_insert(cov, strlen(reference) + 1, reference, start, end, allele_count, sample_id))
        {
            vrd_AVL_Tree* subset = vrd_AVL_tree_init(1);
            if (NULL == subset)
            {
                return line_count;
            } // if
            if (0 != vrd_AVL_tree_insert(subset, sample_id))
            {
                return line_count;
            } // if

            line_count -= vrd_Cov_table_remove(cov, subset);
            vrd_AVL_tree_destroy(&subset);
            return line_count;
        } // if
        line_count += 1;  // OVERFLOW
    } // while

    return line_count;
} // vrd_coverage_from_file


size_t
vrd_variants_from_file(FILE* stream,
                       vrd_SNV_Table* const snv,
                       vrd_MNV_Table* const mnv,
                       vrd_Seq_Table* const seq,
                       size_t const sample_id)
{
    assert(NULL != stream);
    assert(NULL != snv);
    assert(NULL != mnv);
    assert(NULL != seq);

    char reference[128] = {'\0'};
    size_t start = 0;
    size_t end = 0;
    size_t allele_count = 0;
    size_t phase = 0;
    size_t len = 0;
    char inserted[1024] = {'\0'};

    size_t line_count = 0;
    while (7 == fscanf(stream, "%127s %zu %zu %zu %zu %zu %1023s", reference, &start, &end, &allele_count, &phase, &len, inserted))  // UNSAFE
    {
        if (1023 < len)
        {
            goto error;
        } // if

        if ((size_t) -1 == phase)
        {
            phase = VRD_HOMOZYGOUS;
        } // if

        if (1 == len && inserted[0] != '.' && 1 == end - start)
        {
            if (0 != vrd_SNV_table_insert(snv, strlen(reference) + 1, reference, start, allele_count, sample_id, phase, vrd_iupac_to_idx(inserted[0])))
            {
                goto error;
            } // if
        } // if
        else
        {
            if (0 == len)
            {
                inserted[0] = '\0';
            } // if

            vrd_Trie_Node* const elem = vrd_Seq_table_insert(seq, len + 1, inserted);
            if (NULL == elem)
            {
                goto error;
            } // if

            if (0 != vrd_MNV_table_insert(mnv, strlen(reference) + 1, reference, start, end, allele_count, sample_id, phase, *(size_t*) elem))
            {
                goto error;
            } // if

        } // else
        line_count += 1;  // OVERFLOW
    } // while

    return line_count;

error:
    {
        vrd_AVL_Tree* subset = vrd_AVL_tree_init(1);
        if (NULL == subset)
        {
            return line_count;
        } // if
        if (0 != vrd_AVL_tree_insert(subset, sample_id))
        {
            return line_count;
        } // if

        line_count -= vrd_SNV_table_remove(snv, subset);
        line_count -= vrd_MNV_table_remove_seq(mnv, subset, seq);
        vrd_AVL_tree_destroy(&subset);
        return line_count;
    }
} // vrd_variants_from_file


size_t
vrd_annotate_from_file(FILE* ostream,
                       FILE* istream,
                       vrd_Cov_Table const* const cov,
                       vrd_SNV_Table const* const snv,
                       vrd_MNV_Table const* const mnv,
                       vrd_Seq_Table const* const seq,
                       vrd_AVL_Tree const* const subset)
{
    assert(NULL != ostream);
    assert(NULL != istream);
    assert(NULL != cov);
    assert(NULL != snv);
    assert(NULL != mnv);
    assert(NULL != seq);

    char reference[128] = {'\0'};
    size_t start = 0;
    size_t end = 0;
    size_t allele_count = 0;
    size_t phase = 0;
    size_t len = 0;
    char inserted[1024] = {'\0'};

    size_t line_count = 0;
    while (7 == fscanf(istream, "%127s %zu %zu %zu %zu %zu %1023s", reference, &start, &end, &allele_count, &phase, &len, inserted))  // UNSAFE
    {
        if (1023 < len)
        {
            break;
        } // if

        size_t num = 0;
        if (1 == len && inserted[0] != '.' && 1 == end - start)
        {
            num = vrd_SNV_table_query(snv, strlen(reference) + 1, reference, start, vrd_iupac_to_idx(inserted[0]), subset);
        } // if
        else
        {
            if (0 == len)
            {
                inserted[0] = '\0';
            } // if

            vrd_Trie_Node* const elem = vrd_Seq_table_query(seq, len + 1, inserted);
            if (NULL == elem)
            {
                num = 0;
            } // if
            else
            {
                num = vrd_MNV_table_query(mnv, strlen(reference) + 1, reference, start, end, *(size_t*) elem, subset);
            } // else
        } // else

        size_t const den = vrd_Cov_table_query_stab(cov, strlen(reference) + 1, reference, start, end, subset);

        (void) fprintf(ostream, "%s\t%zu\t%zu\t%zu\t%s\t%zu:%zu\n", reference, start, end, len, inserted, num, den);  // UNCHECKED

        line_count += 1;  // OVERFLOW
    } // while

    return line_count;
} // vrd_annotate_from_file
