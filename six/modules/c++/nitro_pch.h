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
#pragma warning(disable: 26458) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26823) // Dereferencing a possibly null pointer '...' (lifetime.1).
#pragma warning(disable: 26822) // Dereferencing a null pointer 'me' (lifetime.1).

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
#pragma warning(disable: 26823) // Dereferencing a possibly null pointer '' (lifetime.1).
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
#pragma comment(lib, "ws2_32")

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
#pragma warning(disable: 26823) // Dereferencing a possibly null pointer '' (lifetime.1).

#include <import/nitf.hpp>
#include <import/nitf.h>
#include <import/nrt.h>

#pragma warning(pop)
#pragma warning(pop)
