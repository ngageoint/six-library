/* =========================================================================
 * This file is part of coda_oss-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CODA_OSS_coda_oss_memory_h_INCLUDED_
#define CODA_OSS_coda_oss_memory_h_INCLUDED_
#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include "config/compiler_extensions.h"

#include "coda_oss/namespace_.h"
namespace coda_oss
{
// C++11 inadvertently ommitted make_unique; provide it here. (Swiped from <memory>.)
template <typename T, typename... TArgs, typename std::enable_if<!std::is_array<T>::value, int>::type = 0>
std::unique_ptr<T> make_unique(TArgs&&... args)
{
    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable: 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead (r .11).
    #endif
    return std::unique_ptr<T>(new T(std::forward<TArgs>(args)...));
    CODA_OSS_disable_warning_pop
}

template <typename T, typename std::enable_if<std::is_array<T>::value &&  std::extent<T>::value == 0, int>::type = 0>
std::unique_ptr<T> make_unique(size_t size)
{
    using element_t = typename std::remove_extent<T>::type;

    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable: 26409) // Avoid calling new and delete explicitly, use std::make_unique<T> instead (r .11).
    #endif    
    return std::unique_ptr<T>(new element_t[size]());
    CODA_OSS_disable_warning_pop
}

template <typename T, typename... TArgs, typename std::enable_if<std::extent<T>::value != 0, int>::type = 0>
void make_unique(TArgs&&...) = delete;

#define CODA_OSS_coda_oss_make_unique 201304L  // c.f., __cpp_lib_make_unique

} // namespace coda_oss

#endif // CODA_OSS_coda_oss_memory_h_INCLUDED_
