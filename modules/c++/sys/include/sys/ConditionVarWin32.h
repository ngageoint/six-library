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


#ifndef __SYS_WIN32_CONDITION_VARIABLE_H__
#define __SYS_WIN32_CONDITION_VARIABLE_H__

#if defined(WIN32) && defined(_REENTRANT)
#if !defined(USE_NSPR_THREADS) && !defined(__POSIX)

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

class ConditionVarWin32 :
            public ConditionVarInterface < ConditionVarDataWin32,
            MutexWin32 >

{
public:
    typedef ConditionVarInterface < ConditionVarDataWin32,
    MutexWin32 > Parent_T;

    ConditionVarWin32()
    {
        mMutex = new MutexWin32();

    }
    ConditionVarWin32(MutexWin32 *theLock, bool isOwner = false) :
            Parent_T(theLock, isOwner)
    {}
    virtual ~ConditionVarWin32()
    {}
    /*!
     *  Wait for on a signal for a time interval.  
     *  This should eventually have
     *  a class TimeInterval as the second argument, which takes
     *  any time interval as a right-hand-side
     *  \param timeout How long to wait.  This is only temporarily
     *  a double
     *  \todo  Create a TimeInterval class, and use it as parameter
     *  \return true upon success
     */
    bool wait(double timeout);
    bool wait();
    bool signal();
    bool broadcast();

};
}
#endif

#endif

#endif
