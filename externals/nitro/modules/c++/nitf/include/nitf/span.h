#pragma once

#if __cplusplus >= 202002L // no C++20, see https://en.cppreference.com/w/cpp/preprocessor/replace
#include <span>
#else

#include <cstdint>

// GSL has gsl::span
#include "gsl/gsl.h"

namespace std
{
	// std::span<> is part of C++20.  Use our own implementation (rather, GSL's) until then.
	template<typename T>
	using span = gsl::span<T>;
}

#endif
