/* =========================================================================
 * This file is part of mt-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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

#include <algorithm>
#include <iterator>
#include <future>

#include "config/compiler_extensions.h"
#include "coda_oss/CPlusPlus.h"
#if CODA_OSS_cpp17
	// <execution> is broken with the older version of GCC we're using
	#if (__GNUC__ >= 10) || _MSC_VER
	#include <execution>
	#define CODA_OSS_mt_Algorithm_has_execution 1
	#endif
#endif

namespace mt
{
// "Roll our own" `std::transform(execution::par)` using std::async()
// https://en.cppreference.com/w/cpp/algorithm/transform

// Our own `Transform_par_()` is built on `std::async()`; for that we need to control
// a couple of settings.
struct Transform_par_settings final
{
    Transform_par_settings() = default;

    Transform_par_settings(ptrdiff_t cutoff) : cutoff_(cutoff) { }
    Transform_par_settings(std::launch policy) : policy_(policy) { }
    Transform_par_settings(ptrdiff_t cutoff, std::launch policy) : cutoff_(cutoff), policy_(policy) { }
    Transform_par_settings(std::launch policy, ptrdiff_t cutoff) : Transform_par_settings(cutoff, policy) { }

    // The value of "default_cutoff" was determined by testing; there is nothing
    // special about it, feel free to change it.
    static constexpr ptrdiff_t dimension = 128 * 8;
    static constexpr ptrdiff_t default_cutoff = dimension * dimension;
    ptrdiff_t cutoff_ =  default_cutoff;

    // https://en.cppreference.com/w/cpp/thread/launch
    std::launch policy_ = std::launch::async; // "the task is executed on a different thread, potentially by creating and launching it first"
};

template <typename InputIt, typename OutputIt, typename UnaryOperation>
inline OutputIt Transform_par_(InputIt first1, InputIt last1, OutputIt d_first, UnaryOperation unary_op,
    const Transform_par_settings& settings)
{
    // https://en.cppreference.com/w/cpp/thread/async
    const auto len = std::distance(first1, last1);
    if (len < settings.cutoff_)
    {
        return std::transform(first1, last1, d_first, unary_op);
    }
    
    const auto mid1 = first1 + len / 2;
    const auto d_mid = d_first + len / 2;
    auto handle = std::async(settings.policy_, Transform_par_<InputIt, OutputIt, UnaryOperation>, mid1, last1, d_mid, unary_op, settings);
    Transform_par_(first1, mid1, d_first, unary_op, settings);
    return handle.get();
}
template <typename InputIt, typename OutputIt, typename UnaryOperation>
inline OutputIt Transform_par(InputIt first1, InputIt last1, OutputIt d_first, UnaryOperation unary_op,
    Transform_par_settings settings = Transform_par_settings{})
{
#if CODA_OSS_mt_Algorithm_has_execution
    CODA_OSS_mark_symbol_unused(settings);
    return std::transform(std::execution::par, first1, last1, d_first, unary_op);
#else
    return Transform_par_(first1, last1, d_first, unary_op, settings);
#endif // CODA_OSS_mt_Algorithm_has_execution
}

}
