// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <WinSock.h>
#include <windows.h>
#undef min
#undef max

#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#include <locale>
#include <array>
#include <string>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <std/string>
#include <std/span>
#include <std/optional>
#include <std/cstddef>
#include <std/bit>

#include "CppUnitTest.h"

#include "gsl/gsl.h"
#include "import/sys.h"
#include "import/math.h"
#include "import/str.h"
#include "str/EncodedStringView.h"
#include "import/except.h"
#include "import/mem.h"
#include "include/TestCase.h"

#include "TestCase.h"

#endif //PCH_H
