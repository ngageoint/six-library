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


#if defined(__sun)
#include <thread.h>
#include <synch.h>
#include "sys/ConditionVarSolaris.h"

sys::ConditionVarSolaris::ConditionVarSolaris() :
    mMutexOwned(new sys::MutexSolaris()),
    mMutex(mMutexOwned.get())
{
    if ( ::cond_init(&mNative, NULL, NULL) != 0)
        throw sys::SystemException("ConditionVar initialization failed");
}

sys::ConditionVarSolaris::ConditionVarSolaris(sys::MutexSolaris* theLock, bool isOwner) :
    mMutex(theLock)
{
    if (!theLock)
        throw SystemException("ConditionVar received NULL mutex");

    if (isOwner)
        mMutexOwned.reset(theLock);

    if ( ::cond_init(&mNative, NULL, NULL) != 0)
        throw sys::SystemException("ConditionVar initialization failed");
}

sys::ConditionVarSolaris::~ConditionVarSolaris()
{
    ::cond_destroy(&mNative);
}

void sys::ConditionVarSolaris::acquireLock()
{
    mMutex->lock();
}

void sys::ConditionVarSolaris::dropLock()
{
    mMutex->unlock();
}

void sys::ConditionVarSolaris::signal()
{
    dbg_printf("Signaling condition\n");
    if (::cond_signal(&mNative) != 0)
        throw sys::SystemException("ConditionVar signal failed");
}

void sys::ConditionVarSolaris::wait()
{
    dbg_printf("Waiting on condition\n");
    if (::cond_wait(&mNative, &(mMutex->getNative())) != 0)
        throw sys::SystemException("ConditionVar wait failed");
}

void sys::ConditionVarSolaris::wait(double seconds)
{
    dbg_printf("Timed waiting on condition [%f]\n", seconds);
    if ( seconds > 0 )
    {
        timestruc_t tout;
        tout.tv_sec = time(NULL) + (int)seconds;
        tout.tv_nsec = (int)((seconds - (int)(seconds)) * 1e9);
        if (::cond_timedwait(&mNative,
                             &(mMutex->getNative()),
                             &tout) != 0)
            throw sys::SystemException("ConditionVar wait failed");
    }
    else
        wait();
}

void sys::ConditionVarSolaris::broadcast()
{
    dbg_printf("Broadcasting condition\n");
    if (::cond_broadcast(&mNative) != 0)
        throw sys::SystemException("ConditionVar broadcast failed");
}

cond_t& sys::ConditionVarSolaris::getNative()
{
    return mNative;
}

#endif

