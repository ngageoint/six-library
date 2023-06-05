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

#ifndef CODA_OSS_sys_MutexCpp11_h_INCLUDED_
#define CODA_OSS_sys_MutexCpp11_h_INCLUDED_

#include <mutex>

#include "sys/MutexInterface.h"

namespace sys
{
/*!
 *  \class MutexPosix
 *  \brief The pthreads implementation of a mutex
 *
 *  Implements a pthread mutex and wraps the outcome
 *
 */
struct MutexCpp11 final : public MutexInterface
{
    //!  Constructor
    MutexCpp11();

    //!  Destructor
    virtual ~MutexCpp11();

    MutexCpp11(const MutexCpp11&) = delete;
    MutexCpp11& operator=(const MutexCpp11&) = delete;

    /*!
     *  Lock the mutex.
     */
    virtual void lock() override;

    /*!
     *  Unlock the mutex.
     */
    virtual void unlock() override;

    /*!
     *  Returns the native type.
     */
    std::mutex& getNative();

    /*!
     *  Return the type name.  This function is essentially free,
     *  because it is static RTTI.
     */
    const char* getNativeType() const noexcept
    {
        return typeid(mNative).name();
    }

private:
    std::mutex mNative;
};
}

#endif // CODA_OSS_sys_MutexCpp11_h_INCLUDED_
