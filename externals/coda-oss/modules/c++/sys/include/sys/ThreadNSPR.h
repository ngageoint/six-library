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


#ifndef __SYS_THREAD_NSPR_THREAD_H__
#define __SYS_THREAD_NSPR_THREAD_H__

#  if defined(USE_NSPR_THREADS)
#include "sys/ThreadInterface.h"
#include <prlock.h>
#include <prcvar.h>
#include <prthread.h>
#include <sys/Convert.h>


namespace sys
{

    inline long getThreadID()
    {
	return (long) PR_GetCurrentThread();
    }


/*!
 *  \class ThreadNSPR
 *  \brief The implementation for Netscape Portable Runtime threads
 *
 *  This class wraps the Netscape thread code.  It can be used with
 *  default constructors, making it a user-level thread.  It could
 *  also be called with a specialized derived class.  The custom
 *  functions here allow for some non-standard manipulation (for
 *  example, the number of CPU's).  Somebody put a lot of work into
 *  NSPR, and I didnt want to avoid some of the clever features,
 *  just because pthreads or Caspr dont support them
 *
 */
class ThreadNSPR : public ThreadInterface
{
public:
    /*!
     *  The default constructor.  When called with no args,
     *  produce a user-level local thread
     *  \param name     Name of thread
     *  \param isLocal  Set to true if you want local threads
     */
    ThreadNSPR(const std::string& name = "",
               bool isLocal = true) :
            ThreadInterface(name)
    {
        mIsLocal = isLocal;
    }

    /*!
     *  The alternate constructor.  This binds a runnable.
     *  The second and third args are the same as in the default
     *  constructor.
     *  \param target   What to run
     *  \param name     Name of thread
     *  \param isLocal  Set to true if you want local threads
     */
    ThreadNSPR(Runnable *target,
               const std::string& name = "",
               bool isLocal = true) :
            ThreadInterface(target, name)
    {
        mIsLocal = isLocal;
    }

    ThreadNSPR(Runnable *target,
               const std::string& name,
               int level,
               int priority,
               bool isLocal = true) :
            ThreadInterface(target, name, level, priority)
    {
        mIsLocal = isLocal;
    }
    //!  The destructor
    virtual ~ThreadNSPR()
    {}

    /*!
     *  Start the thread running.  Same as in other thread 
     *  implementations
     */
    virtual void start();

    /*!
     *  The start function that is bound to the native create call
     *  This should not be used directly
     *  \param v The start arg
     */
    static void __start(void *v);

    /*!
     *  The kill function.  Killing doesnt work on NSPR threads.
     *  Please refer to
     *  http://www.mozilla.org/projects/nspr/tech-notes/abruptexit.html
     *  for more information
     */
    virtual void kill()
    {
        throw sys::SystemException("kill() not implemented on NSPR");
    }
    /*!
     *  Join the thread
     *
     */
    virtual void join();
    /*!
     *  Yield the thread.  Calls PR_Sleep with an interval of
     *  PR_INTERVAL_NO_WAIT
     *
     */
    static void yield();
    
    /*!
     *  Returns the native type.  You probably should not use this
     *  unless you have specific constraints on which package you use
     *  Use of this function may defeat the purpose of these classes:
     *  to provide thread implementation in an abstract interface.
     */
    PRThread *& getNative()
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
    
protected:
    bool mIsLocal;
private:
    PRThread * mNative;
};
}

#  endif
#endif

