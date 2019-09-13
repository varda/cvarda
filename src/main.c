#include <ctype.h>      // isprint
#include <stdint.h>     // SIZE_MAX
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // EXIT_*, malloc, free

#include "../include/varda.h"   // vrd_*


enum
{
    ASCII_SIZE = 95
}; // constants


static inline size_t
ascii_to_idx(char const ch)
{
    if (isprint(ch))
    {
        return ch - ' ';
    } // if
    return ASCII_SIZE;
} // ascii_to_idx


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

    char* const restrict a = vrd_alloc(&vrd_malloc, 5);

    a[0] = 'h';
    a[1] = 'e';
    a[2] = 'l';
    a[3] = 'o';
    a[4] = '\0';

    fprintf(stderr, "a = %s\n", a);
    fprintf(stderr, "a = %s\n", (char*) vrd_deref(&vrd_malloc, a));

    vrd_dealloc(&vrd_malloc, a);

    vrd_Alloc* restrict pool = vrd_pool_init(0, sizeof(int));

    void* const restrict ptr = vrd_alloc(pool, 1);

    int* const restrict b = vrd_deref(pool, ptr);
    *b = 6;

    fprintf(stderr, "b = %d\n", *b);
    fprintf(stderr, "ptr = %zu\n", (size_t) ptr);
    fprintf(stderr, "b = %d\n", *(int*) vrd_deref(pool, ptr));


    vrd_pool_destroy(&pool);

    return EXIT_SUCCESS;
} // main
