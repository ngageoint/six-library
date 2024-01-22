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


#ifndef CODA_OSS_sys_Thread_h_INCLUDED_
#define CODA_OSS_sys_Thread_h_INCLUDED_
#pragma once

#include "sys/Conf.h"
/**
 *  \file
 *  \brief Include the right thread.
 *
 *  This file will auto-select the thread of choice,
 *  if one is to be defined.
 *  \note We need to change the windows part to check _MT
 *  because that is how it determines reentrance!
 *
 */

#if defined(_WIN32)
#include "sys/ThreadWin32.h"
namespace sys
{
using Thread = ThreadWin32;
}

#elif defined(CODA_OSS_POSIX_SOURCE)
#include "sys/ThreadPosix.h"
namespace sys
{
using Thread = ThreadPosix;
}

#else
#error "Which thread package?"
#endif

#endif  // CODA_OSS_sys_Thread_h_INCLUDED_

