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


#if defined(__sgi)
#include <ulocks.h>
#include "sys/MutexIrix.h"
#include "sys/SyncFactoryIrix.h"

sys::MutexIrix::MutexIrix()
{
    if (!sys::SyncFactoryIrix().createLock(*this))
        throw SystemException("Mutex initialization failed");
}

sys::MutexIrix::~MutexIrix()
{
    dbg_printf("~MutexIrix()\n");
    sys::SyncFactoryIrix().destroyLock(*this);
}

void sys::MutexIrix::lock()
{
    dbg_printf("MutexIrix::lock()\n");
    if (!sys::SyncFactoryIrix().setLock(*this))
        throw sys::SystemException("Mutex lock failed");
}

void sys::MutexIrix::unlock()
{
    dbg_printf("MutexIrix::unlock()\n");
    if (!sys::SyncFactoryIrix().unsetLock(*this))
        throw sys::SystemException("Mutex unlock failed");
}

ulock_t*& sys::MutexIrix::getNative()
{
    return mNative;
}

#endif // __sgi

