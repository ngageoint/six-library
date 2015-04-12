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


#ifndef __SYS_THREAD_SOLARIS_CONDITION_VARIABLE_H__
#define __SYS_THREAD_SOLARIS_CONDITION_VARIABLE_H__

#if defined(__sun) && defined(_REENTRANT) && !defined(__POSIX)
#include <thread.h>
#include <synch.h>
#include "sys/ConditionVarInterface.h"
#include "sys/MutexSolaris.h"
namespace sys
{
/*!
 *  \class ConditionVarSolaris
 *  \brief Implementation for a solaris thread condition variable
 *
 *  This class is the wrapper implementation for a solaris cond_t
 *  (Solaris condition variable)
 */
class ConditionVarSolaris :
            public sys::ConditionVarInterface < cond_t,
            sys::MutexSolaris >
{
public:
    typedef ConditionVarInterface < cond_t, MutexSolaris > Parent_T;
    ConditionVarSolaris()
    {
        mMutex = new MutexSolaris();
        if ( ::cond_init(&mNative, NULL, NULL) != 0)
            throw sys::SystemException("ConditionVar initialization failed");
    }

    //!  Constructor
    ConditionVarSolaris(MutexSolaris* theLock, bool isOwner = false) :
            Parent_T(theLock, isOwner)
    {
        if ( ::cond_init(&mNative, NULL, NULL) != 0)
            throw sys::SystemException("ConditionVar initialization failed");
    }

    //!  Destructor
    virtual ~ConditionVarSolaris();

    /*!
     *  Signal using solaris cond_signal
     *  \return true if success
     */
    virtual bool signal();

    /*!
     *  Wait using cond_wait
     *  \return true if success
     */
    virtual bool wait();

    /*!
     *  Wait using cond_timed_wait.  I kept this and the above
     *  function separate only to be explicit.
     *  \param seconds Fraction of a second to wait.  Want a TimeInterval
     *  \todo  Make a parameter initializer that does wait()
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
