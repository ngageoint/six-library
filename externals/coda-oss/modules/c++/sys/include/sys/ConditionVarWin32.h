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


#ifndef __SYS_WIN32_CONDITION_VARIABLE_H__
#define __SYS_WIN32_CONDITION_VARIABLE_H__

#if defined(WIN32) || defined(_WIN32)
#if !defined(USE_NSPR_THREADS)

#include "sys/ConditionVarInterface.h"
#include "sys/MutexWin32.h"


namespace sys
{
/// @note  This implementation is based on section 3.4 of the
///        "Strategies for Implementing POSIX Condition Variables on Win32"
///        article at www.cse.wustl.edu/~schmidt/win32-cv-1.html.
///        This is the ACE framework implementation.
class ConditionVarDataWin32
{
public:
    ConditionVarDataWin32();

    ~ConditionVarDataWin32();

    void wait(HANDLE externalMutex);

    bool wait(HANDLE externalMutex, double timeout);

    void signal();

    void broadcast();

private:
    void waitImpl(HANDLE externalMutex);

private:
    // # of waiting threads
    size_t           mNumWaiters;
    CRITICAL_SECTION mNumWaitersCS;

    // Semaphore used to queue up threads waiting for the condition to
    // become signaled
    HANDLE mSemaphore;

    // Auto-reset event used by the broadcast/signal thread to wait for
    // all the waiting thread(s) to wake up and be released from the
    // semaphore
    HANDLE mWaitersAreDone;

    // Keep track of whether we were broadcasting or signaling.  This
    // allows us to optimize the code if we're just signaling.
    bool mWasBroadcast;
};

class ConditionVarWin32 : public ConditionVarInterface

{
public:

    ConditionVarWin32();
    
    ConditionVarWin32(MutexWin32 *theLock, bool isOwner = false);
    
    virtual ~ConditionVarWin32()
    {}
    
    /*!
     *  Acquire the lock
     */
    virtual void acquireLock();

    /*!
     *  Drop (release) the lock
     */
    virtual void dropLock();

    /*!
     *  Signal using pthread_cond_signal
     */
    virtual void signal();

    /*!
     *  Wait using pthread_cond_wait
     *
     *  WARNING: The user is responsible for locking the mutex prior 
     *           to using this method. There will be no check and on 
     *           certain systems, undefined/unfavorable behavior may 
     *           result.
     */
    virtual void wait();

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
    virtual void wait(double seconds);

    /*!
     *  Broadcast (notify all)
     */
    virtual void broadcast();
    
    /*!
     *  Returns the native type.
     */
    ConditionVarDataWin32& getNative();
    
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
    std::unique_ptr<MutexWin32> mMutexOwned;
    MutexWin32 *mMutex;
    ConditionVarDataWin32 mNative;
};
}
#endif

#endif

#endif
