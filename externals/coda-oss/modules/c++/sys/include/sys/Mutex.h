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


#ifndef __SYS_MUTEX_H__
#define __SYS_MUTEX_H__ 
/**
 *  \file 
 *  \brief Include the right mutex.
 *
 *  This file will auto-select the mutex of choice,
 *  if one is to be defined.  
 *  \note We need to change the windows part to check _MT
 *  because that is how it determines reentrance!
 *
 */

#    if defined(USE_NSPR_THREADS)
#        include "sys/MutexNSPR.h"
namespace sys
{
typedef MutexNSPR Mutex;
}
#    elif (defined(WIN32) || defined(_WIN32))
#        include "sys/MutexWin32.h"
namespace sys
{
typedef MutexWin32 Mutex;
}

/* #    elif defined(USE_BOOST) */
/* #        include "MutexBoost.h" */
/*          typedef MutexBoost Mutex; */
#    elif defined(__sun)
#        include "sys/MutexSolaris.h"
namespace sys
{
typedef MutexSolaris Mutex;
}
#    elif defined(__sgi)
#        include "sys/MutexIrix.h"
namespace sys
{
typedef MutexIrix Mutex;
}
// Give 'em POSIX
#    else
#        include "sys/MutexPosix.h"
namespace sys
{
typedef MutexPosix Mutex;
}
#    endif // Which thread package?

#endif // End of header
