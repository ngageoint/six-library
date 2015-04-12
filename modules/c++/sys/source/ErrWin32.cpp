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

#include <WinSock.h>
#include "sys/Err.h"

int sys::Err::getLast() const
{
    return GetLastError();
}

std::string sys::Err::toString() const
{
    LPTSTR buffer;
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                      mErrId, 0,
                      (LPTSTR)&buffer, 0, NULL) == 0)
    {
        return std::string("Unknown error code");
    }
    
    std::string error(buffer);
    LocalFree(buffer);
    return error;
}

int sys::SocketErr::getLast() const
{
    return WSAGetLastError();
}

#endif
