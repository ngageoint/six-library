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


#ifndef __SYS_THREAD_SOLARIS_MUTEX_H__
#define __SYS_THREAD_SOLARIS_MUTEX_H__


#if defined(__sun)

#include <thread.h>
#include <synch.h>
#include "sys/MutexInterface.h"

namespace sys
{
/*!
 *  \class MutexSolaris
 *  \brief The solaris threads implementation of a mutex
 *
 *  Implements a solaris thread mutex and wraps the outcome
 *
 */
class MutexSolaris : public MutexInterface
{
public:
    //!  Constructor
    MutexSolaris();

    //!  Destructor
    virtual ~MutexSolaris();

    /*!
     *  Lock the mutex.
     */
    virtual void lock();
    
    /*!
     *  Unlock the mutex.
     */
    virtual void unlock();
    
    /*!
     *  Returns the native type.
     */
    mutex_t& getNative();
    
    /*!
     *  Return the type name.  This function is essentially free,
     *  because it is static RTTI.
     */
    const char* getNativeType() const
    {
        return typeid(mNative).name();
    }
    
private:
    mutex_t mNative;
};
}

#endif
#endif
