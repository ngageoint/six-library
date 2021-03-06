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


#ifndef __SYS_THREAD_SOLARIS_THREAD_H__
#define __SYS_THREAD_SOLARIS_THREAD_H__


#if defined(__sun)

#include <thread.h>
#include <synch.h>
#include "sys/ThreadInterface.h"

namespace sys
{

    inline long getThreadID()
    {
        return (long)thr_self();
    }

/*!
 *  \class ThreadSolaris
 *  \brief The implementation of a pthread-specialized thread
 *  
 *  This class provides the wrapper for a pthread_t.  
 *
 */
class ThreadSolaris : public ThreadInterface
{
public:

    //!  Default constructor.  Allows ThreadInterface to bind this to target
    ThreadSolaris(const std::string& name = "") : ThreadInterface(name)
    {}

    /*!
     *  Alternate constructor
     *  \param target  What to run
     *  \param name  The name
     */
    ThreadSolaris(Runnable *target,
                  const std::string& name = "") :
            ThreadInterface(target, name)
    {}

    ThreadSolaris(Runnable *target,
                  const std::string& name,
                  int level,
                  int priority) :
            ThreadInterface(target, name, level, priority)
    {}

    //! Destructor
    virtual ~ThreadSolaris()
    {}

    /*!
     *  The startpoint for thread processing
     */
    virtual void start();

    /*!
     *  Calls the native destroy stuff
     */
    virtual void kill();

    /*!
     *  Join the pthread
     */
    virtual void join();

    /*!
     *  Calls sched_yield to yield the thread of control
     */
    static void yield();
    
    /*!
     *  Returns the native type.  You probably should not use this
     *  unless you have specific constraints on which package you use
     *  Use of this function may defeat the purpose of these classes:
     *  to provide thread implementation in an abstract interface.
     */
    thread_t& getNative()
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
    thread_t mNative;
};

}

extern "C"
{
    /*!
     *  Run function defined and bound to pthread_create.
     *  This should not be invoked directly
     *  \param v The start arg
     */
    void *__sys_ThreadSolaris_start(void *v);

}
#endif
#endif
