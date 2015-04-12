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


#ifndef __SYS_THREAD_WIN32_H__
#define __SYS_THREAD_WIN32_H__

#if defined(WIN32) && defined(_REENTRANT)

#if !defined(USE_NSPR_THREADS) && !defined(__POSIX)

#include "sys/ThreadInterface.h"
#include "sys/Conf.h"
#include <sstream>


#define sleep Sleep

// A typedef and macro to make _beginthreadex look like CreateThread
// See MSDN Wind32 Q&A  July 1999  by Jeffrey Richter
typedef unsigned (__stdcall *THREAD_START_FN) (void *);

#if defined(USE_CREATETHREAD)
#   define __CREATETHREAD(psa, cbStack, pfnStartAddr, \
                       pvParam, fdwCreate, pdwThreadID) \
        CreateThread(psa, cbStack, \
        (LPTHREAD_START_ROUTINE)(pfnStartAddr), \
        (void*)pvParam, fdwCreate, \
        pdwThreadID)

#else
#   define  __CREATETHREAD(psa, cbStack, pfnStartAddr,         \
                        pvParam, fdwCreate, pdwThreadID)    \
   ((HANDLE) _beginthreadex(                                \
                            (void *) (psa),                 \
                            (unsigned) (cbStack),           \
                            (THREAD_START_FN) (pfnStartAddr), \
                            (void *) (pvParam),             \
                            (unsigned) (fdwCreate),         \
                            (unsigned *) (pdwThreadID)))
#endif
namespace sys
{


    inline long getThreadID()
    {
	return (long)GetCurrentThreadId();
    }

class ThreadWin32 : public ThreadInterface<HANDLE>
{
public:
    typedef ThreadInterface<HANDLE> Parent_T;

    ThreadWin32(std::string name = "") : Parent_T(name)
    {}

    ThreadWin32(sys::Runnable *target,
                std::string name = "") :
            Parent_T(target, name)
    {}

    ThreadWin32(sys::Runnable *target,
                std::string name,
                int level,
                int priority) :
            Parent_T(target, name, level, priority)
    {}


    virtual ~ThreadWin32();
    virtual void start();
    static DWORD WINAPI __start(void *v)
    {

        STANDARD_START_CALL(ThreadWin32, v);

        return 0;
    }

    //! Not implemented
    virtual bool kill()
    {
        return false;
    }
    virtual bool join();
    static void yield()
    {
#ifdef USING_WINNT
        SwitchToThread();
#else
        throw SystemException("Thread::yield() only supported in windows NT 4.0 or greater!");
#endif

    }
};

}

#endif // Not using other thread package
#endif // Is windows
#endif
