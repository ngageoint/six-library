// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#pragma once

// We're building in Visual Studio ... used to control where we get a little bit
// of config info
#define CODA_OSS_PCH 1
#ifndef CODA_OSS_LIBRARY_SHARED
#define CODA_OSS_LIBRARY_SHARED 1
#endif

#include "coda_oss/CPlusPlus.h"
#include "config/disable_compiler_warnings.h"

CODA_OSS_disable_warning_system_header_push

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
#include <stdarg.h>

CODA_OSS_disable_warning_pop

#include "import/std.h"


#include <net/net_config.h>
#include <zlib.h>
#pragma comment(lib, "z.lib")
#pragma comment(lib, "minizip.lib")

#include <xml/lite/xml_lite_config.h>

#pragma warning(disable: 4251) // '...': class '...' needs to have dll-interface to be used by clients of class '...'
#pragma warning(disable: 4365) // '...': conversion from '...' to '...', signed/unsigned mismatch
#pragma warning(disable: 5039) // '...': pointer or reference to potentially throwing function passed to '...' function under -EHc. Undefined behavior may occur if this function throws an exception.
#pragma warning(disable: 6054) // String '...' might not be zero-terminated.
#pragma warning(disable: 6387) // '...' could be '...'.
#pragma warning(disable: 6305) // Potential mismatch between sizeof and countof quantities. Use sizeof() to scale byte sizes.
#pragma warning(disable: 6389) // Move '...' to anonymous namespace or put a forward declaration in a common header included in this file.
#pragma warning(disable: 5267) // definition of implicit copy constructor for '...' is deprecated because it has a user-provided destructor

#pragma warning(disable: 26822) // Dereferencing a null pointer '...' (lifetime.1).
#pragma warning(disable: 26432) // If you define or delete any default operation in the type '...', define or delete them all (c.21).
#pragma warning(disable: 26429) // Symbol '...' is never tested for nullness, it can be marked as not_null (f.23).
#pragma warning(disable: 26455) // Default constructor should not throw. Declare it '...' (f.6).
#pragma warning(disable: 26440) // Function '...' can be declared '...' (f.6).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '...' which may throw exceptions (f.6).
#pragma warning(disable: 26496) // The variable '...' does not change after construction, mark it as const (con.4).
#pragma warning(disable: 26462) // The value pointed to by '...' is assigned only once, mark it as a pointer to const (con.4).
#pragma warning(disable: 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead (r.11).
#pragma warning(disable: 26401) // Do not delete a raw pointer that is not an owner<T> (i.11).
#pragma warning(disable: 26472) // Don't use a static_cast for arithmetic conversions. Use brace initialization, gsl::narrow_cast or gsl::narrow (type.1).
#pragma warning(disable: 26485) // Expression '...': No array to pointer decay (bounds.3).
#pragma warning(disable: 26493) // Don't use C-style casts (type.4).
#pragma warning(disable: 26451) // Arithmetic overflow: Using operator '...' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '...' to avoid overflow (io.2).
#pragma warning(disable: 26494) // Variable '...' is uninitialized. Always initialize an object (type.5).
#pragma warning(disable: 26814) // The const variable '...' can be computed at compile-time. Consider using constexpr (con.5).
#pragma warning(disable: 26826) // Don't use C-style variable arguments (f.55).
#pragma warning(disable: 26435) // Function '...' should specify exactly one of '...', '...', or '...' (c.128).
#pragma warning(disable: 26434) // Function '...' hides a non-virtual function '...'.
#pragma warning(disable: 26400) // Do not assign the result of an allocation or a function call with an owner<T> return value to a raw pointer, use owner<T> instead (i.11).
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).
#pragma warning(disable: 26456) // Operator '...' hides a non-virtual operator '...' (c.128).
#pragma warning(disable: 26465) // Don't use const_cast to cast away const or volatile. const_cast is not required; constness or volatility is not being removed by this conversion (type.3).
#pragma warning(disable: 26408) // Avoid malloc() and free(), prefer the nothrow version of new with delete (r.10).
#pragma warning(disable: 26471) // Don't use reinterpret_cast. A cast from void* can use static_cast (type.1).
#pragma warning(disable: 26490) // Don't use reinterpret_cast (type.1).
#pragma warning(disable: 26460) // The reference argument '...' for function '...' can be marked as const (con.3).
#pragma warning(disable: 26495) // Variable '...' is uninitialized. Always initialize a member variable (type.6).
#pragma warning(disable: 26461) // The pointer argument '...' for function '...' can be marked as a pointer to const (con.3).
#pragma warning(disable: 26436) // The type '...' with a virtual function needs either public virtual or protected non-virtual destructor (c.35).
#pragma warning(disable: 26467) // Converting from floating point to unsigned integral types results in non-portable code if the double/float has a negative value. Use gsl::narrow_cast or gsl::narrow instead to guard against undefined behavior and potential data loss (es.46).
#pragma warning(disable: 26135) // Missing annotation _Acquires_lock_(this->mNative) at function '...'.
#pragma warning(disable: 28159) // Consider using '...' instead of '...'. Reason: Deprecated. Use VerifyVersionInfo* or IsWindows* macros from VersionHelpers.
#pragma warning(disable: 26439) // This kind of function should not throw. Declare it '...' (f.6).
#pragma warning(disable: 26454) // Arithmetic overflow: '...' operation produces a negative unsigned result at compile time (io.5).
#pragma warning(disable: 26492) // Don't use const_cast to cast away const or volatile (type.3).
#pragma warning(disable: 26110) // Caller failing to hold lock '...' before calling function '...'.
#pragma warning(disable: 26459) // You called an STL function '...' with a raw pointer parameter at position '...' that may be unsafe - this relies on the caller to check that the passed values are correct. Consider wrapping your range in a gsl::span and pass as a span iterator (stl.1).
#pragma warning(disable: 26473) // Don't cast between pointer types where the source type and the target type are the same (type.1).
#pragma warning(disable: 26491) // Don't use static_cast downcasts (type.2).
#pragma warning(disable: 26405) // Do not assign to an owner<T> which may be in valid state (r.3).
#pragma warning(disable: 26403) // Reset or explicitly delete an owner<T> pointer '...' (r.3).
#pragma warning(disable: 26489) // Don't dereference a pointer that may be invalid: '...'. '...' may have been invalidated at line 132 (lifetime.1).
#pragma warning(disable: 26486) // Don't pass a pointer that may be invalid to a function. Parameter 0 '...' in call to '...' may be invalid (lifetime.3).
#pragma warning(disable: 26823) // Dereferencing a possibly null pointer '...' (lifetime.1).
#pragma warning(disable: 26487) // Don't return a pointer '...' that may be invalid (lifetime.4).
#pragma warning(disable: 26812) // The enum type '...' is unscoped. Prefer '...' over '...' (Enum.3).
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26458) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26482) // Only index into arrays using constant expressions (bounds.2).
#pragma warning(disable: 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).

// Yes, these are our files ... but they don't change very often, and if they do
// change we want to rebuild everything anyway.
#include "gsl/gsl.h"
#include "config/Exports.h"
#include "mem/SharedPtr.h"
#include "sys/filesystem.h"
#include "except/Throwable.h"
#include "sys/Conf.h"

#include "xml/lite/xerces_.h"
#pragma comment(lib, "xerces-c")

#include "hdf5/lite/H5_.h"
#pragma comment(lib, "hdf5-c++.lib")

