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


#ifdef _WIN32

#if !defined(USE_NSPR_THREADS)

#include "sys/MutexWin32.h"

sys::MutexWin32::MutexWin32()
{
    mNative = CreateMutex(nullptr, FALSE, nullptr);
    if (mNative == nullptr)
        throw sys::SystemException("Mutex initializer failed");
}

sys::MutexWin32::~MutexWin32()
{
    CloseHandle(mNative);
}

void sys::MutexWin32::lock()
{
    if (WaitForSingleObject(mNative, INFINITE) == WAIT_FAILED)
        throw sys::SystemException("Mutex lock failed");
}

void sys::MutexWin32::unlock()
{
    if (ReleaseMutex(mNative) != TRUE)
        throw sys::SystemException("Mutex unlock failed");
}

HANDLE& sys::MutexWin32::getNative()
{
    return mNative;
}

#endif // Not some other thread package

#endif // Windows platform
