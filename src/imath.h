#ifndef VRD_IMATH_H
#define VRD_IMATH_H

#ifdef __cplusplus
extern "C"
{
#endif


#include <stddef.h>     // size_t
#include <stdint.h>     // UINT64_C


static inline int
ilog2(size_t const x)
{
    return 64 - __builtin_clzll(x);  // GCC builtin also works with clang
} // ilog2


static inline size_t
ipow2(int const x)
{
    return (UINT64_C(1) << x);
} // ipow2


static inline size_t
umax(size_t const a, size_t const b)
{
    return a > b ? a : b;
} // umax


static inline int
bittest(size_t const n, int const i)
{
    if (n & (UINT64_C(1) << i))
    {
        return 1;
    } // if
    return 0;
} // bittest


#ifdef __cplusplus
} // extern "C"
#endif

#endif
