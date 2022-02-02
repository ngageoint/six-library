/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2020, Maxar Technologies, Inc.
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, http://www.gnu.org/licenses/.
 *
 */
#ifndef CODA_OSS_coda_oss_cstddef_h_INCLUDED_
#define CODA_OSS_coda_oss_cstddef_h_INCLUDED_
#pragma once

#include <cstddef>

// Need a fairly decent C++ compiler to use the real GSL.  This brings in more than 
// we really need for span (e.g., gsl::narrow()), but it keeps things simple.
#include "gsl/gsl.h"  // not gsl/byte; need #pragma here to turn off warnings

namespace coda_oss
{
	#if defined(GSL_BYTE_H) // the above #include'd gsl/byte
		using byte = gsl::byte;
	#else // no gsl::byte, use our own
        // https://en.cppreference.com/w/cpp/types/byte
        enum class byte : unsigned char {};
	#endif  // GSL_BYTE_H
}

#endif  // CODA_OSS_coda_oss_cstddef_h_INCLUDED_
