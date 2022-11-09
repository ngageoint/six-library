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

#ifndef CODA_OSS_sys_AtomicCounter_h_INCLUDED_
#define CODA_OSS_sys_AtomicCounter_h_INCLUDED_
#pragma once

#include <sys/AtomicCounterCpp11.h>

namespace sys
{
/*!
 *  \class AtomicCounter
 *  \brief This class provides atomic incrementing, decrementing, and setting
 *         of an unsigned integer.  All operations are thread-safe.
 */
template<typename TAtomicCounterImpl>
struct AtomicCounterT final
{
    using ValueType = typename TAtomicCounterImpl::ValueType ;

    //! Constructor
    AtomicCounterT(ValueType initialValue = 0) :
        mImpl(initialValue)
    {
    }

    /*!
     *   Increment the value
     *   \return The value PRIOR to incrementing
     */
    ValueType getThenIncrement()
    {
        return mImpl.getThenIncrement();
    }

    ValueType operator++(int )
    {
        return getThenIncrement();
    }

    /*!
     *   Increment the value
     *   \return The value AFTER incrementing
     */
    ValueType incrementThenGet()
    {
        return (getThenIncrement() + 1);
    }

    ValueType operator++()
    {
        return incrementThenGet();
    }

    //! Increment the value
    void increment()
    {
        getThenIncrement();
    }

    /*!
     *   Decrement the value
     *   \return The value PRIOR to decrementing
     */
    ValueType getThenDecrement()
    {
        return mImpl.getThenDecrement();
    }

    ValueType operator--(int )
    {
        return getThenDecrement();
    }

    /*!
     *   Decrement the value
     *   \return The value AFTER decrementing
     */
    ValueType decrementThenGet()
    {
        return (getThenDecrement() - 1);
    }

    ValueType operator--()
    {
        return decrementThenGet();
    }

    //! Decrement the value
    void decrement()
    {
        getThenDecrement();
    }

    /*!
     *   Get the current value
     *   \return The current value
     */
    ValueType get() const
    {
        return mImpl.get();
    }

    AtomicCounterT(const AtomicCounterT&) = delete;
    AtomicCounterT& operator=(const AtomicCounterT&) = delete;

private:
    TAtomicCounterImpl mImpl;
};


using AtomicCounterCpp11 = AtomicCounterT<AtomicCounterImplCpp11>;
using AtomicCounter = AtomicCounterCpp11;
using AtomicCounterOS = AtomicCounterCpp11;

}

#endif  // CODA_OSS_sys_AtomicCounter_h_INCLUDED_
