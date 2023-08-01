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

#include "sys/SemaphoreWin32.h"

sys::SemaphoreWin32::SemaphoreWin32(unsigned int count, size_t _maxCount)
{
    // Ensure maxCount never becomes negative due to casting between signed/unsigned types
    constexpr auto maxLong = std::numeric_limits<LONG>::max();
    const auto maxCount = (_maxCount > maxLong) ? maxLong : static_cast<LONG>(_maxCount);

    mNative = CreateSemaphore(nullptr, static_cast<LONG>(count), maxCount, nullptr);
    if (mNative == nullptr)
        throw sys::SystemException("CreateSemaphore Failed");
}

void sys::SemaphoreWin32::wait()
{
    DWORD waitResult = WaitForSingleObject(
                           mNative,
                           INFINITE);
    if (waitResult != WAIT_OBJECT_0)
    {
        throw sys::SystemException("Semaphore wait failed");
    }
}

void sys::SemaphoreWin32::signal()
{
    if (!ReleaseSemaphore(mNative,
                          1,
                          nullptr) )
    {
        throw sys::SystemException("Semaphore signal failed");
    }
}

HANDLE& sys::SemaphoreWin32::getNative()
{
    return mNative;
}

#endif
#endif
