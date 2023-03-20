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


#ifndef __MT_CPU_AFFINITY_THREAD_INITIALIZER_LINUX_H__
#define __MT_CPU_AFFINITY_THREAD_INITIALIZER_LINUX_H__

#if !defined(__APPLE_CC__)
#if defined(__linux) || defined(__linux__)

#include <memory>

#include <sys/ScopedCPUAffinityUnix.h>
#include <mt/AbstractCPUAffinityThreadInitializer.h>

namespace mt
{
/*!
 * \class CPUAffinityThreadInitializerLinux
 * \brief Linux-specific setting of the CPU affinity of a thread
 */
class CPUAffinityThreadInitializerLinux :
        public AbstractCPUAffinityThreadInitializer
{
public:

    /*!
     * Constructor
     *
     * \param cpu A ScopedCPUMaskUnix object corresponding to the
     *            affinity mask for the CPUs that this thread
     *            is allowed to bind to
     */
    CPUAffinityThreadInitializerLinux(
            std::unique_ptr<const sys::ScopedCPUMaskUnix>&& cpu);

    /*!
     * Attempt to bind to the affinity mask given during construction
     *
     * \throws if setting the thread affinity fails
     */
    virtual void initialize() override;

private:
    std::unique_ptr<const sys::ScopedCPUMaskUnix> mCPU;
};
}

#endif
#endif
#endif
