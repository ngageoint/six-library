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

void sys::SemaphoreIrix::wait()
{
    dbg_printf("SemaphoreIrix::wait()\n");
    if (!sys::SyncFactoryIrix().waitSemaphore(*this))
        throw SystemException("Semaphore wait failed");

}

void sys::SemaphoreIrix::signal()
{
    dbg_printf("SemaphoreIrix::signal()\n");
    if (!sys::SyncFactoryIrix().signalSemaphore(*this))
        throw SystemException("Semaphore signal failed");
}

usema_t*& sys::SemaphoreIrix::getNative()
{
    // We don't actually use this, but SemaphoreIrix uses usema_t* as its template..
    return (usema_t*) NULL;
}

#endif // __sgi

