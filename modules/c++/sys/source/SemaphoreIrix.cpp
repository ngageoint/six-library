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
#include "sys/SemaphoreIrix.h"


sys::SemaphoreIrix::SemaphoreIrix(unsigned int count)
{
    if (!sys::SyncFactoryIrix().createSemaphore(*this, count))
        throw SystemException("Semaphore initialization failed");
}

sys::SemaphoreIrix::~SemaphoreIrix()
{
    dbg_printf("~SemaphoreIrix()\n");
    sys::SyncFactoryIrix().destroySemaphore(*this);
    dbg_printf("done.\n");
}

bool sys::SemaphoreIrix::wait()
{
    dbg_printf("SemaphoreIrix::wait()\n");
    return sys::SyncFactoryIrix().waitSemaphore(*this);

}

bool sys::SemaphoreIrix::signal(void)
{
    dbg_printf("SemaphoreIrix::signal()\n");
    return sys::SyncFactoryIrix().signalSemaphore(*this);
}

#endif // __sgi && _REENTRANT && !__POSIX

