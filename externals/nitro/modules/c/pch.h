#pragma once

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1

#include "config/disable_compiler_warnings.h"

// TODO: get rid of these someday? ... from Visual Studio code-analysis
#pragma warning(disable: 6385) // Reading invalid data from '...':  the readable size is '...' bytes, but '...' bytes may be read.
#pragma warning(disable: 6386) // Buffer overrun while writing to '...':  the writable size is '...' bytes, but '...' bytes might be written.

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#if defined(_WIN32)
#undef BIGENDIAN
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <memory.h>
#include <stdint.h>

#include <jpeglib.h>
#include <jerror.h>

#include <sys/types.h>

#pragma warning(disable: 4505) // '...': unreferenced function with internal linkage has been removed
#pragma warning(disable: 4774) // '...' : format string expected in argument 3 is not a string literal

#if __cplusplus
#pragma warning(disable: 4365) // '...': conversion from '...' to '...', signed/unsigned mismatch
#pragma warning(disable: 4800) // Implicit conversion from '...' to bool. Possible information loss
#pragma warning(disable: 4804) // '...: unsafe use of type '...' in operation
#pragma warning(disable: 5219) // implicit conversion from '...' to '...', possible loss of data
#pragma warning(disable: 4191) // '...': unsafe conversion from '...' to '...'
#pragma warning(disable: 4244) // '...': conversion from '...' to '...', possible loss of data
#endif

#define OPJ_STATIC
