/* =========================================================================
 * This file is part of coda_oss-c++
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
#ifndef CODA_OSS_coda_oss_optional_h_INCLUDED_
#define CODA_OSS_coda_oss_optional_h_INCLUDED_
#pragma once

#include <assert.h>

#include <utility>
#include <stdexcept>

#include "coda_oss/namespace_.h"
#include "coda_oss/optional_.h"

namespace coda_oss
{
template<typename T>
using optional = details::optional<T>;
    
// https://en.cppreference.com/w/cpp/utility/optional/make_optional
template <typename T, typename... TArgs>
inline optional<T> make_optional(TArgs&&... args)
{
    return details::make_optional<T>(std::forward<TArgs>(args)...);
}
}

#define CODA_OSS_coda_oss_optional 201606L // c.f., __cpp_lib_optional

#endif  // CODA_OSS_coda_oss_optional_h_INCLUDED_
