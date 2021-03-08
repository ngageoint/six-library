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

#ifndef __SYS_ATOMIC_COUNTER_X86_H__
#define __SYS_ATOMIC_COUNTER_X86_H__

namespace sys
{
// Implemented from boost/smart_ptr/detail/atomic_count_gcc_x86.hpp
struct AtomicCounterImpl
{
    typedef int ValueType;

    explicit
    AtomicCounterImpl(ValueType initialValue) :
        mValue(initialValue)
    {
    }

    ValueType getThenIncrement()
    {
        return atomicExchangeAndAdd(&mValue, 1);
    }

    ValueType getThenDecrement()
    {
        return atomicExchangeAndAdd(&mValue, -1);
    }

    ValueType get() const
    {
        return atomicExchangeAndAdd(&mValue, 0);
    }

    AtomicCounterImpl(const AtomicCounterImpl&) = delete;
    AtomicCounterImpl& operator=(const AtomicCounterImpl&) = delete;

private:
    static
    ValueType atomicExchangeAndAdd(ValueType* pw, ValueType dv)
    {
        // int r = *pw;
        // *pw += dv;
        // return r;

        int r;

        __asm__ __volatile__
        (
            "lock\n\t"
            "xadd %1, %0":
            "+m"( *pw ), "=r"( r ): // outputs (%0, %1)
            "1"( dv ): // inputs (%2 == %1)
            "memory", "cc" // clobbers
        );

        return r;
    }

private:
    mutable ValueType mValue;
};
}

#endif
