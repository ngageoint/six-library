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


#ifndef __SYS_THREAD_INTERFACE_H__
#define __SYS_THREAD_INTERFACE_H__

#include "sys/Runnable.h"

#include <typeinfo>
#include <iostream>
#include "sys/SystemException.h"


namespace sys
{
/*!
 *  \def STANDARD_START_CALL(MY_NAME, PTR_TO_ME)
 *  You should probably place this in you compatibility 
 *  layer if you are writing a CTI integration package.
 *  call it as follows:
 *
 *  \code
 *   static void* PthreadThread::start(void* v)
 *   {
 *     // declare this function. 
 *     STANDARD_START_CALL(Win32Thread, v);
 *     return NULL;
 *   }
 *  \endcode
 */
#define STANDARD_START_CALL(MY_NAME, PTR_TO_ME) \
   sys::MY_NAME *me = \
        static_cast<sys::MY_NAME*>(PTR_TO_ME); \
   me->setIsRunning(true); \
   me->target()->run(); \
   me->setIsRunning(false); \


/*!
 * \class ThreadInterface
 * \brief defines a thread, which implements a runnable
 * \todo  Add the string name variable and associate values
 *  
 * A thread in java implements runnable, allowing it to pass
 * threads as runners.  The run part of the interface
 * does nothing in the parent thread, however, if it is
 * set to a thread, that thread's run function may be called
 *
 */

struct ThreadInterface : public Runnable
{
    enum { DEFAULT_LEVEL, KERNEL_LEVEL, USER_LEVEL };
    enum { MINIMUM_PRIORITY, NORMAL_PRIORITY, MAXIMUM_PRIORITY };

    //! Default constructor
    ThreadInterface() : mIsSelf(true)
    {
        initialize(this, NORMAL_PRIORITY, DEFAULT_LEVEL, "");
    }
    /*!
     *  This, in C++, may seem redundant, since
     *  I can use the default constructor, but
     *  I want to be really obvious with this
     *  API
     *  \param name The name of this thread
     */
    ThreadInterface(const std::string& name) : mIsSelf(true)
    {
        initialize(this, NORMAL_PRIORITY, DEFAULT_LEVEL, name);
    }


    /*!
     *  Constructor
     *  \param target What to run
     */
    ThreadInterface(Runnable *target) : mIsSelf(false)
    {
        initialize(target, NORMAL_PRIORITY, DEFAULT_LEVEL, "");
    }

    /*!
     *  This, in C++, may seem redundant, since
     *  I can use the default constructor, but
     *  I want to be really obvious with this
     *  API
     *  \param target What to run
     *  \param name The name of this thread
     */
    ThreadInterface(Runnable *target,
                    const std::string& name) : mIsSelf(false)
    {
        initialize(target, NORMAL_PRIORITY, DEFAULT_LEVEL, name);
    }

    ThreadInterface(Runnable *target,
                    const std::string& name,
                    int level,
                    int priority) : mIsSelf(false)
    {
        initialize(target, priority, level, name);
    }

    //!  Destructor
    virtual ~ThreadInterface()
    {
        // If the thread is still running, crash the program to prevent all kinds
        // of nasty issues that could pop up (execution in freed memory, etc).
        if (isRunning())
        {
            std::cerr << Ctxt(FmtX("Thread object [%s] destructed before " \
                                   "thread terminated, aborting program.", 
                                   getName().c_str())) << std::endl;
            abort();
        }
        
        if (mTarget && mTarget != this)
            delete mTarget;
    }

    /*!
     *  Return the name of this thread
     *  \return Thread name
     */
    std::string getName() const
    {
        return mName;
    }

    /*!
     *  Set the name of the thread
     *  \param name New name for thread
     */
    void setName(const std::string& name)
    {
        mName = name;
    }

    /*!
     *  Get the level at which this thread runs.
     *  DEFAULT_LEVEL means "I dont know or care"
     *  \return The level
     */
    int getLevel() const
    {
        return mLevel;
    }

    /*!
     *  Get the thread priority
     *  \return The thread priority
     */
    int getPriority() const
    {
        return mPriority;
    }

    /*!
     *  Set the thread priority.  The meaning of this
     *  function depends upon the implementing thread packages.
     *  Please consult the notes individually for interpretations
     *  of the behavior.  Setting the priority may take effect immediately,
     *  or it may not.  Either way, the behavior should be graceful so
     *  as not to disturb the functionality of the actual thread.
     *
     *  \param priority The new priority
     */
    virtual void setPriority(int priority)
    {
        mPriority = priority;
    }

    /*!
     *  Start the thread
     *  This function is called by the thread package user.
     *
     */
    virtual void start() = 0;

    /*!
     *  Destroy the thread
     */
    virtual void kill() = 0;

    // You could say Thread(new MyThread()).start();
    /*!
     *  The only function that is specialized by the implementor
     *  This function is called by start if no target is defined,
     *  allowing the implementor to inherit this class directly
     */
    virtual void run()
    {}

    /*!
     *  Join the thread
     */
    virtual void join() = 0;

    /*!
     *  Yield the current thread of control
     */
    static void yield();
    //     std::string toString() const { return mName; }

    /*!
     *  Return the target
     *  \return The target runnable
     */
    Runnable* target()
    {
        return mTarget;
    }

    bool isRunning()
    {
        return mIsRunning;
    }
    void setIsRunning(bool isRunning)
    {
        mIsRunning = isRunning;
    }

        
    ThreadInterface(const ThreadInterface&) = delete;
    ThreadInterface& operator=(const ThreadInterface&) = delete;

private:
    bool mIsSelf;

    /*!
     *  Generic initialization routine for constructors
     *  to call.  These arguments always need to be
     *  initialized
     *
     */
    void initialize(Runnable *target,
                    int priority,
                    int level,
                    const std::string& name)
    {
        mTarget = target;
        mName = name;
        mLevel = level;
        mPriority = priority;
        mIsRunning = false;
    }

    //!  The target (could be this)
    Runnable *mTarget;
    //!  The name of this thread
    std::string mName;
    //!  The priority of this thread
    int mPriority;
    //!  The level at which this thread runs
    int mLevel;
    bool mIsRunning;
};
}

#endif
