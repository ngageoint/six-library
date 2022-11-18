/* =========================================================================
 * This file is part of mt-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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


#if !defined(__APPLE_CC__)
#if defined(__linux) || defined(__linux__)

#include <sched.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <sys/Conf.h>
#include <except/Exception.h>
#include <mem/SharedPtr.h>
#include <mt/CPUAffinityThreadInitializerLinux.h>

namespace mt
{
CPUAffinityThreadInitializerLinux::CPUAffinityThreadInitializerLinux(
        std::unique_ptr<const sys::ScopedCPUMaskUnix>&& cpu) :
    mCPU(std::move(cpu))
{
}
#if CODA_OSS_autoptr_is_std // std::auto_ptr removed in C++17
CPUAffinityThreadInitializerLinux::CPUAffinityThreadInitializerLinux(
        mem::auto_ptr<const sys::ScopedCPUMaskUnix> cpu) :
    CPUAffinityThreadInitializerLinux(std::unique_ptr<const sys::ScopedCPUMaskUnix>(cpu.release()))
{
}
#endif

void CPUAffinityThreadInitializerLinux::initialize()
{
    pid_t tid = syscall(SYS_gettid);
    if (::sched_setaffinity(tid, mCPU->getSize(), mCPU->getMask()) == -1)
    {
	   throw except::Exception(Ctxt("Failed setting processor affinity"));
    }
}
}

#endif
#endif
