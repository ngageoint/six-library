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


#ifndef __SYS_THREAD_SOLARIS_CONDITION_VARIABLE_H__
#define __SYS_THREAD_SOLARIS_CONDITION_VARIABLE_H__

#if defined(__sun)
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
class ConditionVarSolaris : public sys::ConditionVarInterface
{
public:
    ConditionVarSolaris();

    //!  Constructor
    ConditionVarSolaris(MutexSolaris* theLock, bool isOwner = false);

    //!  Destructor
    virtual ~ConditionVarSolaris();

    /*!
     *  Acquire the lock
     */
    virtual void acquireLock();

    /*!
     *  Drop (release) the lock
     */
    virtual void dropLock();
    
    /*!
     *  Signal using solaris cond_signal
     */
    virtual void signal();

    /*!
     *  Wait using cond_wait
     *
     *  WARNING: The user is responsible for locking the mutex prior 
     *           to using this method. There will be no check and on 
     *           certain systems, undefined/unfavorable behavior may 
     *           result.
     */
    virtual void wait();

    /*!
     *  Wait using cond_timed_wait.  I kept this and the above
     *  function separate only to be explicit.
     *  \param seconds Fraction of a second to wait.  Want a TimeInterval
     *  \todo  Make a parameter initializer that does wait()
     *
     *  WARNING: The user is responsible for locking the mutex prior 
     *           to using this method. There will be no check and on 
     *           certain systems, undefined/unfavorable behavior may 
     *           result.
     */
    virtual void wait(double seconds);

    /*!
     *  Broadcast (notify all)
     */
    virtual void broadcast();
    
    /*!
     *  Returns the native type.
     */
    cond_t& getNative();
    
    /*!
     *  Return the type name.  This function is essentially free,
     *  because it is static RTTI.
     */
    const char* getNativeType() const
    {
        return typeid(mNative).name();
    }
    
private:
    // This is set if we own the mutex, to make sure it gets deleted.
    std::unique_ptr<MutexSolaris> mMutexOwned;
    MutexSolaris *mMutex;
    cond_t mNative;
};

}

#endif
#endif
