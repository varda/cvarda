#ifndef TREE_H
#define TREE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stdint.h>     // uint32_t


enum
{
    NULLPTR = 0,

    LEFT =  0,
    RIGHT = 1,

    HOMOZYGOUS = (1 << 28) - 1
}; // constants


static inline size_t
umax(size_t const a, size_t const b)
{
    return a > b ? a : b;
} // max


#ifdef __cplusplus
} // extern "C"
#endif

#endif
