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

#ifndef CODA_OSS_sys_AtomicCounterCpp11_h_INCLUDED_
#define CODA_OSS_sys_AtomicCounterCpp11_h_INCLUDED_
#pragma once

#include <stddef.h>

#include <atomic>  // C++11: https://en.cppreference.com/w/cpp/atomic/atomic

namespace sys
{
struct AtomicCounterImplCpp11 final
{
    using ValueType = size_t ;

    explicit AtomicCounterImplCpp11(ValueType initialValue) :
        mValue(initialValue)
    {
    }

    ValueType getThenIncrement()
    {
        // https://en.cppreference.com/w/cpp/atomic/atomic/fetch_add
        return mValue.fetch_add(1);
    }

    ValueType getThenDecrement()
    {
        // https://en.cppreference.com/w/cpp/atomic/atomic/fetch_sub
        return mValue.fetch_sub(1);
    }

    ValueType get() const
    {
        // https://en.cppreference.com/w/cpp/atomic/atomic/load
        return mValue.load();
    }

    AtomicCounterImplCpp11(const AtomicCounterImplCpp11&) = delete;
    AtomicCounterImplCpp11& operator=(const AtomicCounterImplCpp11&) = delete;

private:
    std::atomic<ValueType> mValue;
};
}

#endif  // CODA_OSS_sys_AtomicCounterCpp11_h_INCLUDED_
