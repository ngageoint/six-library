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
#ifndef CODA_OSS_sys_Optional_h_INCLUDED_
#define CODA_OSS_sys_Optional_h_INCLUDED_
#pragma once

#include <optional>
namespace sys
{
    template<typename T>
    using Optional = std::optional<T>;

    // https://en.cppreference.com/w/cpp/utility/optional/make_optional
    template <typename T, typename... TArgs>
    inline Optional<T> make_Optional(TArgs&&... args)
    {
        return std::make_optional<T>(std::forward<TArgs>(args)...);
    }
}

#include "str/Convert.h"
namespace str
{
    template <typename T>
    std::string toString(const sys::Optional<T>& value)
    {
        return toString(value.value());
    }
}

#endif  // CODA_OSS_sys_Optional_h_INCLUDED_
