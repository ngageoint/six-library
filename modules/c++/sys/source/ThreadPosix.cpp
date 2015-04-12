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


#include "sys/ThreadPosix.h"

#if defined(__POSIX) && defined(_REENTRANT)

#ifdef WIN32
#   define SIGKILL 0
#endif
void sys::ThreadPosix::start()
{

    if (mLevel != DEFAULT_LEVEL)
    {
        throw sys::SystemException("Cannot determine upfront wheteher pthread threads are implemented using kernel or user level threads.  Set the level to DEFAULT_LEVEL");
    }

    if (mPriority != NORM_PRIORITY)
    {
        sched_param sp;

        if (mPriority == MAX_PRIORITY)
            sp.sched_priority = sched_get_priority_max(SCHED_OTHER);

        else if (mPriority == MIN_PRIORITY)
            sp.sched_priority = sched_get_priority_min(SCHED_OTHER);

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setschedparam(&attr,
                                   &sp);

        if (::pthread_create(&mNative,
                             &attr,
                             (void *(*)(void *))this->__start,
                             this) != 0)
            throw sys::SystemException("pthread_create()");
        pthread_attr_destroy(&attr);
    }
    else
    {
        if (::pthread_create(&mNative,
                             NULL,
                             (void *(*)(void *))this->__start,
                             this) != 0)
            throw sys::SystemException("pthread_create()");
    }
}
void *sys::ThreadPosix::__start(void *v)
{


    STANDARD_START_CALL(ThreadPosix, v);
    /*
    sys::Runnable *runnable = 
        static_cast<sys::Runnable *>(v);

    runnable->run();

    // If the pointers don't match, then the runnable is not 'this'
    // and therefore it's ok to delete the runnable.
    delete runnable;
    */

    pthread_exit(NULL);
    return NULL;
}

bool sys::ThreadPosix::kill()
{
    if ( pthread_kill(mNative, SIGKILL) != 0)
    {
        return false;
    }
    mIsRunning = false;
    return true;
}
bool sys::ThreadPosix::join()
{

    return ( ::pthread_join(mNative, NULL) == 0);

}
void sys::ThreadPosix::yield()
{
    ::sched_yield();
}

#endif

