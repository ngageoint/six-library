/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2022, Maxar Technologies, Inc.
 *
 * coda_oss-c++ is free software; you can redistribute it and/or modify
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
#ifndef CODA_OSS_coda_oss_type_traits_h_INCLUDED_
#define CODA_OSS_coda_oss_type_traits_h_INCLUDED_
#pragma once

#include <type_traits>

#include "CPlusPlus.h"

#include "coda_oss/namespace_.h"
namespace coda_oss
{
// workaround missing "is_trivially_copyable" in g++ < 5.0
// https://stackoverflow.com/a/31798726/8877
#if defined(__GNUC__) && (__GNUC__ < 5)
template <typename T>
struct is_trivially_copyable final
{
    static_assert(CODA_OSS_cplusplus < 201402L, "C++14 must have is_trivially_copyable.");
    static constexpr bool value = __has_trivial_copy(T);
};
#else
using std::is_trivially_copyable;
#endif
}

#endif  // CODA_OSS_coda_oss_type_traits_h_INCLUDED_
