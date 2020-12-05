#pragma once

// Need a fairly decent C++ compiler to use the real GSL
#if (__cplusplus >= 201402L /*C++14*/)
#define NITRO_USE_gsl_ 1
#elif defined(_MSC_VER) && (_MSC_VER >= 1920 /* Visual Studio 2019 */)
// need /Zc:__cplusplus to get the right version for __cplusplus
#define NITRO_USE_gsl_ 1
#endif

#if !NITRO_USE_gsl_

#include "gsl_.h"

#else

#include "GSL-3.1.0/include/gsl/gsl_algorithm"	// copy
#include "GSL-3.1.0/include/gsl/gsl_assert"		// Ensures/Expects
#include "GSL-3.1.0/include/gsl/gsl_byte"			// byte
#include "GSL-3.1.0/include/gsl/gsl_util"			// finally()/narrow()/narrow_cast()...
#include "GSL-3.1.0/include/gsl/multi_span"		// multi_span, strided_span...
#include "GSL-3.1.0/include/gsl/pointers"			// owner, not_null
#include "GSL-3.1.0/include/gsl/span"				// span
#include "GSL-3.1.0/include/gsl/string_span"		// zstring, string_span, zstring_builder...

#endif
