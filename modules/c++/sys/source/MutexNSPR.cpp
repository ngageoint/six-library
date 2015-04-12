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
#include "sys/MutexNSPR.h"

sys::MutexNSPR::MutexNSPR()
{
    mNative = PR_NewLock();
    if (mNative == NULL)
        throw sys::SystemException("Mutex initialization failed");
}
sys::MutexNSPR::~MutexNSPR()
{
    if (mNative != NULL)
        PR_DestroyLock(mNative);
}
void sys::MutexNSPR::lock()
{
    PR_Lock(mNative);
}

void sys::MutexNSPR::unlock()
{
    PRStatus status = PR_Unlock(mNative);
    if (status == PR_FAILURE)
        throw sys::SystemException("Mutex unlock failed");
}

PRLock*& sys::MutexNSPR::getNative()
{
    return mNative;
}

#endif

