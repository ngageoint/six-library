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


#ifndef __SYS_THREAD_PTHREAD_CONDITION_VARIABLE_H__
#define __SYS_THREAD_PTHREAD_CONDITION_VARIABLE_H__

#if defined(__POSIX) && defined(_REENTRANT)

#include "sys/MutexPosix.h"
#include "sys/ConditionVarInterface.h"
#include <pthread.h>

namespace sys
{

/*!
 *  \class ConditionVarPosix
 *  \brief Implementation for a pthread condition variable
 *
 *  This class is the wrapper implementation for a pthread_cond_t
 *  (Pthread condition variable)
 */
class ConditionVarPosix :
            public ConditionVarInterface < pthread_cond_t,
            MutexPosix >
{
public:
    typedef ConditionVarInterface < pthread_cond_t, MutexPosix > Parent_T;

    ConditionVarPosix()
    {
        mMutex = new MutexPosix();
        assert( ::pthread_cond_init(&mNative, NULL) == 0);
        /*      if ( ::pthread_cond_init(&mNative, NULL) != 0) */
        /*   throw SystemException("ConditionVar initialization failed"); */

    }

    //!  Constructor
    ConditionVarPosix(MutexPosix* theLock, bool isOwner = false) :
            Parent_T(theLock, isOwner)
    {
        assert( ::pthread_cond_init(&mNative, NULL) == 0);
        /*      if ( ::pthread_cond_init(&mNative, NULL) != 0) */
        /*   throw SystemException("ConditionVar initialization failed"); */
    }

    //!  Destructor
    virtual ~ConditionVarPosix();

    /*!
     *  Signal using pthread_cond_signal
     *  \return true if success
     */
    virtual bool signal();

    /*!
     *  Wait using pthread_cond_wait
     *  \return true if success
     */
    virtual bool wait();

    /*!
     *  Wait using pthread_cond_timed_wait.  I kept this and the above
     *  function separate only to be explicit.
     *  \param seconds Fraction of a second to wait.  
     *  \todo Want a TimeInterval
     *  \return true if success
     */
    virtual bool wait(double seconds);

    /*!
     *  Broadcast (notify all)
     *  \return true if success
     */
    virtual bool broadcast();
};
}

#endif
#endif
