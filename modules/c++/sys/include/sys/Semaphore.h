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


#ifndef __SYS_SEMAPHORE_H__
#define __SYS_SEMAPHORE_H__ 
/**
 *  \file 
 *  \brief Include the right semaphore.
 *
 *  This file will auto-select the semaphore of choice,
 *  if one is to be defined.  
 *  \note We need to change the windows part to check _MT
 *  because that is how it determines reentrance!
 *
 */
#  if defined(_REENTRANT)

#    if defined(USE_NSPR_THREADS)
#        include "sys/SemaphoreNSPR.h"
namespace sys
{
typedef SemaphoreNSPR Semaphore;
}
// If they explicitly want posix
#    elif defined(__POSIX) && !defined(__APPLE_CC__)
#        include "sys/SemaphorePosix.h"
namespace sys
{
typedef SemaphorePosix Semaphore;
}
#    elif defined(WIN32)
#        include "sys/SemaphoreWin32.h"
namespace sys
{
typedef SemaphoreWin32 Semaphore;
}
#    elif defined(__sun) && !defined(__POSIX)
#        include "sys/SemaphoreSolaris.h"
namespace sys
{
typedef SemaphoreSolaris Semaphore;
}
#    elif defined(__sgi) && !defined(__POSIX)
#        include "sys/SemaphoreIrix.h"
namespace sys
{
typedef SemaphoreIrix Semaphore;
}
#    elif defined(__APPLE_CC__)
typedef int Semaphore;
#    else
#        include "sys/SemaphorePosix.h"
namespace sys
{
typedef SemaphorePosix Semaphore;
}
#    endif // Which thread package?

#  endif // Are we reentrant?

#endif // End of header
