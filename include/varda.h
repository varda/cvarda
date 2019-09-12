#ifndef VARDA_H
#define VARDA_H

#ifdef __cplusplus
extern "C"
{
#endif


static inline void
version(int* const restrict major,
        int* const restrict minor,
        int* const restrict patch)
{
    *major = VRD_VERSION_MAJOR;
    *minor = VRD_VERSION_MINOR;
    *patch = VRD_VERSION_PATCH;
} // version


#ifdef __cplusplus
} // extern "C"
#endif

#endif
