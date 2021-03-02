/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __SYS_ATOMIC_COUNTER_WIN32_H__
#define __SYS_ATOMIC_COUNTER_WIN32_H__

// In order to include windows.h with the appropriate stuff defined beforehand
#include <sys/Conf.h>

namespace sys
{
// Implemented from boost/smart_ptr/detail/atomic_count_win32.hpp
struct AtomicCounterImpl
{
    typedef long ValueType;

    explicit
    AtomicCounterImpl(ValueType initialValue) :
        mValue(initialValue)
    {
    }

    ValueType getThenIncrement()
    {
        return (InterlockedIncrement(&mValue) - 1);
    }

    ValueType getThenDecrement()
    {
        return (InterlockedDecrement(&mValue) + 1);
    }

    ValueType get() const
    {
        return static_cast<const volatile long&>(mValue);
    }

    AtomicCounterImpl(const AtomicCounterImpl&) = delete;
    AtomicCounterImpl& operator=(const AtomicCounterImpl&) = delete;

private:
    ValueType mValue;
};
}

#endif
