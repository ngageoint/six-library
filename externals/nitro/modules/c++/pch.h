#pragma once

#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#pragma warning(disable: 4710) // '...': function not inlined
#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if / Qspectre switch specified
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
// TODO: get rid of these someday?
#pragma warning(disable: 4514) //	'...': unreferenced inline function has been removed
#pragma warning(disable: 26823) // Dereferencing a possibly null pointer '...' (lifetime.1).

#include "cpp.h"
#pragma comment(lib, "ws2_32")

#pragma warning(disable: 26822) // Dereferencing a null pointer '...' (lifetime.1).
#pragma warning(disable: 26467) // Converting from floating point to unsigned integral types results in non-portable code if the double/float has a negative value. Use gsl::narrow_cast or gsl::narrow instead to guard against undefined behavior and potential data loss(es.46).

#include "nitf/coda-oss.hpp"

#pragma comment(lib, "io-c++")
#pragma comment(lib, "except-c++")
#pragma comment(lib, "sys-c++")
#pragma comment(lib, "str-c++")

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1

#include "import/nitf.h"
#include "nitf/ImageIO.h"
#include "nitf/System.h"
#include "nitf/Field.h"
#include "nitf/Types.h"
#pragma comment(lib, "nitf-c")

// changing this breaks SWIG
#pragma warning (disable: 26812) // The enum type '...' is unscoped. Prefer '...' over '...'

#pragma warning(disable: 5039) //	'...': pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc. Undefined behavior may occur if this function throws an exception.
#pragma warning(disable: 26456) // Operator '...' hides a non-virtual operator '...' (c.128).
#pragma warning(disable: 26432) // If you define or delete any default operation in the type '...', define or delete them all(c.21).
#pragma warning(disable: 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
#pragma warning(disable: 26434) // Function '...' hides a non-virtual function '...' (c.128).
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4)
#pragma warning(disable: 26458) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26486) // Don't pass a pointer that may be invalid to a function. Parameter '...' in call to '...' may be invalid (lifetime.3).
#pragma warning(disable: 26487) // Don't return a pointer '...' that may be invalid(lifetime.4).

#include "nitf/Object.hpp"
