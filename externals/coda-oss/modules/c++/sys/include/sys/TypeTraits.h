/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2022, Maxar Technologies, Inc.
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
#ifndef CODA_OSS_sys_TypeTraits_h_INCLUDED_
#define CODA_OSS_sys_TypeTraits_h_INCLUDED_
#pragma once

#include <type_traits>

#include "CPlusPlus.h"

namespace sys
{
template <class T>
// workaround missing "is_trivially_copyable" in g++ < 5.0
#if defined(__GNUC__) && (__GNUC__ < 5)
    static_assert(CODA_OSS_cplusplus < 201402L, "C++14 must have is_trivially_copyable.");
    struct IsTriviallyCopyable final
    {
        // https://stackoverflow.com/a/31798726/8877
        static constexpr bool value = __has_trivial_copy(T);
    };
#else
    template <typename T>
    using IsTriviallyCopyable = std::is_trivially_copyable<T>;
#endif
}

#endif  // CODA_OSS_sys_TypeTraits_h_INCLUDED_
