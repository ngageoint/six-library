#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// We're building in Visual Studio ... used to control where we get a little bit of config info
#define NITRO_PCH 1

#pragma warning(disable: 4619) // #pragma warning: there is no warning number '...'
#pragma warning(disable: 4668) // '...' is not defined as a preprocessor macro, replacing with '...' for '...'
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#pragma warning(disable: 4710) // '...': function not inlined
#pragma warning(disable: 5264) // '...': '...' variable is not used

#pragma warning(disable: 5045) // Compiler will insert Spectre mitigation for memory load if / Qspectre switch specified

#pragma warning(disable: 4625) // '...': copy constructor was implicitly defined as deleted
#pragma warning(disable: 4626) // '...': assignment operator was implicitly defined as deleted
#pragma warning(disable: 5026) // '...': move constructor was implicitly defined as deleted
#pragma warning(disable: 5027) //	'...': move assignment operator was implicitly defined as deleted

// TODO: get rid of these someday?
#pragma warning(disable: 4774) // '...' : format string expected in argument 3 is not a string literal
#pragma warning(disable: 4267) // '...': conversion from '...' to '...', possible loss of data
#pragma warning(disable: 4244) // 	'...': conversion from '...' to '...', possible loss of data
#pragma warning(disable: 4242) // '...': conversion from '...' to '...', possible loss of data
#pragma warning(disable: 4018) // '...': signed / unsigned mismatch
#pragma warning(disable: 4389) // '...': signed / unsigned mismatch
#pragma warning(disable: 4365) // '...': conversion from '...' to '...', signed / unsigned mismatch
#pragma warning(disable: 5219) // implicit conversion from '...' to '...', possible loss of data
#pragma warning(disable: 4514) //	'...': unreferenced inline function has been removed
#pragma warning(disable: 4127) // conditional expression is constant
#pragma warning(disable: 5039) // '...' : pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc. Undefined behavior may occur if this function throws an exception.

// TODO: get rid of these someday? ... from Visual Studio code-analysis
#pragma warning(disable: 26451) // Arithmetic overflow : Using operator '...' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '*' to avoid overflow(io.2).
#pragma warning(disable: 6385) // Reading invalid data from '...':  the readable size is '...' bytes, but '...' bytes may be read.
#pragma warning(disable: 6386) // Buffer overrun while writing to '...':  the writable size is '...' bytes, but '...' bytes might be written.
#pragma warning(disable: 26812) // The enum type '...' is unscoped. Prefer '...' over '...' (Enum.3).

// error 4996: '...': warning STL4037: The effect of instantiating the template std::complex for any type other than float, double, or long double is unspecified. You can define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING to suppress this warning.
#define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
#include <complex>

#pragma warning(push)
#pragma warning(disable: 4464) // relative include path contains '..'
#include "../../nitro_pch.h"
#pragma warning(pop)

#include <import/nitf.hpp>
#include <import/nitf.h>
#include <import/nrt.h>

#include <import/six.h>
#include <import/six/sicd.h>
