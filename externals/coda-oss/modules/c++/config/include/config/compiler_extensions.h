/* =========================================================================
 * This file is part of config-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * config-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_config_compiler_extentions_h_INCLUDED_
#define CODA_OSS_config_compiler_extentions_h_INCLUDED_
#pragma once

#include <config/coda_oss_config.h>

#ifndef CODA_OSS_attribute_noinline_DEFINED_
	#define CODA_OSS_attribute_noinline_DEFINED_ 1

	// https://stackoverflow.com/a/49468469/8877
	#if defined(__GNUC__)
	#define ATTRIBUTE_NOINLINE __attribute__((noinline))
	#elif defined(_MSC_VER)
	#define ATTRIBUTE_NOINLINE __declspec(noinline)
	#else
	#define ATTRIBUTE_NOINLINE
	#endif
#endif // CODA_OSS_attribute_noinline_DEFINED_

#ifndef CODA_OSS_attribute_aligned_DEFINED_
	#define CODA_OSS_attribute_aligned_DEFINED_ 1

	#if defined(__GNUC__)
	// https://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Variable-Attributes.html
	#define ATTRIBUTE_ALIGNED(x) __attribute__((aligned(x)))
	#else
	#define ATTRIBUTE_ALIGNED(X)
	#endif
#endif // CODA_OSS_attribute_aligned_DEFINED_

#endif // CODA_OSS_config_compiler_extentions_h_INCLUDED_
