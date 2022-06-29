// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H
#pragma once

#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#pragma warning(disable: 4710) // '...': function not inlined
#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if / Qspectre switch specified
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
// TODO: get rid of these someday?
#pragma warning(disable: 5039) //	'...': pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc. Undefined behavior may occur if this function throws an exception.
#pragma warning(disable: 4514) //	'...': unreferenced inline function has been removed

#pragma warning(push)
#pragma warning(disable: 4464) // relative include path contains '..'
#pragma warning(disable: 4365) // '...': conversion from '...' to '...', signed/unsigned mismatch
#include "../modules/c++/cpp.h"
#pragma warning(pop)
#pragma comment(lib, "ws2_32")

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1

#pragma warning(disable: 5032) // detected #pragma warning(push) with no corresponding #pragma warning(pop)
#pragma warning(push)
#pragma warning(disable: 4464) // relative include path contains '..'
#include <nitf/coda-oss.hpp>
#pragma warning(disable: 5031) // #pragma warning(pop): likely mismatch, popping warning state pushed in different file
#pragma comment(lib, "io-c++")
#pragma comment(lib, "except-c++")
#pragma comment(lib, "sys-c++")
#pragma comment(lib, "str-c++")
#pragma warning(pop)

#pragma warning(push)
#include "CppUnitTest.h"
#pragma warning(pop)

#include <import/nrt.h>
#include <import/nitf.h>
#include <nitf/System.hpp>

#include "nitf_Test.h"
#include "Test.h"

#endif //PCH_H
