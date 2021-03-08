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


#if defined(__sun)
#include "sys/ThreadSolaris.h"
#include "sys/Dbg.h"


void sys::ThreadSolaris::start()
{

    long flags = 0;
    if (getLevel() == KERNEL_LEVEL )
        flags |= THR_BOUND;

    dbg_printf("Starting thread and is kernel? [%d]\n",
               getLevel() == KERNEL_LEVEL);
    // \todo Kernel level

    int p = thr_create(NULL,
                       0,
                       __sys_ThreadSolaris_start,
                       this,
                       flags,
                       &mNative);
    if (p != 0)
    {
        std::string message = "thr_create failed: ";
        if ( p == EAGAIN ) message += "EAGAIN";
        else if ( p == EINVAL ) message += "EINVAL";
        else if ( p == ENOMEM ) message += "ENOMEM";
        throw sys::SystemException(message);
    }
}
extern "C" void *__sys_ThreadSolaris_start(void *v)
{
    dbg_printf("Beginning __start()\n");
    STANDARD_START_CALL(ThreadSolaris, v);
    dbg_printf("Finishing __start()\n");
    return NULL;

}

void sys::ThreadSolaris::kill()
{
    dbg_printf("Killing thread\n");
    if ( thr_kill(mNative, SIGKILL) != 0)
    {
        throw sys::SystemException("thr_kill()");
    }
    setIsRunning(false);
}

void sys::ThreadSolaris::join()
{
    dbg_printf("Joining thread\n");
    if (::thr_join(mNative, NULL, NULL) != 0 )
        throw sys::SystemException("thr_join()");
}

void sys::ThreadSolaris::yield()
{
    dbg_printf("Yielding thread\n");
    ::thr_yield();
}

#endif
