#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*

#include "../include/varda.h"   // vrd_*


struct Test
{
    size_t test;
}; // Test


int
main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    int major = 0;
    int minor = 0;
    int patch = 0;
    vrd_version(&major, &minor, &patch);

    (void) fprintf(stderr, "cvarda version %d.%d.%d\n", major,
                                                        minor,
                                                        patch);

    vrd_Pool* restrict pool = vrd_pool_init(10002, sizeof(struct Test));
    if (NULL == pool)
    {
        (void) fprintf(stderr, "vrd_pool_init() failed\n");
        return EXIT_FAILURE;
    } // if

    vrd_pool_destroy(&pool);

    return EXIT_SUCCESS;
} // main
