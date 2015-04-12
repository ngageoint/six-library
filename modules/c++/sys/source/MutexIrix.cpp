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


#if defined(__sgi) && defined(_REENTRANT) && !defined(__POSIX)
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

bool sys::MutexIrix::lock()
{
    dbg_printf("MutexIrix::lock()\n");
    return sys::SyncFactoryIrix().setLock(*this);
}

bool sys::MutexIrix::unlock()
{
    dbg_printf("MutexIrix::unlock()\n");
    return sys::SyncFactoryIrix().unsetLock(*this);
}

#endif // __sgi && _REENTRANT && !__POSIX

