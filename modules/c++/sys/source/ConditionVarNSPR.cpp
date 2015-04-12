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


#if defined(USE_NSPR_THREADS)
#include "sys/ConditionVarNSPR.h"

sys::ConditionVarNSPR::ConditionVarNSPR() :
    mMutexOwned(new sys::MutexNSPR()),
    mMutex(mMutexOwned.get())
{
    mNative = PR_NewCondVar( (mMutex->getNative()) );
    if (mNative == NULL)
        throw sys::SystemException("Condition Variable initialization failed");
}

sys::CondtionVarNSPR::ConditionVarNSPR(sys::MutexNSPR *theLock, bool isOwner) :
    mMutex(theLock)
{
    if (!theLock)
        throw SystemException("ConditionVar received NULL mutex");

    if (isOwner)
        mMutexOwned.reset(theLock);

    mNative = PR_NewCondVar( (mMutex->getNative()) );
    if (mNative == NULL)
        throw sys::SystemException("Condition Variable initialization failed");
}

sys::ConditionVarNSPR::~ConditionVarNSPR()
{
    PR_DestroyCondVar(mNative);
}

void sys::ConditionVarNSPR::acquireLock()
{
    mMutex->lock();
}

void sys::ConditionVarNSPR::dropLock()
{
    mMutex->unlock();
}

void sys::ConditionVarNSPR::signal()
{
    if (PR_NotifyCondVar(mNative) != PR_SUCCESS)
        throw sys::SystemException("Condition Variable signal failed");

}

void sys::ConditionVarNSPR::wait()
{
    if (PR_WaitCondVar(mNative, PR_INTERVAL_NO_WAIT) != PR_SUCCESS)
        throw sys::SystemException("Condition Variable wait failed");
}

void sys::ConditionVarNSPR::wait(double seconds)
{
    double milli = seconds * 1000;
    if (PR_WaitCondVar(mNative, PR_MillisecondsToInterval((PRUint32) milli)) != PR_SUCCESS)
        throw sys::SystemException("Condition Variable wait failed");
}

void sys::ConditionVarNSPR::broadcast()
{
    if (PR_NotifyAllCondVar(mNative) != PR_SUCCESS)
        throw sys::SystemException("Condition Variable broadcast failed");
}

PRCondVar*& sys::ConditionVarNSPR::getNative()
{
    return mNative;
}

#endif

