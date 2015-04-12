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


#ifndef __SYS_THREAD_IRIX_H__
#define __SYS_THREAD_IRIX_H__

#if defined(__sgi) && defined(_REENTRANT) && !defined(__POSIX)
#include "sys/SyncFactoryIrix.h"
#include "sys/ThreadInterface.h"
#include <string>

namespace sys
{
inline long getThreadID()
{
    return (long)getpid();
}

class ThreadIrix : public ThreadInterface<pid_t>
{
public:
    typedef ThreadInterface<pid_t> Parent_T;

    ThreadIrix(std::string name = "") :
            Parent_T(name)
    {}

    ThreadIrix(sys::Runnable *target,
               std::string name = "") :
            Parent_T(target, name)
    {}

    ThreadIrix(sys::Runnable *target,
               std::string name,
               int level,
               int priority) :
            Parent_T(target, name, level, priority)
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
        assert(sys::SyncFactoryIrix().startThread(*this));
    }

    virtual bool kill()
    {
        dbg_printf("ThreadIrix::kill()\n");
        return sys::SyncFactoryIrix().killThread(*this);
    }

    static void __start(void *v)
    {
        EVAL(v);
        dbg_printf("ThreadIrix::__start()\n");
        sys::ThreadIrix *me = (sys::ThreadIrix *)v;
        me->target()->run();
        if (me != me->target())
        {
            delete me->target();
        }
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
    virtual bool join();

    static void yield();

};


}

#endif
#endif

