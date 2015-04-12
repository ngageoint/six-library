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


#if !defined(WIN32)
#include "sys/ProcessUnix.h"
#include <stdlib.h>

void sys::ProcessUnix::start()
{
    pid_t lPid = fork();
    if (lPid == sys::ProcessUnix::PROCESS_CREATE_FAILED)
    {
        dbg_printf("Invalid pid!\n");
        exit(EXIT_FAILURE);
    }
    if (lPid == sys::ProcessUnix::THE_CHILD)
    {
        // Run the runnable that we pulled off the queue
        mTarget->run();
        dbg_printf("Call succeeded\n");
        exit(EXIT_SUCCESS);
    }
    else // lPid == THE_PARENT
    {
        dbg_printf("In parent and child pid is: %d\n", lPid);
        mChildProcessID = lPid;
        return ;
    }

}


void sys::ProcessUnix::waitFor()
{
    dbg_printf("Waiting\n");
    int status;
    //pid_t pid = wait(&status);
    //assert(pid == mChildProcessID);
    int options = 0;
    int whatExited = waitpid(mChildProcessID, &status, options);
    if (WIFEXITED(status))
    {
        dbg_printf("Exited normally.\n");
    }
    if (WIFSIGNALED(status))
    {
        dbg_printf("Uncaught signal.\n");
    }
    if (WIFSTOPPED(status))
    {
        dbg_printf("Stopped.\n");
    }
    dbg_printf("Finished waiting on pid: %d\n", mChildProcessID);
    assert(whatExited == mChildProcessID);
}
#endif
