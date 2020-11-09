//
// pch.h
// Header for standard system include files.
//

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// TODO: get rid of these someday? ... from Visual Studio code-analysis
#pragma warning(disable: 26495) // Variable '...' is uninitialized. Always initialize a member variable(type.6).
#pragma warning(disable: 26451) // Arithmetic overflow : Using operator '...' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '*' to avoid overflow(io.2).
#pragma warning(disable: 6385) // Reading invalid data from '...':  the readable size is '...' bytes, but '...' bytes may be read.
#pragma warning(disable: 6386) // Buffer overrun while writing to '...':  the writable size is '...' bytes, but '...' bytes might be written.

#pragma once

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>

#include <string>
#include <ios>

#include <windows.h>
#undef min
#undef max
#  include <direct.h>
#  include <io.h>
# include <sys/types.h>
# include <sys/stat.h>
#include <sys/Filesystem.h>

extern int close(int fd);
extern int read(int fd, void* buf, unsigned int count);
extern int write(int fd, const void* buf, unsigned int count);
extern int _isatty(int fd);
#include "gtest/gtest.h"

#include <import/nrt.h>
#include <nitf/System.hpp>

#include "nitf_Test.h"
#include "Test.h"

#pragma comment(lib, "io-c++")
#pragma comment(lib, "except-c++")
#pragma comment(lib, "sys-c++")
#pragma comment(lib, "str-c++")

#pragma comment(lib, "ws2_32")
