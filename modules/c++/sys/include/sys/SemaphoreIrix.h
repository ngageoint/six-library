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


#ifndef __SYS_SEMAPHORE_IRIX_H__
#define __SYS_SEMAPHORE_IRIX_H__

#if defined(__sgi) && defined(_REENTRANT) && !defined(__POSIX)

#include <ulocks.h>
#include "sys/SyncFactoryIrix.h"
#include "sys/SemaphoreInterface.h"

namespace sys
{
class SemaphoreIrix : public SemaphoreInterface<usema_t*>
{
public:
    SemaphoreIrix(unsigned int count = 0);
    virtual ~SemaphoreIrix();
    bool wait();
    bool signal();
};
}

#endif
#endif
