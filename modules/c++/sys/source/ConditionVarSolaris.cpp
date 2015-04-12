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


#if defined(__sun) && defined(_REENTRANT) && !defined(__POSIX)
#include <thread.h>
#include <synch.h>
#include "sys/ConditionVarSolaris.h"

sys::ConditionVarSolaris::~ConditionVarSolaris()
{
    ::cond_destroy(&mNative);
}

bool sys::ConditionVarSolaris::signal()
{
    dbg_printf("Signaling condition\n");
    return (::cond_signal(&mNative) == 0);
}
bool sys::ConditionVarSolaris::wait()
{
    dbg_printf("Waiting on condition\n");
    return (::cond_wait(&mNative,
                        &(mMutex->getNative())) == 0);
}
// Implement this
bool sys::ConditionVarSolaris::wait(double seconds)
{
    dbg_printf("Timed waiting on condition [%f]\n", seconds);
    if ( seconds > 0 )
    {
        timestruc_t tout;
        tout.tv_sec = time(NULL) + (int)seconds;
        tout.tv_nsec = (int)((seconds - (int)(seconds)) * 1e9);
        return
            (::cond_timedwait(&mNative,
                              &(mMutex->getNative()),
                              &tout) == 0);
    }
    else return wait();
}
bool sys::ConditionVarSolaris::broadcast()
{
    dbg_printf("Broadcasting condition\n");
    return (::cond_broadcast(&mNative) == 0);
}

#endif

