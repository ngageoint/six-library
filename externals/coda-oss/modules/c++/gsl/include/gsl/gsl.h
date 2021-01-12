#pragma once

#include "sys/Conf.h"

// Need a fairly decent C++ compiler to use the real GSL
#if CODA_OSS_cpp14
#define CODA_OSS_use_gsl_ 1
#endif

#if !CODA_OSS_use_gsl_

#include "gsl_.h" // our own "fake" GSL

#else

#include "gsl_algorithm"	// copy
#include "gsl_assert"		// Ensures/Expects
#include "gsl_byte"			// byte
#include "gsl_util"			// finally()/narrow()/narrow_cast()...
#include "multi_span"		// multi_span, strided_span...
#include "pointers"			// owner, not_null
#include "span"				// span
#include "string_span"		// zstring, string_span, zstring_builder...

#endif
