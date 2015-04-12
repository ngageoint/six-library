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

#include "sys/MutexWin32.h"

sys::MutexWin32::MutexWin32()
{

    mNative = CreateMutex(NULL, FALSE, NULL);
    if (mNative == NULL)
        throw sys::SystemException("Mutex initializer failed");
}

sys::MutexWin32::~MutexWin32()
{
    CloseHandle(mNative);
}

bool sys::MutexWin32::lock()
{
    return (WaitForSingleObject(mNative, INFINITE) != WAIT_FAILED);
}

bool sys::MutexWin32::unlock()
{
    return (ReleaseMutex(mNative) == TRUE);
}

#endif // Not some other thread package

#endif // Windows platform
