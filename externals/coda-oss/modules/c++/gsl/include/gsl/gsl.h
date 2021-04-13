/* =========================================================================
 * This file is part of gsl-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#ifndef CODA_OSS_gsl_gsl_h_INCLUDED_
#define CODA_OSS_gsl_gsl_h_INCLUDED_
#pragma once

// Need a fairly decent C++ compiler to use the real GSL
#include "gsl/use_gsl.h"

// always compile Gsl (not "gsl") code--our own simple implementation
#include "gsl/Gsl_.h"  // our own "fake" GSL

#if CODA_OSS_use_real_gsl
	#if _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4626) // '...' : assignment operator was implicitly defined as deleted
	#pragma warning(disable: 5027) // '...' : move assignment operator was implicitly defined as deleted
	#endif

	#include "gsl/gsl"

	#if _MSC_VER
	#pragma warning(pop)
	#endif

#endif

#endif  // CODA_OSS_gsl_gsl_h_INCLUDED_
