/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2021, Maxar Technologies, Inc.
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CODA_OSS_sys_CPlusPlus_h_INCLUDED_
#define CODA_OSS_sys_CPlusPlus_h_INCLUDED_

#include "coda_oss/CPlusPlus.h"

// We've got various "replacements" (to a degree) for C++ library functionality
// only available in later releases.  Adding these names to "std" is technically
// forbidden, but it makes for fewer (eventual) changes in client code.
//
// You can bring these into your code by #include'ng the file from coda_oss; e.g.,
//    #include <coda_oss/span.h> // coda_oss::span

#endif // CODA_OSS_sys_CPlusPlus_h_INCLUDED_
