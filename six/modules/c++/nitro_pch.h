#pragma once

#include "cpp_pch.h"

#include <std/bit>
#include <std/cstddef>
#include <std/filesystem>
#include <std/memory>
#include <std/optional>
#include <std/span>
#include <std/string>

#pragma warning(disable: 4251) // '...' : class '...' needs to have dll-interface to be used by clients of struct '...'
#pragma warning(push)
#pragma warning(disable: 26447) // The function is declared '...' but calls function '..' which may throw exceptions (f.6).
#pragma warning(disable: 26812) // The enum type '...' is unscoped. Prefer '...' over '...' (Enum.3).
#pragma warning(disable: 26455) //	 Default constructor may not throw.Declare it 'noexcept' (f.6).

#include <import/str.h>
#include <import/sys.h>
#pragma warning(push)
#pragma warning(disable: 5039) //	'...': pointer or reference to potentially throwing function passed to 'extern "C"' function under - EHc.Undefined behavior may occur if this function throws an exception.
#include <import/mt.h>
#pragma warning(pop)
#include <import/io.h>
#include <import/logging.h>

#pragma warning(push)
#pragma warning(disable: 26489) // Don't dereference a pointer that may be invalid: '...'. '...' may have been invalidated at line ... (lifetime.1).

#pragma warning(push)
#pragma warning(disable: 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
#pragma warning(disable: 26459) // You called an STL function '...' with a raw pointer parameter at position '...' that may be unsafe-this relies on the caller to check that the passed values are correct. Consider wrapping your range in a gsl::span and pass as a span iterator(stl.1).
#pragma warning(disable: 26485) // Expression '...': No array to pointer decay (bounds.3).
#include <import/math/linear.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 26487) // Don't return a pointer '...' that may be invalid (lifetime.4).
#pragma warning(disable: 26482) // Only index into arrays using constant expressions (bounds.2).
#include <import/math/poly.h>
#pragma warning(pop)
#pragma warning(pop)

#include <polygon/PolygonMask.h>
#include <math/Utilities.h>

#include <nitf/coda-oss.hpp>
#pragma comment(lib, "io-c++")
#pragma comment(lib, "except-c++")
#pragma comment(lib, "sys-c++")
#pragma comment(lib, "str-c++")
#pragma comment(lib, "logging-c++")
#pragma comment(lib, "xml.lite-c++.lib")
#pragma comment(lib, "xerces-c")
#pragma comment(lib, "ws2_32")

// these are from Xerces
#pragma warning(disable: 5219) // implicit conversion from '...' to '...', possible loss of data
#pragma warning(disable: 4365) // '...': conversion from '...' to '...', signed / unsigned mismatch
#pragma warning(disable: 26477) // Use 'nullptr' rather than 0 or NULL (es.47).
#pragma warning(disable: 26493) // Don't use C-style casts (type.4).
#pragma warning(disable: 26814) // The const variable '...' can be computed at compile-time. Consider using constexpr (con.5).
#pragma warning(disable: 26496) // The variable '...' does not change after construction, mark it as const (con.4).
#pragma warning(disable: 26497) // The function '...' could be marked constexpr if compile-time evaluation is desired (f.4).
#pragma warning(disable: 26475) // Do not use function style casts (es.49). Prefer '...' over '...'.
#pragma warning(disable: 26495) // Variable '...' is uninitialized. Always initialize a member variable (type.6).
#pragma warning(disable: 26461) // The pointer argument '...' for function '...' can be marked as a pointer to const (con.3).
#pragma warning(disable: 26462) // The value pointed to by '...' is assigned only once, mark it as a pointer to const (con.4).
#pragma warning(disable: 26494) // Variable '...' is uninitialized. Always initialize an object (type.5).
#pragma warning(disable: 26489) // Don't dereference a pointer that may be invalid: '...'. '...' may have been invalidated at line ... (lifetime.1).
#pragma warning(disable: 26488) // Do not dereference a potentially null pointer : ...'. '...' was null at line ... (lifetime.1).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '..' which may throw exceptions (f.6).
#pragma warning(disable: 26485) // Expression '...': No array to pointer decay (bounds.3).
#pragma warning(disable: 26457) // (void) should not be used to ignore return values, use '...' instead (es.48).
#pragma warning(disable: 26487) // Don't return a pointer '...' that may be invalid (lifetime.4).
#pragma warning(disable: 26451) // Arithmetic overflow : Using operator '...' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '...' to avoid overflow (io.2).
#pragma warning(disable: 26455) //	 Default constructor may not throw.Declare it 'noexcept' (f.6).
#pragma warning(disable: 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead (r.11).
#pragma warning(disable: 26429) // Symbol '...' is never tested for nullness, it can be marked as not_null(f.23).
#pragma warning(disable: 26400) // Do not assign the result of an allocation or a function call with an owner<T> return value to a raw pointer, use owner<T> instead(i.11).
#include <import/xml/lite.h>

#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 26492) // Don't use const_cast to cast away const or volatile (type.3).
#pragma warning(disable: 26812) // The enum type '...' is unscoped. Prefer '...' over '...' (Enum.3).
#pragma warning(disable: 26455) // Default constructor may not throw. Declare it '...' (f.6).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '...' which may throw exceptions (f.6).
#pragma warning(disable: 26440) // Function '...' can be declared '...' (f.6).
#pragma warning(disable: 26456) // Operator '...' hides a non-virtual operator '...' (c.128).
#pragma warning(disable: 26432) // If you define or delete any default operation in the type '...', define or delete them all(c.21).
#pragma warning(disable: 26434) // Function '...' hides a non-virtual function '...' (c.128).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '...' which may throw exceptions(f.6).

#include <import/nitf.hpp>
#include <import/nitf.h>
#include <import/nrt.h>

#pragma warning(pop)
