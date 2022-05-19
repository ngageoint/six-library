// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef CODA_OSS_pch_h_INCLUDED_
#define CODA_OSS_pch_h_INCLUDED_

#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#pragma warning(disable: 4710) // '...': function not inlined
#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if / Qspectre switch specified
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'

// add headers that you want to pre-compile here
#include "framework.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <limits>
#include <complex>
#include <memory>
#include <new>
#include <utility>
#include <type_traits>
#include <typeinfo>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

#include <std/span>
#include <std/string>
#include <std/filesystem>
#include <std/memory>
#include <std/cstddef>
#include <std/optional>
#include <std/type_traits>

#pragma warning(disable: 4514) //	'...': unreferenced inline function has been removed

#endif //CODA_OSS_pch_h_INCLUDED_
