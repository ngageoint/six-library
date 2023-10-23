// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H
#pragma once

// add headers that you want to pre-compile here

//
// pch.h
// Header for standard system include files.
//
#pragma once

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1

#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#pragma warning(disable: 4710) // '...': function not inlined

#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if / Qspectre switch specified

#pragma warning(disable: 4625) // '...': copy constructor was implicitly defined as deleted
#pragma warning(disable: 4626) // '...': assignment operator was implicitly defined as deleted
#pragma warning(disable: 5026) // '...': move constructor was implicitly defined as deleted
#pragma warning(disable: 5027) //	'...': move assignment operator was implicitly defined as deleted

// TODO: get rid of these someday? ... from Visual Studio code-analysis
#pragma warning(disable: 5039) //	'...': pointer or reference to potentially throwing function passed to 'extern "C"' function under - EHc.Undefined behavior may occur if this function throws an exception.

#pragma warning(disable: 26495) // Variable '...' is uninitialized. Always initialize a member variable(type.6).
#pragma warning(disable: 26451) // Arithmetic overflow : Using operator '...' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '*' to avoid overflow(io.2).
#pragma warning(disable: 6385) // Reading invalid data from '...':  the readable size is '...' bytes, but '...' bytes may be read.
#pragma warning(disable: 6386) // Buffer overrun while writing to '...':  the writable size is '...' bytes, but '...' bytes might be written.
#pragma warning(disable: 5219) // implicit conversion from '...' to '...', possible loss of data
#pragma warning(disable: 5204) // '...': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
#pragma warning(disable: 5220) // '...': a non-static data member with a volatile qualified type no longer implies
#pragma warning(disable: 4355) // '...': used in base member initializer list
#pragma warning(disable: 5105) // macro expansion producing '...' has undefined behavior

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#undef min
#undef max

#include <sstream>
#include <iostream>
#include <complex>

#include <import/nitf.hpp>
#include <io/FileInputStream.h>

#pragma comment(lib, "io-c++")
#pragma comment(lib, "except-c++")
#pragma comment(lib, "sys-c++")
#pragma comment(lib, "str-c++")

#pragma comment(lib, "ws2_32")


#endif //PCH_H