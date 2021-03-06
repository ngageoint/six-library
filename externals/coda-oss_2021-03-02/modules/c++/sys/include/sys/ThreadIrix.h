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


#ifndef __SYS_THREAD_IRIX_H__
#define __SYS_THREAD_IRIX_H__

#if defined(__sgi)
#include "sys/SyncFactoryIrix.h"
#include "sys/ThreadInterface.h"
#include <string>

namespace sys
{
inline long getThreadID()
{
    return (long)getpid();
}

class ThreadIrix : public ThreadInterface
{
public:

    ThreadIrix(const std::string& name = "") :
            ThreadInterface(name)
    {}

    ThreadIrix(sys::Runnable *target,
               const std::string& name = "") :
            ThreadInterface(target, name)
    {}

    ThreadIrix(sys::Runnable *target,
               const std::string& name,
               int level,
               int priority) :
            ThreadInterface(target, name, level, priority)
    {}

    virtual ~ThreadIrix()
    {}

    //virtual void start()
    //{
    //SyncFactoryIrix::getInstance()->createThread(*this);
    //void (*entry) (void *), unsigned inh,
    /*      mNative = sproc(start,  */
    /*        PR_SADDR|PR_SFDS|PR_SDIR|PR_SUMASK, */
    /*        this);  */
    /*      if (mNative <= 0) */
    /*   throw ThreadObjectException("ThreadIrix::start() failed"); */
    //}

    virtual void start()
    {
        dbg_printf("ThreadIrix::start()\n");
        if (!sys::SyncFactoryIrix().startThread(*this))
            throw sys::SystemException("start()");
    }

    virtual void kill()
    {
        dbg_printf("ThreadIrix::kill()\n");
        if (!sys::SyncFactoryIrix().killThread(*this))
            throw sys::SystemException("kill()");
    }

    static void __start(void *v)
    {
        EVAL(v);
        dbg_printf("ThreadIrix::__start()\n");
        sys::ThreadIrix *me = (sys::ThreadIrix *)v;
        me->target()->run();
        dbg_printf("ThreadIrix::__start() has completed\n");
        exit(EXIT_SUCCESS);
    }

    //virtual bool kill()
    //{
    //return SyncFactoryIrix->killThread(*this);
    /*      if ( ::kill(mNative, SIGKILL) == 0 ) { */
    /*   mIsRunning = false; */
    /*   return true; */
    /*      } */
    /*      return false; */
    //}
    virtual void join();

    static void yield();

    /*!
     *  Returns the native type.  You probably should not use this
     *  unless you have specific constraints on which package you use
     *  Use of this function may defeat the purpose of these classes:
     *  to provide thread implementation in an abstract interface.
     */
    pid_t& getNative()
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
    pid_t mNative;
};


}

#endif
#endif

