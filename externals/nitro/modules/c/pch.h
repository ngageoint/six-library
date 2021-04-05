#pragma once

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1

#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#pragma warning(disable: 4710) // '...': function not inlined
#pragma warning(disable: 4774) // '...' : format string expected in argument 3 is not a string literal
#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if / Qspectre switch specified

// TODO: get rid of these someday? ... from Visual Studio code-analysis
#pragma warning(disable: 6385) // Reading invalid data from '...':  the readable size is '...' bytes, but '...' bytes may be read.
#pragma warning(disable: 6386) // Buffer overrun while writing to '...':  the writable size is '...' bytes, but '...' bytes might be written.


#pragma warning(push)
#pragma warning(disable: 4255) // '...': no function prototype given : converting '...' to '...'

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

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

#include <sys/types.h>
#pragma warning(pop)

#pragma warning(disable: 4505) // '...': unreferenced local function has been removed
#pragma warning(disable: 4514) // '...' : unreferenced inline function has been removed

