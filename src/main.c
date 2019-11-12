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

    vrd_Seq_Table* seq = vrd_Seq_table_init(100);
    if (NULL == seq)
    {
        (void) fprintf(stderr, "vrd_Seq_table_init() failed\n");
        goto error;
    } // if

    (void) fprintf(stderr, "q: %p\n", vrd_Seq_table_query(seq, 4, "bar"));

    vrd_Seq_table_insert(seq, 4, "bar");
    (void) fprintf(stderr, "q: %p\n", vrd_Seq_table_query(seq, 4, "bar"));
    vrd_Seq_table_insert(seq, 4, "bar");
    (void) fprintf(stderr, "q: %p\n", vrd_Seq_table_query(seq, 4, "bar"));

    vrd_Seq_table_remove(seq, 0);
    (void) fprintf(stderr, "q: %p\n", vrd_Seq_table_query(seq, 4, "bar"));
    vrd_Seq_table_remove(seq, 0);
    (void) fprintf(stderr, "q: %p\n", vrd_Seq_table_query(seq, 4, "bar"));
    vrd_Seq_table_remove(seq, 0);
    (void) fprintf(stderr, "q: %p\n", vrd_Seq_table_query(seq, 4, "bar"));
    vrd_Seq_table_remove(seq, 0);
    (void) fprintf(stderr, "q: %p\n", vrd_Seq_table_query(seq, 4, "bar"));


/*
    vrd_Seq_table_free_list_print(seq);

    if (NULL == vrd_Seq_table_insert(seq, 7, "romane"))
    {
        (void) fprintf(stderr, "vrd_Seq_table_insert() failed\n");
        goto error;
    } // if
    if (NULL == vrd_Seq_table_insert(seq, 8, "romanus"))
    {
        (void) fprintf(stderr, "vrd_Seq_table_insert() failed\n");
        goto error;
    } // if
    if (NULL == vrd_Seq_table_insert(seq, 8, "romulus"))
    {
        (void) fprintf(stderr, "vrd_Seq_table_insert() failed\n");
        goto error;
    } // if
    if (NULL == vrd_Seq_table_insert(seq, 7, "rubens"))
    {
        (void) fprintf(stderr, "vrd_Seq_table_insert() failed\n");
        goto error;
    } // if
    if (NULL == vrd_Seq_table_insert(seq, 6, "ruber"))
    {
        (void) fprintf(stderr, "vrd_Seq_table_insert() failed\n");
        goto error;
    } // if
    if (NULL == vrd_Seq_table_insert(seq, 8, "rubicon"))
    {
        (void) fprintf(stderr, "vrd_Seq_table_insert() failed\n");
        goto error;
    } // if
    if (NULL == vrd_Seq_table_insert(seq, 11, "rubicundus"))
    {
        (void) fprintf(stderr, "vrd_Seq_table_insert() failed\n");
        goto error;
    } // if

    vrd_Seq_table_free_list_print(seq);

    void* q = NULL;

    q = vrd_Seq_table_query(seq, 7, "romane");
    (void) fprintf(stderr, "query: %p (%zu)\n", q, NULL != q ? *(size_t*) q : -1);

    vrd_Seq_table_remove(seq, 7);

    q = vrd_Seq_table_query(seq, 7, "romane");
    (void) fprintf(stderr, "query: %p (%zu)\n", q, NULL != q ? *(size_t*) q : -1);

    vrd_Seq_table_free_list_print(seq);

    vrd_Seq_table_write(seq, "seq");

    vrd_Seq_table_destroy(&seq);

    seq = vrd_Seq_table_init(100);

    vrd_Seq_table_free_list_print(seq);

    (void) fprintf(stderr, "read: %d\n", vrd_Seq_table_read(seq, "seq"));

    vrd_Seq_table_free_list_print(seq);

*/

    vrd_Seq_table_destroy(&seq);

    return EXIT_SUCCESS;

error:
    {
        vrd_Seq_table_destroy(&seq);

        return EXIT_FAILURE;
    }
} // main
