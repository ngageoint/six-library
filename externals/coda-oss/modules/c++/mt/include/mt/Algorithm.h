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

#ifndef CODA_OSS_mt_Algorithm_h_INCLUDED_
#define CODA_OSS_mt_Algorithm_h_INCLUDED_
#pragma once

#include <algorithm>
#include <iterator>
#include <future>

namespace mt
{
namespace details
{
    template <typename InputIt, typename OutputIt, typename TFunc>
    inline OutputIt transform_async(const InputIt first1, const InputIt last1, OutputIt d_first, TFunc f,
        typename std::iterator_traits<InputIt>::difference_type cutoff, std::launch policy)
    {
        // https://en.cppreference.com/w/cpp/thread/async
        const auto len = std::distance(first1, last1);
        if (len < cutoff)
        {
            return std::transform(first1, last1, d_first, f);
        }

        const auto mid1 = first1 + len / 2;
        const auto d_mid = d_first + len / 2;
        auto handle = std::async(policy, transform_async<InputIt, OutputIt, TFunc>, mid1, last1, d_mid, f, cutoff, policy);
        transform_async(first1, mid1, d_first, f, cutoff, policy);
        return handle.get();
    }
}
template <typename InputIt, typename OutputIt, typename TFunc>
inline OutputIt transform_async(const InputIt first1, const InputIt last1, OutputIt d_first, TFunc f,
    typename std::iterator_traits<InputIt>::difference_type cutoff, std::launch policy)
{
    // details::... eliminates the overload
    return details::transform_async(first1, last1, d_first, f, cutoff, policy);
}
template <typename InputIt, typename OutputIt, typename TFunc>
inline OutputIt transform_async(const InputIt first1, const InputIt last1, OutputIt d_first, TFunc f,
    typename std::iterator_traits<InputIt>::difference_type cutoff)
{
    const std::launch policy = std::launch::deferred | std::launch::async;
    return transform_async(first1, last1, d_first, f, cutoff, policy);
}

}
#endif // CODA_OSS_mt_Algorithm_h_INCLUDED_

