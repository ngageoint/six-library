// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#pragma warning(push)
#pragma warning(disable: 5039) //	'...': pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc. Undefined behavior may occur if this function throws an exception.
#pragma warning(disable: 5105) // macro expansion producing '...' has undefined behavior
#include <windows.h>
#pragma warning(pop)
#pragma comment(lib, "ws2_32")

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1
#ifndef CODA_OSS_LIBRARY_SHARED
#define CODA_OSS_LIBRARY_SHARED 1
#endif

#include <import/nrt.h>
#include <import/nitf.h>
#include <nitf/TestingTest.h>

#include <nitf/coda-oss.hpp>

#include <import/nitf.hpp>
#include <nitf/UnitTests.hpp>
#include <nitf/TREField.hpp>
#include <nitf/TestingTest.hpp>
#include <nitf/UnitTests.hpp>

#include "Test.h"

#endif //PCH_H
