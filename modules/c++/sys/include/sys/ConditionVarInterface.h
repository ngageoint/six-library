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


#ifndef __SYS_CONDITION_VAR_INTERFACE_H__
#define __SYS_CONDITION_VAR_INTERFACE_H__

#if defined(_REENTRANT)
#include <typeinfo>
#include "sys/SystemException.h"
namespace sys
{
/*!
 *  \class ConditionVarInterface
 *  \brief Interface for any condition variable implementation
 *
 *  Provide the API for condition variables in this thread wrapper
 */
template <typename NativeConditionVar_T, typename Mutex_T>
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
    ConditionVarInterface() : mMutex(NULL), mIsOwner(true)
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
    ConditionVarInterface(Mutex_T *theLock, bool isOwner = false) :
            mMutex(NULL)
    {
        mIsOwner = isOwner;
        mMutex = theLock;
    }

    //!  Destructor
    virtual ~ConditionVarInterface()
    {
        if (mIsOwner && mMutex != NULL)
        {
            delete mMutex;
            mMutex = NULL;
        }

    }

    /*!
     *  Acquire the lock
     *  \return true upon success
     */
    virtual bool acquireLock()
    {
        return mMutex->lock();
    }

    /*!
     *  Drop (release) the lock
     *  \return true upon success
     */
    virtual bool dropLock()
    {
        return mMutex->unlock();
    }

    /*!
     *  Wait for on a signal for a time interval.  
     *  This should eventually have
     *  a class TimeInterval as the second argument, which takes
     *  any time interval as a right-hand-side
     *  \param timeout How long to wait.  This is only temporarily
     *  a double
     *  \todo  Create a TimeInterval class, and use it as parameter
     *  \return true upon success
     */
    virtual bool wait(double timeout) = 0;

    /*!
     *  Wait on a signal
     *  \return true upon success
     */
    virtual bool wait() = 0;

    /*!
     *  Signal (notify)
     *  \return true upon success
         */
    virtual bool signal() = 0;

    /*!
     *  Broadcast (notify all)
     *  \return true upon success
     */
    virtual bool broadcast() = 0;

    /*!
     *  Returns the native type.  You probably should not use this
     *  unless you have specific constraints on which package you use
     *  Use of this function may defeat the purpose of these classes:
     *  to provide thread implementation in an abstract interface.
     */
    NativeConditionVar_T& getNative()
    {
        return mNative;
    }

    /*!
     *  Return the type name.  This function is essentially free,
     *  because it is static RTTI due to its template implementation
     */
    const char* getNativeType() const
    {
        return typeid(mNative).name();
    }

protected:
    NativeConditionVar_T mNative;
    Mutex_T *mMutex;
    bool mIsOwner;

};

}
#endif
#endif
