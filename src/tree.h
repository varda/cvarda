#ifndef TREE_H
#define TREE_H

#ifdef __cplusplus
#define restrict
extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // UINT64_C


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
} // umax


static inline int
ilog2(size_t const x)
{
    return 64 - __builtin_clzll(x);
} // ilog2


static inline size_t
ipow2(int const x)
{
    return (UINT64_C(1) << x);
} // ipow2


#ifdef __cplusplus
} // extern "C"
#endif

#endif
