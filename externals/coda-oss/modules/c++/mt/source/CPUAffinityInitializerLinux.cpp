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

#include <sstream>

#include <sys/OS.h>
#include <sys/Conf.h>
#include <except/Exception.h>
#include <mt/CPUAffinityInitializerLinux.h>

namespace
{
std::vector<int> mergeAvailableCPUs()
{
    std::vector<int> physicalCPUs;
    std::vector<int> htCPUs;
    sys::OS().getAvailableCPUs(physicalCPUs, htCPUs);

    // Merge physical CPUs first so that
    // traversal visits them before hyperthreaded CPUs
    std::vector<int> mergedCPUs;
    mergedCPUs.reserve(physicalCPUs.size() + htCPUs.size());
    mergedCPUs.insert(mergedCPUs.end(), physicalCPUs.begin(), physicalCPUs.end());
    mergedCPUs.insert(mergedCPUs.end(), htCPUs.begin(), htCPUs.end());
    return mergedCPUs;
}
}

namespace mt
{
struct AvailableCPUProvider final : public AbstractNextCPUProviderLinux
{
    AvailableCPUProvider() :
        mCPUs(mergeAvailableCPUs()),
        mNextCPUIndex(0)
    {
    }

    virtual std::unique_ptr<const sys::ScopedCPUMaskUnix> nextCPU() override
    {
        if (mNextCPUIndex >= mCPUs.size())
        {
            std::ostringstream msg;
            msg << "No more CPUs available (size = " << mCPUs.size() << ")";
            throw except::Exception(Ctxt(msg.str()));
        }

        std::unique_ptr<const sys::ScopedCPUMaskUnix> mask(
                new sys::ScopedCPUMaskUnix());
        CPU_SET_S(mCPUs.at(mNextCPUIndex++), mask->getSize(), mask->getMask());
        return mask;
    }

private:
    const std::vector<int> mCPUs;
    size_t mNextCPUIndex;
};

struct OffsetCPUProvider final : public AbstractNextCPUProviderLinux
{
    OffsetCPUProvider(int initialOffset) :
        mNextCPU(initialOffset)
    {
    }

    virtual std::unique_ptr<const sys::ScopedCPUMaskUnix> nextCPU() override
    {
        std::unique_ptr<const sys::ScopedCPUMaskUnix> mask(new sys::ScopedCPUMaskUnix());
        CPU_SET_S(mNextCPU++, mask->getSize(), mask->getMask());
        return mask;
    }

private:
    int mNextCPU;
};

CPUAffinityInitializerLinux::CPUAffinityInitializerLinux() :
    mCPUProvider(new AvailableCPUProvider())
{
}

CPUAffinityInitializerLinux::CPUAffinityInitializerLinux(int initialOffset) :
    mCPUProvider(new OffsetCPUProvider(initialOffset))
{
}
}

#endif
#endif
