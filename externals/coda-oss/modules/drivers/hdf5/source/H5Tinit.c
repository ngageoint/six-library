// This file is generated as part of the build process.
// It isn't necessary different for each compiler, but that's
// probably a slightly better discriminant than OS/platform.

// These have a ".c_" extention so the normal build process won't find them.
#if defined(_MSC_VER)
#include "H5Tinit_MSVC.c_"
#elif defined(__GNUC__)
#include "H5Tinit_GCC.c_"
#else
#error "Unknown compiler."
#endif
