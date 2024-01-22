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


#ifndef __MT_CPU_AFFINITY_INITIALIZER_LINUX_H__
#define __MT_CPU_AFFINITY_INITIALIZER_LINUX_H__

#if !defined(__APPLE_CC__)
#if defined(__linux) || defined(__linux__)

#include <memory>
#include <vector>

#include <sys/ScopedCPUAffinityUnix.h>
#include <mt/AbstractCPUAffinityInitializer.h>
#include <mt/CPUAffinityThreadInitializerLinux.h>
#include <mem/SharedPtr.h>

namespace mt
{
struct AbstractNextCPUProviderLinux
{
    virtual std::unique_ptr<const sys::ScopedCPUMaskUnix> nextCPU() = 0;
    virtual ~AbstractNextCPUProviderLinux() {}
};

/*!
 * \class CPUAffinityInitializerLinux
 * \brief Linux-specific class for providing thread-level affinity initializers.
 */
class CPUAffinityInitializerLinux : public AbstractCPUAffinityInitializer
{
public:

    /*!
     * Constructor that uses the available CPUs (possibly restricted
     * via taskset) to set affinities
     */
    CPUAffinityInitializerLinux();

    /*!
     * Constructor that uses all online CPUs to incrementally update
     * to the next CPU, starting from 'initialOffset'.
     *
     * \param initialOffset Initial CPU to start using when pinning threads
     */
    CPUAffinityInitializerLinux(int initialOffset);

    /*!
     * \throws if there are no more available CPUs to bind to
     * \returns a new CPUAffinityInitializerLinux for the next available
     *          CPU that can be bound to.
     */
    std::unique_ptr<CPUAffinityThreadInitializerLinux> newThreadInitializer()
    {
        return std::unique_ptr<CPUAffinityThreadInitializerLinux>(
                newThreadInitializerImpl());
    }

private:
    CPUAffinityThreadInitializerLinux* newThreadInitializerImpl() override
    {
        return new CPUAffinityThreadInitializerLinux(mCPUProvider->nextCPU());
    }

    std::unique_ptr<AbstractNextCPUProviderLinux> mCPUProvider;
};
}

#endif
#endif
#endif

