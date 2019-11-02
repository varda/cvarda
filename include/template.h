#ifndef VRD_TEMPLATE_H
#define VRD_TEMPLATE_H


#define VRD_TEMPLATE_WRAP(suffix, ...) vrd_ ## __VA_ARGS__ ## suffix

#define VRD_TEMPLATE(type, suffix) VRD_TEMPLATE_WRAP(suffix, type)


#endif
