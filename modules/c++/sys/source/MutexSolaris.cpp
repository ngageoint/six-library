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


#if defined(__sun) && defined(_REENTRANT) && !defined(__POSIX)
#include "sys/MutexSolaris.h"

sys::MutexSolaris::MutexSolaris()
{
    if (::mutex_init(&mNative, USYNC_THREAD, NULL) != 0)
        throw sys::SystemException("Mutex initialization failed");
}

sys::MutexSolaris::~MutexSolaris()
{
    if ( ::mutex_destroy(&mNative) != 0 )
    {
        ::mutex_unlock(&mNative);
        ::mutex_destroy(&mNative);
    }
}

bool sys::MutexSolaris::lock()
{
#ifdef THREAD_DEBUG
    dbg_printf("Locking mutex\n");
#endif
    return (::mutex_lock(&mNative) == 0);
}

bool sys::MutexSolaris::unlock()
{
#ifdef THREAD_DEBUG
    dbg_printf("Unlocking mutex\n");
#endif
    return (::mutex_unlock(&mNative) == 0);
}

#endif

