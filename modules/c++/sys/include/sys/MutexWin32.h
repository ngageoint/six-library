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


#ifndef __SYS_MUTEX_WIN32_H__
#define __SYS_MUTEX_WIN32_H__

#if defined(WIN32) && defined(_REENTRANT)
#if !defined(USE_NSPR_THREADS) && !defined(__POSIX)

#include "sys/MutexInterface.h"

namespace sys
{
class MutexWin32 : public MutexInterface<HANDLE>
{
public:
    //! \todo Add string name option
    MutexWin32();
    virtual ~MutexWin32();
    /*!
     *  Lock the mutex.
     *  \return true if success
     */
    virtual bool lock();

    /*!
     *  Unlock the mutex.
     *  \return true if success
     */
    virtual bool unlock();

};
}
#endif
#endif
#endif
