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


#if defined(WIN32)

#include "sys/Err.h"
#include "sys/Exec.h"

int sys::ExecPipe::closePipe()
{
    if (!mOutStream)
    {
        throw except::IOException(
            Ctxt("The stream is already closed"));
    }

    // in case it fails
    FILE* tmp = mOutStream;
    mOutStream = NULL;

    const int exitStatus  = pclose(tmp);
    if (exitStatus == -1)
    {
        sys::SocketErr err;
        throw except::IOException(
                Ctxt("Failure while closing stream to child process: " + 
                     err.toString()));

    }

    return exitStatus;
}

#endif
