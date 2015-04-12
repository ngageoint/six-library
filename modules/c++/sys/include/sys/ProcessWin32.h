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


#ifndef __SYS_PROCESS_WIN32_H__
#define __SYS_PROCESS_WIN32_H__

#if defined(WIN32)

#include "sys/ProcessInterface.h"
#include "sys/Thread.h"

namespace sys
{
class ProcessWin32 : public ProcessInterface< Thread* >
{
public:
    ProcessWin32()
    {}
    ProcessWin32(Runnable* target) : ProcessInterface< Thread* >(target)
    {}
    virtual ~ProcessWin32()
    {
        delete mChildProcessID;
    }
    void start();
    void waitFor();
};

}
#endif
#endif
