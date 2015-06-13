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


#if defined(__sun)
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

void sys::MutexSolaris::lock()
{
#ifdef THREAD_DEBUG
    dbg_printf("Locking mutex\n");
#endif
    if (::mutex_lock(&mNative) != 0)
        throw sys::SystemException("Mutex lock failed");
}

void sys::MutexSolaris::unlock()
{
#ifdef THREAD_DEBUG
    dbg_printf("Unlocking mutex\n");
#endif
    if (::mutex_unlock(&mNative) != 0)
        throw sys::SystemException("Mutex unlock failed");
}

mutex_t& sys::MutexSolaris::getNative()
{
    return mNative;
}

#endif

