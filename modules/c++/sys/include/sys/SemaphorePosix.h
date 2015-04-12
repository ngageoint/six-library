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


#ifndef __SYS_SEMAPHORE_POSIX_H__
#define __SYS_SEMAPHORE_POSIX_H__
#if defined(__POSIX) && defined(_REENTRANT) && !defined(__APPLE_CC__)

#include "sys/SemaphoreInterface.h"

#if defined(__APPLE_CC_H__)
#  include <sys/semaphore.h>
#else
#  include <semaphore.h>
#endif

namespace sys
{
//    typedef ::sem_t sem_t;
class SemaphorePosix : public SemaphoreInterface<sem_t>
{
public:
    SemaphorePosix(unsigned int count = 0);
    virtual ~SemaphorePosix();
    bool wait();
    bool signal();
};
}

#endif
#endif
