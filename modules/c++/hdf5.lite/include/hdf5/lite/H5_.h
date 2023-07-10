/* =========================================================================
 * This file is part of hdf5.lite-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * hdf5.lite-c++ is free software; you can redistribute it and/or modify
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

#pragma once
#ifndef CODA_OSS_hdf5_lite_H5__h_INCLUDED_
#define CODA_OSS_hdf5_lite_H5__h_INCLUDED_

#include "config/compiler_extensions.h"

CODA_OSS_disable_warning_system_header_push
#if _MSC_VER
#pragma warning(disable: 26493) // Don't use C-style casts (type.4).
#pragma warning(disable: 26494) // Variable '...' is uninitialized. Always initialize an object (type.5).
#pragma warning(disable: 26451) // Arithmetic overflow: Using operator '...' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator '...' to avoid overflow (io.2).
#pragma warning(disable: 26814) // The const variable '...' can be computed at compile-time. Consider using constexpr (con.5).
#pragma warning(disable: 26447) // The function is declared '...' but calls function '...' which may throw exceptions (f.6).
#pragma warning(disable: 26455) // Default constructor should not throw. Declare it '...' (f.6).
#pragma warning(disable: 26440)  // Function '...' can be declared '...' (f.6).
#pragma warning(disable: 26496) // The variable '...' does not change after construction, mark it as const (con.4).
#pragma warning(disable: 26462) // The value pointed to by '...' is assigned only once, mark it as a pointer to const (con.4).
#pragma warning(disable: 26435) // Function '...' should specify exactly one of '...', '...', or '...' (c.128).
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).
#pragma warning(disable: 26497) // You can attempt to make '...' constexpr unless it contains any undefined behavior (f.4).
#pragma warning(disable: 26433) // Function '...' should be marked with '...' (c.128).
#pragma warning(disable: 26456) // Operator '...' hides a non-virtual operator '...' (c.128).
#pragma warning(disable: 26446) // Prefer to use gsl::at() instead of unchecked subscript operator (bounds.4).
#pragma warning(disable: 26477)  // Use '...' rather than 0 or NULL(es .47).
#pragma warning(disable: 26457) // (void) should not be used to ignore return values, use '...' instead (es.48).
#pragma warning(disable: 26813) // Use '...' to check if a flag is set.
#else
CODA_OSS_disable_warning(-Wshadow)
CODA_OSS_disable_warning(-Wsuggest-override)
CODA_OSS_disable_warning(-Wzero-as-null-pointer-constant)
#endif

// see https://docs.hdfgroup.org/archive/support/HDF5/doc1.8/cpplus_RM/readdata_8cpp-example.html
#include <H5Cpp.h>

#include <highfive/H5Easy.hpp>
#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataType.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5Attribute.hpp>
#include <highfive/H5Exception.hpp>
#include <highfive/H5Reference.hpp>
#include <highfive/H5PropertyList.hpp>

CODA_OSS_disable_warning_pop

#endif // CODA_OSS_hdf5_lite_H5__h_INCLUDED_
