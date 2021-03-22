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


#ifndef __MT_CPU_AFFINITY_INITIALIZER_WIN32_H__
#define __MT_CPU_AFFINITY_INITIALIZER_WIN32_H__

#if defined(WIN32) || defined(_WIN32)

#include <mt/AbstractCPUAffinityInitializer.h>
#include <mt/CPUAffinityThreadInitializerWin32.h>
#include <mem/SharedPtr.h>

namespace mt
{
/*!
 * \class CPUAffinityInitializerWin32
 * \brief Windows-specific class for providing thread-level affinity initializers.
 * \todo This is a stub implementation that doesn't do anything. Make this work.
 */
class CPUAffinityInitializerWin32 : public AbstractCPUAffinityInitializer
{
public:
    /*!
     * \todo Not yet implemented
     * \returns NULL
     */
    mem::auto_ptr<CPUAffinityThreadInitializerWin32> newThreadInitializer()
    {
        return mem::auto_ptr<CPUAffinityThreadInitializerWin32>(
                newThreadInitializerImpl());
    }

private:
    virtual CPUAffinityThreadInitializerWin32* newThreadInitializerImpl() override
    {
        return nullptr;
    }
};
}

#endif
#endif

