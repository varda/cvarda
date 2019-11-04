#ifndef VRD_IUPAC_H
#define VRD_IUPAC_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t


enum
{
    VRD_IUPAC_SIZE = 16
}; // constants


static inline size_t
vrd_iupac_to_idx(char const ch)
{
    switch (ch)
    {
        case '.':
        case 'Z':
        case 'z':
            return 0;
        case 'A':
        case 'a':
            return 1;
        case 'C':
        case 'c':
            return 2;
        case 'M':
        case 'm':
            return 3;
        case 'G':
        case 'g':
            return 4;
        case 'R':
        case 'r':
            return 5;
        case 'S':
        case 's':
            return 6;
        case 'V':
        case 'v':
            return 7;
        case 'T':
        case 't':
        case 'U':
        case 'u':
            return 8;
        case 'W':
        case 'w':
            return 9;
        case 'Y':
        case 'y':
            return 10;
        case 'H':
        case 'h':
            return 11;
        case 'K':
        case 'k':
            return 12;
        case 'D':
        case 'd':
            return 13;
        case 'B':
        case 'b':
            return 14;
        case 'N':
        case 'n':
            return 15;
    } // switch
    return VRD_IUPAC_SIZE;
} // vrd_iupac_to_idx


#ifdef __cplusplus
} // extern "C"
#endif

#endif
