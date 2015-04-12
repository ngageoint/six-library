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


#ifndef __SYS_THREAD_SOLARIS_THREAD_H__
#define __SYS_THREAD_SOLARIS_THREAD_H__


#if defined(__sun) && defined(_REENTRANT) && !defined(__POSIX)

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
class ThreadSolaris : public ThreadInterface<thread_t>
{
public:

    typedef ThreadInterface<thread_t> Parent_T;
    //!  Default constructor.  Allows ThreadInterface to bind this to target
    ThreadSolaris(std::string name = "") : Parent_T(name)
    {}

    /*!
     *  Alternate constructor
     *  \param target  What to run
            *  \param name  The name
     */
    ThreadSolaris(Runnable *target,
                  std::string name = "") :
            Parent_T(target, name)
    {}

    ThreadSolaris(Runnable *target,
                  std::string name,
                  int level,
                  int priority) :
            Parent_T(target, name, level, priority)
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
    virtual bool kill();

    /*!
     *  Join the pthread
     */
    virtual bool join();

    /*!
     *  Calls sched_yield to yield the thread of control
     */
    static void yield();

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
