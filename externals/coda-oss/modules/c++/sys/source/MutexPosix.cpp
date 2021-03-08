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

#include <sys/MutexPosix.h>

#if CODA_OSS_POSIX_SOURCE



sys::MutexPosix::MutexPosix()
{
    if (::pthread_mutex_init(&mNative, NULL) != 0)
        throw sys::SystemException("Mutex initialization failed");
}

sys::MutexPosix::~MutexPosix()
{
    if ( ::pthread_mutex_destroy(&mNative) == -1 )
    {
        ::pthread_mutex_unlock(&mNative);
        ::pthread_mutex_destroy(&mNative);
    }
}

void sys::MutexPosix::lock()
{
#ifdef THREAD_DEBUG
    dbg_printf("Locking mutex\n");
#endif
    if (::pthread_mutex_lock(&mNative) != 0)
        throw sys::SystemException("Mutex lock failed");
}

void sys::MutexPosix::unlock()
{
#ifdef THREAD_DEBUG
    dbg_printf("Unlocking mutex\n");
#endif
    if (::pthread_mutex_unlock(&mNative) != 0)
        throw sys::SystemException("Mutex unlock failed");

}

pthread_mutex_t& sys::MutexPosix::getNative()
{
    return mNative;
}

#endif

