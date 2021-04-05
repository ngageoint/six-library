#pragma once

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1

#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#pragma warning(disable: 4710) // '...': function not inlined
#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if / Qspectre switch specified

// changing this breaks SWIG
#pragma warning (disable: 26812) // The enum type '...' is unscoped. Prefer '...' over '...'

// TODO: get rid of these someday?
#pragma warning(disable: 5039) //	'...': pointer or reference to potentially throwing function passed to 'extern "C"' function under - EHc.Undefined behavior may occur if this function throws an exception.
#pragma warning(disable: 4514) //	'...': unreferenced inline function has been removed

// TODO: get rid of these someday? ... from Visual Studio code-analysis
#pragma warning(disable: 26401) // Do not delete a raw pointer that is not an owner<T>(i.11).
#pragma warning(disable: 26432) // If you define or delete any default operation in the type '...', define or delete them all(c.21).
#pragma warning(disable: 26434) // Function '...' hides a non-virtual function '...' (c.128).
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4)
#pragma warning(disable: 26456) // Operator '...' hides a non-virtual operator '...' (c.128).
#pragma warning(disable: 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
#pragma warning(disable: 26485) // Expression '...' : No array to pointer decay(bounds.3).
#pragma warning(disable: 26486) // Don't pass a pointer that may be invalid to a function. Parameter 3 '...' in call to '...' may be invalid (lifetime.3).
#pragma warning(disable: 26487) // Don't return a pointer '...' that may be invalid(lifetime.4).
#pragma warning(disable: 26440) // Function '...' can be declared '...' (f.6).


// need C++14 for std::make_unique<T>
#pragma warning(disable: 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead(r.11).


#pragma warning(push)
#pragma warning(disable: 4355) // '...': used in base member initializer list
#pragma warning(disable: 4625) // '...': copy constructor was implicitly defined as deleted
#pragma warning(disable: 4626) // '...': assignment operator was implicitly defined as deleted#pragma warning(disable: 5220) // '...': a non - static data member with a volatile qualified type no longer implies
#pragma warning(disable: 5026) // '...': move constructor was implicitly defined as deleted
#pragma warning(disable: 5027) //	'...': move assignment operator was implicitly defined as deleted
#pragma warning(disable: 5204) // '...': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
#pragma warning(disable: 5220) // '...': a non - static data member with a volatile qualified type no longer implies#pragma warning(disable: 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead(r.11).
#pragma warning(disable: 26401) // Do not delete a raw pointer that is not an owner<T>(i.11).
#pragma warning(disable: 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead(r.11).
#pragma warning(disable: 26429) // Symbol '...' is never tested for nullness, it can be marked as not_null(f.23).
#pragma warning(disable: 26426) // Global initializer calls a non-constexpr function '...' (i.22).
#pragma warning(disable: 26432) // If you define or delete any default operation in the type '...', define or delete them all(c.21).
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).
#pragma warning(disable: 26434) // Function '...' hides a non - virtual function '...' (c.128).
#pragma warning(disable: 26435) // Function '...' should specify exactly one of '...', '...', or '...' (c.128).
#pragma warning(disable: 26440) // Function '...' can be declared '...' (f.6).
#pragma warning(disable: 26443) // Overriding destructor should not use explicit '...' or '...' specifiers(c.128).
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '...' which may throw exceptions(f.6).
#pragma warning(disable: 26455) // Default constructor may not throw.Declare it '...' (f.6).
#pragma warning(disable: 26456) // Operator '...' hides a non - virtual operator '...' (c.128).
#pragma warning(disable: 26472) // Don't use a static_cast for arithmetic conversions. Use brace initialization, gsl::narrow_cast or gsl::narrow (type.1).
#pragma warning(disable: 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
#pragma warning(disable: 26495) // Variable '...' is uninitialized. Always initialize a member variable(type.6).

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#undef min
#undef max

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>

#include <string>
#include <numeric>
#include <limits>
#include <memory>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <ios>
#include <vector>
#include <map>
#include <utility>
#include <functional>
#include <mutex>
#include <atomic>
#include <future>

#include "nitf/coda-oss.hpp"
#pragma warning(pop)

#include "import/nitf.h"
#include "nitf/ImageIO.h"
#include "nitf/System.h"
#include "nitf/Field.h"
#include "nitf/Types.h"

#include "nitf/Object.hpp"

#pragma comment(lib, "io-c++")
#pragma comment(lib, "except-c++")
#pragma comment(lib, "sys-c++")
#pragma comment(lib, "str-c++")

#pragma comment(lib, "ws2_32")

