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


#ifndef __MT_LINUX_CPU_AFFINITY_INITIALIZER_H__
#define __MT_LINUX_CPU_AFFINITY_INITIALIZER_H__

#if !defined(__APPLE_CC__)
#if defined(__linux) || defined(__linux__)


#include "mt/CPUAffinityInitializer.h"
#include "mt/LinuxCPUAffinityThreadInitializer.h"

namespace mt
{
    class LinuxCPUAffinityInitializer : public mt::CPUAffinityInitializer
    {
	int mNextCPU;
	cpu_set_t nextCPU();
    public:
	LinuxCPUAffinityInitializer(int initialOffset) : 
	    mNextCPU(initialOffset) {}
	
	~LinuxCPUAffinityInitializer() {}
	
	LinuxCPUAffinityThreadInitializer* newThreadInitializer()
	{
	    return new LinuxCPUAffinityThreadInitializer(nextCPU());
	}
	
    };
}

#endif
#endif
#endif

