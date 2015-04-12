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


#ifndef __SYS_RUNTIME_PROCESS_INTERFACE_H__
#define __SYS_RUNTIME_PROCESS_INTERFACE_H__


/*!
 *  \file
 *  \brief Runtime, system-independent process creation API
 *
 *  When it comes to multitasking, we almost all prefer threads to
 *  heritage process calls.  However, threads and processes are almost
 *  never equivalent.  Sometimes we need a process.  Here we define
 *  a simple API for process creation in a system-independent manner
 *
 */

#include "sys/Dbg.h"
#include "sys/Runnable.h"
#include <vector>

namespace sys
{

template <typename Pid_T> class ProcessInterface : public sys::Runnable
{
public:
    enum { THE_CHILD = 0 };
    enum { PROCESS_CREATE_FAILED = -1 };
    ProcessInterface()
    {
        mTarget = this;
    }
    ProcessInterface(sys::Runnable* target) : mTarget(target)
    {}

    virtual ~ProcessInterface()
    {}

    virtual void start() = 0;
    virtual void waitFor() = 0;
    virtual void run() = 0;

protected:
    Pid_T mChildProcessID;
    sys::Runnable* mTarget;
};
}

#endif
