#include <errno.h>      // errno
#include <stddef.h>     // NULL
#include <stdio.h>      // FILE, stderr, fprintf
#include <stdlib.h>     // EXIT_*
#include <string.h>     // strlen

#include "../include/varda.h"   // VRD_*, vrd_*


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    vrd_Seq_Table* seq = NULL;

    seq = vrd_seq_table_init(1000);
    if (NULL == seq)
    {
        (void) fprintf(stderr, "vrd_seq_table_init() failed\n");
        goto error;
    } // if

    char word[1024] = {'\0'};
    while (1 == scanf("%1023s", word))
    {
        (void) fprintf(stderr, "Adding: %s\n", word);
        void* const restrict elem = vrd_seq_table_insert(seq, strlen(word) + 1, word);
        if (NULL == elem)
        {
            (void) fprintf(stderr, "vrd_seq_table_insert() failed\n");
            goto error;
        } // if

        (void) fprintf(stderr, "ref: %zu\n", *(size_t*) elem);

    } // while




    vrd_seq_table_destroy(&seq);

    return EXIT_SUCCESS;

error:

    vrd_seq_table_destroy(&seq);

    return EXIT_FAILURE;
} // main
