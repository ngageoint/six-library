/* =========================================================================
 * This file is part of sys-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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

#include "config/coda_oss_config.h"

#if !defined(WIN32)

#include <errno.h>

#include <sys/Conf.h>
#include <except/Exception.h>

#include <sys/ScopedCPUAffinityUnix.h>

namespace sys
{
ScopedCPUMaskUnix::ScopedCPUMaskUnix()
{
    // The number of processors in the mask must be _at least_ the number of
    // CPU's representable by the kernel's internal mask. This is given by
    // the constant CPU_SETSIZE.
    const int numOnlineCPUs = ScopedCPUAffinityUnix::getNumOnlineCPUs();
    const int maxCPUs = std::max<int>(numOnlineCPUs, CPU_SETSIZE);
    initialize(maxCPUs);
}

ScopedCPUMaskUnix::ScopedCPUMaskUnix(int numCPUs)
{
    initialize(numCPUs);
}

void ScopedCPUMaskUnix::initialize(int numCPUs)
{
    mSize = CPU_ALLOC_SIZE(numCPUs);
    mMask = CPU_ALLOC(numCPUs);

    if (mMask == NULL)
    {
        std::ostringstream msg;
        msg << "Failed to allocate CPU mask for " << numCPUs << "CPUs";
        throw except::Exception(Ctxt(msg.str()));
    }

    CPU_ZERO_S(mSize, mMask);
}

ScopedCPUMaskUnix::~ScopedCPUMaskUnix()
{
    if (mMask != NULL)
    {
        CPU_FREE(mMask);
    }
}

std::string ScopedCPUMaskUnix::toString() const
{
    std::ostringstream str;
    const int numCPUs = getNumOnlineCPUs();
    for (int cpu = 0; cpu < numCPUs; ++cpu)
    {
        str << (CPU_ISSET_S(cpu, mSize, mMask) ? "1" : "0");
    }

    return str.str();
}

int ScopedCPUMaskUnix::getNumOnlineCPUs()
{
#ifdef _SC_NPROCESSORS_ONLN
    const int numOnlineCPUs = sysconf(_SC_NPROCESSORS_ONLN);
    if (numOnlineCPUs == -1)
    {
        throw except::Exception(Ctxt("Failed to get online CPU count"));
    }
    return numOnlineCPUs;
#else
throw except::NotImplementedException(Ctxt("Unable to get the number of CPUs"));
#endif
}

//-----------------------------------------------------------------------------

ScopedCPUAffinityUnix::ScopedCPUAffinityUnix() :
    ScopedCPUMaskUnix()
{
    if (sched_getaffinity(0, mSize, mMask) == -1)
    {
        std::ostringstream msg;
        msg << "Failed to get CPU affinity with"
            << " CPU_SETSIZE=" << CPU_SETSIZE << ","
            << " numOnlineCPUs=" << getNumOnlineCPUs() << ","
            << " alloc size=" << mSize << ".";
        switch (errno)
        {
        case EINVAL:
            msg << " Affinity mask smaller than kernel mask size.";
            break;
        case EFAULT:
            msg << " Invalid mask address.";
            break;
        case ESRCH:
            msg << " PID for current process is invalid.";
            break;
        default:
            msg << " Unknown cause.";
        }
        throw except::Exception(Ctxt(msg.str()));
    }
}
}

#endif
