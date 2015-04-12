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


#ifndef __SYS_RUNTIME_PROCESS_H__
#define __SYS_RUNTIME_PROCESS_H__


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

#include "sys/ProcessInterface.h"

#if defined(WIN32)
#  include "sys/ProcessWin32.h"
namespace sys
{
typedef ProcessWin32 Process;
}
#
#else
#  include "sys/ProcessUnix.h"
namespace sys
{
typedef ProcessUnix Process;
}
#endif


#endif
