/* =========================================================================
 * This file is part of gsl-c++
 * =========================================================================
 *
 * (C) Copyright 2021, Maxar Technologies, Inc.
 *
 * gsl-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef CODA_OSS_gsl_use_gsl_h_INCLUDED_
#define CODA_OSS_gsl_use_gsl_h_INCLUDED_
#pragma once

// Need a fairly decent C++ compiler to use the real GSL
// It seems that the WAF build on GitHub gets a old compiler ...? Using just CODA_OSS_cpp14 doesn't work.
#ifndef CODA_OSS_coda_oss_use_real_gsl_
	#if defined(_MSC_VER) && (_MSC_VER < 1910) // VS2017: https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-160
		#define CODA_OSS_coda_oss_use_real_gsl_ 0
	#else
		#include "coda_oss/CPlusPlus.h"
		#define CODA_OSS_coda_oss_use_real_gsl_ CODA_OSS_cpp14
	#endif
#endif

#endif  // CODA_OSS_gsl_use_gsl_h_INCLUDED_
