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


#ifndef CODA_OSS_sys_ConditionVarPosix_h_INCLUDED_
#define CODA_OSS_sys_ConditionVarPosix_h_INCLUDED_
#pragma once

#include <new>

#include <sys/Conf.h>
#include "sys/ConditionVarInterface.h"

#if CODA_OSS_POSIX_SOURCE

#include "sys/MutexPosix.h"

#include <pthread.h>

namespace sys
{

/*!
 *  \class ConditionVarPosix
 *  \brief Implementation for a pthread condition variable
 *
 *  This class is the wrapper implementation for a pthread_cond_t
 *  (Pthread condition variable)
 */
class ConditionVarPosix final : public ConditionVarInterface
{
    ConditionVarPosix(MutexPosix* theLock, bool isOwner, std::nullptr_t);

public:
    ConditionVarPosix();

    //!  Constructor
    explicit ConditionVarPosix(MutexPosix* theLock, bool isOwner = false);
    explicit ConditionVarPosix(MutexPosix&);  // isOwner = false

    //!  Destructor
    virtual ~ConditionVarPosix();

    ConditionVarPosix(const ConditionVarPosix&) = delete;
    ConditionVarPosix& operator=(const ConditionVarPosix&) = delete;

    /*!
     *  Acquire the lock
     */
    virtual void acquireLock() override;

    /*!
     *  Drop (release) the lock
     */
    virtual void dropLock() override;

    /*!
     *  Signal using pthread_cond_signal
     */
    virtual void signal() override;

    /*!
     *  Wait using pthread_cond_wait
     *
     *  WARNING: The user is responsible for locking the mutex prior
     *           to using this method. There will be no check and on
     *           certain systems, undefined/unfavorable behavior may
     *           result.
     */
    virtual void wait() override;

    /*!
     *  Wait using pthread_cond_timed_wait.  I kept this and the above
     *  function separate only to be explicit.
     *  \param seconds Fraction of a second to wait.
     *  \todo Want a TimeInterval
     *
     *  WARNING: The user is responsible for locking the mutex prior
     *           to using this method. There will be no check and on
     *           certain systems, undefined/unfavorable behavior may
     *           result.
     */
    virtual void wait(double seconds) override;

    /*!
     *  Broadcast (notify all)
     */
    virtual void broadcast() override;

    /*!
     *  Returns the native type.
     */
    pthread_cond_t& getNative();

    /*!
     *  Return the type name.  This function is essentially free,
     *  because it is static RTTI.
     */
    const char* getNativeType() const
    {
        return typeid(mNative).name();
    }

private:
    // This is set if we own the mutex, to make sure it gets deleted.
    std::unique_ptr<MutexPosix> mMutexOwned;
    MutexPosix *mMutex;
    pthread_cond_t mNative;
};
}

#endif
#endif  // CODA_OSS_sys_ConditionVarPosix_h_INCLUDED_
