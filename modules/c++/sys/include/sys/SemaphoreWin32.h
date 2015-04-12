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


#ifndef __SYS_SEMAPHORE_WIN32_H__
#define __SYS_SEMAPHORE_WIN32_H__

#if defined(WIN32) && defined(_REENTRANT)

#if !defined(USE_NSPR_THREADS) && !defined(__POSIX)

#include "sys/Conf.h"
#include "sys/SemaphoreInterface.h"

namespace sys
{

class SemaphoreWin32 : public SemaphoreInterface<HANDLE>
{
public:
    enum { MAX_COUNT = 10 };
    SemaphoreWin32(unsigned int count = 0)
    {
        mNative = CreateSemaphore(NULL, count, MAX_COUNT, NULL);
        if (mNative == NULL)
            throw sys::SystemException("CreateSempaphore Failed");

    }
    virtual ~SemaphoreWin32()
    {
    }

    bool wait()
    {
        DWORD waitResult = WaitForSingleObject(
                               mNative,
                               INFINITE);
        if (waitResult != WAIT_OBJECT_0)
        {
            return false;
        }
        return true;

    }
    bool signal()
    {
        if (!ReleaseSemaphore(mNative,
                              1,
                              NULL) )
        {
            return false;
        }
        return true;
    }
};

}
#endif
#endif
#endif
