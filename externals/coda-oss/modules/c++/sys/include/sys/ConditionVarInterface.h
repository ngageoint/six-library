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


#ifndef __SYS_CONDITION_VAR_INTERFACE_H__
#define __SYS_CONDITION_VAR_INTERFACE_H__

#include <typeinfo>
#include "sys/SystemException.h"
#include "sys/Mutex.h"
namespace sys
{
/*!
 *  \class ConditionVarInterface
 *  \brief Interface for any condition variable implementation
 *
 *  Provide the API for condition variables in this thread wrapper
 */

class ConditionVarInterface
{
public:

    /*!
     *  This constructor means that you are creating the lock
     *  that you will use inside of the condition variable.
     *  This means that we should delete this ourselves,
     *  so the mIsOwner flag is set to true.  Its up to the derived
     *  class to implement this default constructor.  Really, this
     *  option is only necessary for CASPR condition variables.  It
     *  is not the recommended behavior but it may be useful.
     */
    ConditionVarInterface()
    {}

    /*!
     *  This constructor is the preferred method for creating a new
     *  condition variable.  It shares a global lock, which must be
     *  deleted externally, by default.
     *
     *  \param theLock  This is an existing lock that you wish to
     *  use for synchronization purposes
     *
     *  \param isOwner This defaults to false, which means that your
     *  condition variable is not responsible for the deletion of the 
     *  mutex (in other words, you must delete the mutex explicitly.
     *  If you set this parameter to true, you may actually be sharing
     *  a lock, but this class will STILL delete it.
     *
     */
    ConditionVarInterface(Mutex *, bool = false)
    {}

    //!  Destructor
    virtual ~ConditionVarInterface()
    {}

    /*!
     *  Acquire the lock
     */
    virtual void acquireLock() = 0;

    /*!
     *  Drop (release) the lock
     */
    virtual void dropLock() = 0;

    /*!
     *  Wait for on a signal for a time interval.  
     *  This should eventually have
     *  a class TimeInterval as the second argument, which takes
     *  any time interval as a right-hand-side
     *  \param timeout How long to wait.  This is only temporarily
     *  a double
     *  \todo  Create a TimeInterval class, and use it as parameter
     *
     *  WARNING: The user is responsible for locking the mutex prior 
     *           to using this method. There will be no check and on 
     *           certain systems, undefined/unfavorable behavior may 
     *           result.
     */
    virtual void wait(double timeout) = 0;

    /*!
     *  Wait on a signal
     *
     *  WARNING: The user is responsible for locking the mutex prior 
     *           to using this method. There will be no check and on 
     *           certain systems, undefined/unfavorable behavior may 
     *           result.
     */
    virtual void wait() = 0;

    /*!
     *  Signal (notify)
     */
    virtual void signal() = 0;

    /*!
     *  Broadcast (notify all)
     */
    virtual void broadcast() = 0;

};

}
#endif
