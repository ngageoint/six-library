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


#if defined(__sgi)
#include "sys/ConditionVarIrix.h"

#ifdef __SGI_LIBC_BEGIN_NAMESPACE_STD 
using std::sigsend;
#endif

sys::ConditionVarIrix::ConditionVarIrix() :
    mMutexOwned(new sys::MutexIrix()),
    mMutex(mMutexOwned.get())
{
    dbg_ln("Creating a default condition variable");
}

sys::ConditionVarIrix::ConditionVarIrix(sys::MutexIrix *theLock, bool isOwner) :
    mMutex(theLock)
{
    if (!theLock)
        throw SystemException("ConditionVar received NULL mutex");

    dbg_ln("Creating a cv given a mutex");
    if (isOwner)
        mMutexOwned.reset(theLock);
}

sys::ConditionVarIrix::~ConditionVarIrix()
{
    dbg_printf("BEGIN ~ConditionVarIrix\n");
    broadcast();
    mNative.clear();
    dbg_printf("END ~ConditionVarIrix\n");
}

void sys::ConditionVarIrix::acquireLock()
{
    mMutex->lock();
}

void sys::ConditionVarIrix::dropLock()
{
    mMutex->unlock();
}

void sys::ConditionVarIrix::signal()
{
    dbg_printf("ConditionVarIrix::signal()\n");
    int lStatus = 0;
    if ( mNative.size() > 0 )
    {
        lStatus = sigsend(P_PID, mNative.back(), SIGUSR1);
        mNative.pop_back();
    }
    if (lStatus != 0)
        throw sys::SystemException("Condition Variable signal failed");
}

void sys::ConditionVarIrix::wait()
{
    dbg_printf("ConditionVarIrix::wait()\n");
    sys::ConditionVarIrix::wait(0);
}

void sys::ConditionVarIrix::wait(double timeout)
{
    dbg_printf("Timed waiting on condition [%f]\n", timeout);
    sigset_t lSignalSet;
    siginfo_t lSignalInfo;
    timespec_t *lTimeout = NULL;

    mNative.insert(mNative.begin(), getpid());

    sigemptyset(&lSignalSet);
    sigaddset(&lSignalSet, SIGUSR1);

    if ( timeout > 0 )
    {
        lTimeout = new timespec_t;
        lTimeout->tv_sec = (int)timeout;
        lTimeout->tv_nsec = (int)((timeout - (int)timeout) * 1e9);
    }

    mMutex->unlock();
    sigtimedwait(&lSignalSet, &lSignalInfo, lTimeout);
    mMutex->lock();

    for (int i = mNative.size(); i > 0; i-- )
    {
        if ( mNative.at(i - 1) == getpid() )
        {
            mNative.erase(mNative.begin() + i - 1);
        }
    }

    if (lTimeout != NULL)
        delete lTimeout;

    if (lSignalInfo.si_signo != SIGUSR1)
        throw sys::SystemException("Condition Variable wait failed");
}

void sys::ConditionVarIrix::broadcast()
{
    dbg_printf("ConditionVarIrix::broadcast()\n");
    int lStatus = 0;
    while (mNative.size() > 0 )
    {
        lStatus |= sigsend(P_PID, mNative.back(), SIGUSR1);
        mNative.pop_back();
    }
    if (lStatus != 0)
        throw sys::SystemException("Condition Variable broadcast failed");
}

std::vector<pid_t>& sys::ConditionVarIrix::getNative()
{
    return mNative;
}

#endif

