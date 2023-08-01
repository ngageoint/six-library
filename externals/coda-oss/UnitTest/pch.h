// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#pragma warning(push)
#pragma warning(disable: 5105) // macro expansion producing '...' has undefined behavior
#include <WinSock.h>
#include <windows.h>
#include <comdef.h>
#pragma warning(pop)
#undef min
#undef max

#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include "import/std.h"
#include <std/bit>
#include <std/cstddef>
#include <std/filesystem>
#include <std/numbers>
#include <std/optional>
#include <std/span>
#include <std/string>
#include <std/type_traits>

#include "CppUnitTest.h"

#include <gsl/gsl.h>
#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>
#include <import/math.h>
#include <import/math/linear.h>
#include <import/math/poly.h>
#include <import/mem.h>
#include <import/cli.h>
#include <import/mt.h>
#include <import/logging.h>
#include <import/re.h>
#include <import/zip.h>
#include <import/xml/lite.h>
#include <import/hdf5/lite.h>

#include <io/ReadUtils.h>
#include <hdf5/lite/highfive.h>

#include "TestCase.h"

#endif //PCH_H
