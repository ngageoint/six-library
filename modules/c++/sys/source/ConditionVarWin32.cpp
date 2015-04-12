/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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


#if defined(WIN32) && defined(_REENTRANT)

#if !defined(USE_NSPR_THREADS) && !defined(__POSIX)

#include "sys/ConditionVarWin32.h"

namespace
{
    // RAII for EnterCriticalSection() / LeaveCriticalSection()
    class ScopedCriticalSection
    {
    public:
        ScopedCriticalSection(CRITICAL_SECTION& criticalSection) :
            mCriticalSection(criticalSection)
        {
            EnterCriticalSection(&mCriticalSection);
        }

        ~ScopedCriticalSection()
        {
            LeaveCriticalSection(&mCriticalSection);
        }

    private:
        CRITICAL_SECTION& mCriticalSection;
    };
}

sys::ConditionVarDataWin32::ConditionVarDataWin32():
    mNumWaiters(0),
    mSemaphore(CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL)),
    mWaitersAreDone(CreateEvent(NULL, FALSE, FALSE, NULL)),
    mWasBroadcast(false)
{
    InitializeCriticalSection(&mNumWaitersCS);
    if (mSemaphore == NULL || mWaitersAreDone == NULL)
    {
        throw sys::SystemException(
            "ConditionVarDataWin32 Initializer failed");
    }
}

sys::ConditionVarDataWin32::~ConditionVarDataWin32()
{
    CloseHandle(mWaitersAreDone);
	CloseHandle(mSemaphore);
    DeleteCriticalSection(&mNumWaitersCS);
}

void sys::ConditionVarDataWin32::wait(HANDLE externalMutex)
{
    // Increment # waiting
    {
        const ScopedCriticalSection lock(mNumWaitersCS);
        ++mNumWaiters;
    }

    // Atomically release the mutex and wait on the semaphore until signal()
    // or broadcast() are called by another thread
    if (SignalObjectAndWait(externalMutex, mSemaphore, INFINITE, FALSE) != 
        WAIT_OBJECT_0)
    {
        throw sys::SystemException("SignalObjectAndWait() failed");
    }

    waitImpl(externalMutex);
}

bool sys::ConditionVarDataWin32::wait(HANDLE externalMutex, double timeout)
{
    if (timeout == 0)
    {
        wait(externalMutex);
        return true;
    }

    // Increment # waiting
    {
        const ScopedCriticalSection lock(mNumWaitersCS);
        ++mNumWaiters;
    }

    // Atomically release the mutex and wait on the semaphore until signal()
    // or broadcast() are called by another thread or we time out
    switch (SignalObjectAndWait(externalMutex,
                                mSemaphore, 
                                static_cast<DWORD>(timeout * 1000), 
                                FALSE))
    {
    case WAIT_OBJECT_0:
        waitImpl(externalMutex);
        return true;
    case WAIT_TIMEOUT:
        return false;
    default:
        throw sys::SystemException("SignalObjectAndWait() failed");
    }
}

void sys::ConditionVarDataWin32::waitImpl(HANDLE externalMutex)
{
    // Mark that we're no longer waiting
    // If we woke up via broadcast(), determine if we're the last waiter
    bool lastWaiter;
    {
        const ScopedCriticalSection lock(mNumWaitersCS);
        --mNumWaiters;
        lastWaiter = (mWasBroadcast && mNumWaiters == 0);
    }

    if (lastWaiter)
    {
        // Atomically signals the mWaitersAreDone event and waits until it can
        // acquire the external mutex.  This is used to ensure fairness.
        /// @note  Fairness relies on the fact that Windows NT mutex requests
        ///        are queued in FIFO order.  As a result, all waiting threads
        ///        will acquire the external mutex before any of them can
        ///        reacquire it a second time.
        ///        Need the atomicity of SignalObjectAndWait() here to ensure
        ///        that the last thread gets his chance to wait on the
        ///        external mutex.
        SignalObjectAndWait(mWaitersAreDone, externalMutex, INFINITE, FALSE);
    }
    else
    {
        // We need to wait until we get the external mutex back
        WaitForSingleObject(externalMutex, INFINITE);
    }
}

void sys::ConditionVarDataWin32::signal()
{
    bool haveWaiters;
    {
        const ScopedCriticalSection lock(mNumWaitersCS);
        haveWaiters = mNumWaiters > 0;
    }

    // If there are waiters, increment the semaphore by 1 to wake one up
    if (haveWaiters)
    {
        ReleaseSemaphore(mSemaphore, 1, NULL);
    }
}

void sys::ConditionVarDataWin32::broadcast()
{
    bool haveWaiters;
    {
        const ScopedCriticalSection lock(mNumWaitersCS);

        // Increment the semaphore by the # of threads waiting
        // This will allow all those threads to wake up in wait()
        if (mNumWaiters > 0)
        {
            mWasBroadcast = true;
            haveWaiters = true;
            ReleaseSemaphore(mSemaphore, mNumWaiters, 0);
        }
        else
        {
            haveWaiters = false;
        }
    }

    if (haveWaiters)
    {
        // Wait for all the awakened threads to acquire mSemaphore
        WaitForSingleObject(mWaitersAreDone, INFINITE);

        // This assignment is ok w/o locking the critical section
        // since no waiter threads can wake up to access it
        mWasBroadcast = false;
    }
}

bool sys::ConditionVarWin32::wait(double timeout)
{
    dbg_printf("Timed waiting on condition [%f]\n", timeout);
    return mNative.wait(mMutex->getNative(), timeout);
}

bool sys::ConditionVarWin32::wait()
{
    dbg_printf("Waiting on condition\n");
    mNative.wait(mMutex->getNative());
    return true;
}

bool sys::ConditionVarWin32::signal()
{
    dbg_printf("Signalling condition\n");
    mNative.signal();
    return true;
}


bool sys::ConditionVarWin32::broadcast()
{
    dbg_printf("Broadcasting condition\n");
    mNative.broadcast();
    return true;
}

#endif // No other thread package

#endif // Windows

