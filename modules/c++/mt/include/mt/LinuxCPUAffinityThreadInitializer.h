/* =========================================================================
 * This file is part of mt-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * mt-c++ is free software; you can redistribute it and/or modify
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


#ifndef __MT_LINUX_CPU_AFFINITY_THREAD_INITIALIZER_H__
#define __MT_LINUX_CPU_AFFINITY_THREAD_INITIALIZER_H__

#if !defined(__APPLE_CC__)
#if defined(__linux) || defined(__linux__)

#include <sched.h>
#include <sys/types.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

#include <import/sys.h>
#include "mt/CPUAffinityThreadInitializer.h"

namespace mt
{
    class LinuxCPUAffinityThreadInitializer : public mt::CPUAffinityThreadInitializer
    {	
	cpu_set_t mCPU;
    public:
	LinuxCPUAffinityThreadInitializer(const cpu_set_t& cpu);
	void initialize();
    };
}
    

#endif
#endif
#endif
