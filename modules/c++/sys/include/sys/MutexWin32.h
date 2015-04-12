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


#ifndef __SYS_MUTEX_WIN32_H__
#define __SYS_MUTEX_WIN32_H__

#if defined(WIN32)
#if !defined(USE_NSPR_THREADS)

#include "sys/MutexInterface.h"

namespace sys
{
class MutexWin32 : public MutexInterface
{
public:
    //! \todo Add string name option
    MutexWin32();
    virtual ~MutexWin32();
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
    HANDLE& getNative();
    
    /*!
     *  Return the type name.  This function is essentially free,
     *  because it is static RTTI.
     */
    const char* getNativeType() const
    {
        return typeid(mNative).name();
    }
    
private:
    HANDLE mNative;
};
}
#endif
#endif
#endif
