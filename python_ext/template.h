#ifndef VRD_PY_TEMPLATE_H
#define VRD_PY_TEMPLATE_H


#define VRD_PY_TEMPLATE_WRAP(suffix, ...) __VA_ARGS__ ## suffix

#define VRD_PY_TEMPLATE(type, suffix) VRD_PY_TEMPLATE_WRAP(suffix, type)


#define VRD_PY_STRINGIZE_WRAP(type) # type

#define VRD_PY_STRINGIZE(type) VRD_PY_STRINGIZE_WRAP(type)


#endif
