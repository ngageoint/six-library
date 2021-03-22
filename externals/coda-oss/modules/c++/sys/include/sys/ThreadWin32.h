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


#ifndef __SYS_THREAD_WIN32_H__
#define __SYS_THREAD_WIN32_H__

#if defined(WIN32) || defined(_WIN32)

#if !defined(USE_NSPR_THREADS)

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
	return static_cast<long>(GetCurrentThreadId());
    }

class ThreadWin32 : public ThreadInterface
{
public:

    ThreadWin32(const std::string& name = "") : ThreadInterface(name)
    {}

    ThreadWin32(sys::Runnable *target,
                const std::string& name = "") :
            ThreadInterface(target, name)
    {}

    ThreadWin32(sys::Runnable *target,
                const std::string& name,
                int level,
                int priority) :
            ThreadInterface(target, name, level, priority)
    {}


    virtual ~ThreadWin32();
    virtual void start();
    static DWORD WINAPI __start(void *v)
    {
        STANDARD_START_CALL(ThreadWin32, v);

        return 0;
    }

    //! Not implemented
    virtual void kill()
    {
        throw sys::SystemException("kill() not implemented in ThreadWin32");
    }
    virtual void join();
    static void yield()
    {
#ifdef USING_WINNT
        SwitchToThread();
#else
        throw sys::SystemException("Thread::yield() only supported in windows NT 4.0 or greater!");
#endif

    }
    
    /*!
     *  Returns the native type.  You probably should not use this
     *  unless you have specific constraints on which package you use
     *  Use of this function may defeat the purpose of these classes:
     *  to provide thread implementation in an abstract interface.
     */
    HANDLE & getNative()
    {
        return mNative;
    }

    /*!
     *  Return the type name.  This function is essentially free,
     *  because it is static RTTI.
     */
    const char* getNativeType() const
    {
        return typeid(mNative).name();
    }
    
private:
    HANDLE mNative;
};

}

#endif // Not using other thread package
#endif // Is windows
#endif
