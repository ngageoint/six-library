/* =========================================================================
 * This file is part of std-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2023, Maxar Technologies, Inc.
 *
 * std-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */


#pragma once
#ifndef CODA_OSS_import_std_h_INCLUDED_
#define CODA_OSS_import_std_h_INCLUDED_

#include "coda_oss/CPlusPlus.h"
#include "config/disable_compiler_warnings.h"

CODA_OSS_disable_warning_system_header_push
#if _MSC_VER
#pragma warning(disable: 4619) // #pragma warning: there is no warning number '...'

#pragma warning(disable: 5027) // '...': move assignment operator was implicitly defined as deleted
#pragma warning(disable: 5026) // '...': move constructor was implicitly defined as deleted
#pragma warning(disable: 4626) // '...': assignment operator was implicitly defined as deleted
#pragma warning(disable: 4625) // '...': copy constructor was implicitly defined as deleted
#pragma warning(disable: 4355) // '...': used in base member initializer list
#pragma warning(disable: 5220) // '...': a non-static data member with a volatile qualified type no longer implies that compiler generated copy/move constructors and copy/move assignment operators are not trivial
#pragma warning(disable: 5204) // '...': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
#pragma warning(disable: 5264)  // '...': '...' variable is not used
#endif // _MSC_VER

#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cinttypes>
#include <cfloat>
#include <climits>
#include <cassert>
#include <ctime>

#include <limits>
#include <memory>
#include <new>

#include <complex>
#include <utility>
#include <tuple>
#include <typeinfo>
#include <type_traits>
#include <exception>
#include <stdexcept>
#include <locale>

#include <mutex>
#include <atomic>
#include <thread>
#include <future>

#include <ios>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <streambuf>

#include <array>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <stack>

#include <numeric>
#include <functional>
#include <algorithm>
#include <iterator>
#include <random>

//#include <coda_oss/filesystem.h>
#include <coda_oss/bit.h>
#include <coda_oss/cstddef.h>
#include <coda_oss/numbers.h>
#include <coda_oss/optional.h>
#include <coda_oss/span.h>
#include <coda_oss/string.h>
#include <coda_oss/type_traits.h>

CODA_OSS_disable_warning_pop

#endif // CODA_OSS_import_std_h_INCLUDED_


