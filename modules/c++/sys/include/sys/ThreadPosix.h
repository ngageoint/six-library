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


#ifndef __SYS_THREAD_PTHREAD_THREAD_H__
#define __SYS_THREAD_PTHREAD_THREAD_H__

#if defined(__POSIX) && defined(_REENTRANT)

#include <sched.h>
#include <pthread.h>
#include "sys/ThreadInterface.h"


/*!
 *  \file ThreadPosix.h
 *  \brief Provides the compatibility layer for pthreads
 *
 *  If linked properly, this library allows the developer to leverage upon
 *  the pthreads API using C++ objects.
 */
namespace sys
{

    /*!
     *  Function returns a string identifier for the current
     *  thread.
     */
    inline long getThreadID()
    {
	return (long)pthread_self();
    }

/*!
 *  \class ThreadPosix
 *  \brief The implementation of a pthread-specialized thread
 *  
 *  This class provides the wrapper for a pthread_t.  
 *
 */
class ThreadPosix : public ThreadInterface<pthread_t>
{

public:
    typedef ThreadInterface<pthread_t> Parent_T;
    /*!
            *  Default constructor.  Allows ThreadInterface to bind this to target
            *  \param name  The name
            */
    ThreadPosix(std::string name = "") :
            Parent_T(name)
    {}
    /*!
     *  Alternate constructor
     *  \param target  What to run
            *  \param name  The name
     */
    ThreadPosix(Runnable *target,
                std::string name = "") :
            Parent_T(target, name)
    {}


    ThreadPosix(Runnable *target,
                std::string name,
                int level,
                int priority) :
            Parent_T(target, name, level, priority)
    {}

    //! Destructor
    virtual ~ThreadPosix()
    {}

    /*!
     *  The startpoint for thread processing
     */
    virtual void start();


    /*!
     *  Run function defined and bound to pthread_create.
     *  This should not be invoked directly
     *  \param v The start arg
     */
    static void *__start(void *v);


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


protected:

};

}

#endif
#endif
