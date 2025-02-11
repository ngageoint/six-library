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

#ifndef __SYS_ATOMIC_COUNTER_MUTEX_H__
#define __SYS_ATOMIC_COUNTER_MUTEX_H__

#include <sys/Mutex.h>

namespace sys
{
struct AtomicCounterImpl
{
    typedef size_t ValueType;

    explicit
    AtomicCounterImpl(ValueType initialValue) :
        mValue(initialValue)
    {
    }

    ValueType getThenIncrement()
    {
        ValueType value;

        mMutex.lock();
        value = mValue;
        ++mValue;
        mMutex.unlock();

        return value;
    }

    ValueType getThenDecrement()
    {
        ValueType value;

        mMutex.lock();
        value = mValue;
        --mValue;
        mMutex.unlock();

        return value;
    }

    ValueType get() const
    {
        ValueType value;

        mMutex.lock();
        value = mValue;
        mMutex.unlock();

        return value;
    }

    AtomicCounterImpl(const AtomicCounterImpl&) = delete;
    AtomicCounterImpl& operator=(const AtomicCounterImpl&) = delete;

private:
    ValueType mValue;
    mutable Mutex mMutex;
};
}

#endif
