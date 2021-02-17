#pragma once

#include "cpp_pch.h"

#pragma warning(push)
#pragma warning(disable: 5039) //	'...': pointer or reference to potentially throwing function passed to 'extern "C"' function under - EHc.Undefined behavior may occur if this function throws an exception.
#pragma warning(disable: 26493) // Don't use C-style casts (type.4).
#pragma warning(disable: 26473) // Don't cast between pointer types where the source type and the target type are the same (type.1).
#pragma warning(disable: 26495) // Variable '...' is uninitialized.Always initialize a member variable(type.6).
#pragma warning(disable: 26812) // The enum type '...' is unscoped. Prefer '...' over '...' (Enum.3).
#pragma warning(disable: 26495) // Variable '...' is uninitialized. Always initialize a member variable(type.6).
#include <nitf/coda-oss.hpp>
#include <import/sys.h>
#include <import/io.h>
#include <import/mt.h>
#pragma warning(pop)
#include <import/str.h>
#include <import/logging.h>
#include <math/Utilities.h>
#include <xml/lite/Element.h>
#include <xml/lite/Validator.h>

#pragma warning(push)
#pragma warning(disable: 26495) // Variable '...' is uninitialized. Always initialize a member variable(type.6).
#pragma warning(disable: 26492) // Don't use const_cast to cast away const or volatile (type.3).
#pragma warning(disable: 26812) // The enum type '...' is unscoped. Prefer '...' over '...' (Enum.3).
#pragma warning(disable: 26455) // Default constructor may not throw. Declare it '...' (f.6).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '...' which may throw exceptions (f.6).
#pragma warning(disable: 26490) // Don't use reinterpret_cast (type.1).
#pragma warning(disable: 26440) // Function '...' can be declared '...' (f.6).
#pragma warning(disable: 26492) // Don't use const_cast to cast away const or volatile (type.3).

#include <import/nitf.hpp>
#include <import/nitf.h>
#include <import/nrt.h>

#pragma warning(pop)

#pragma comment(lib, "io-c++")
#pragma comment(lib, "except-c++")
#pragma comment(lib, "sys-c++")
#pragma comment(lib, "str-c++")

#pragma comment(lib, "ws2_32")