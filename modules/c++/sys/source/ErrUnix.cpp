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


#ifndef _WIN32

#include "sys/Err.h"

int sys::Err::getLast() const
{
    return errno;
}

std::string sys::Err::toString() const
{
    char *temp = strerror(mErrId);
    if (temp == nullptr)
        return std::string("");

    std::string stringError = temp;
    return stringError;
}

int sys::SocketErr::getLast() const
{
    return errno;
}

#endif

