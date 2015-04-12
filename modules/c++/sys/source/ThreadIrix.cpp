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
#include "sys/ThreadIrix.h"

// void sys::ThreadIrix::start()
// {
//     sys::SyncFactoryIrix()->startThread(*this);
// }
// bool sys::ThreadIrix::kill()
// {
//     return sys::SyncFactoryIrix()->killThread(*this);
// }

bool sys::ThreadIrix::join()
{
    dbg_printf("Joining on thread\n");
    int lStatus;
    do
    {
        waitpid(mNative, &lStatus, 0);
        dbg_printf("Waiting on thread [%d]\n", mNative);
    }
    while ( WIFEXITED(lStatus) == 0 );
    dbg_printf("Done joining on thread\n");
    return true;
}

void sys::ThreadIrix::yield()
{
    dbg_printf("ThreadIrix::yield()\n");
    sginap(0);
}
#endif

