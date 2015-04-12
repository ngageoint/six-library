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


#ifndef __SYS_MUTEX_INTERFACE_H__
#define __SYS_MUTEX_INTERFACE_H__



#if defined(_REENTRANT)
#include <typeinfo>
#include "sys/SystemException.h"
#include "sys/Dbg.h"

namespace sys
{

/*!
 *  \class MutexInterface
 *  \brief The interface for any mutex
 *
 *  This class defines the interface for any mutex in any package that
 *  is wrapped herein
 */
template <typename T> class MutexInterface
{
public:
    //!  Constructor
    MutexInterface()
    {
#ifdef THREAD_DEBUG
        dbg_printf("Creating a mutex\n");
#endif

    }

    //!  Destructor
    virtual ~MutexInterface()
    {
#ifdef THREAD_DEBUG
        dbg_printf("Destroying a mutex\n");
#endif

    }

    /*!
     *  Lock the mutex up.
     *  \return true upon success
     */
    virtual bool lock() = 0;

    /*!
     *  Unlock the mutex.
     *  \return true upon success
     */
    virtual bool unlock() = 0;

    /*!
     *  Returns the native type.  You probably should not use this
     *  unless you have specific constraints on which package you use
     *  Use of this function may defeat the purpose of these classes:
     *  to provide thread implementation in an abstract interface.
     */
    T& getNative()
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
    T mNative;

};

}

#endif // Are we reentrant?

#endif
