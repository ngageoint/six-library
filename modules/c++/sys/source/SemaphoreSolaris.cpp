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
#include <synch.h>
#include "sys/SemaphoreSolaris.h"

sys::SemaphoreSolaris::SemaphoreSolaris(unsigned int count)
{
    sema_init(&mNative, count, USYNC_THREAD, NULL);
}

sys::SemaphoreSolaris::~SemaphoreSolaris()
{
    sema_destroy(&mNative);
}

void sys::SemaphoreSolaris::wait()
{
    if (sema_wait(&mNative) != 0)
        throw sys::SystemException("Semaphore wait failed");
}

void sys::SemaphoreSolaris::signal()
{
    if (sema_post(&mNative) != 0)
        throw sys::SystemException("Semaphore signal failed");
}

sema_t& sys::SemaphoreSolaris::getNative()
{
    return mNative;
}

#endif

