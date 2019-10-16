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

#ifndef __SYS_SCOPED_CPU_AFFINITY_UNIX_H__
#define __SYS_SCOPED_CPU_AFFINITY_UNIX_H__

#include <config/coda_oss_config.h>

#if !defined(WIN32)

#include <memory>
#include <sched.h>
#include <string>

namespace sys
{
/*!
 * \class ScopedCPUMaskUnix
 * \brief Class for generating a CPU mask on Unix
 *
 * ScopedCPUMaskUnix handles the creation/destruction of a dynamically
 * created cpu_set_t on Unix systems. This allows for the creation of
 * CPU masks greater than the number of CPUs reported by CPU_SETSIZE.
 */
class ScopedCPUMaskUnix
{
public:
    /*!
     * Constructor which automatically creates a mask that can hold
     * at least the number of online CPUs
     */
    ScopedCPUMaskUnix();

    /*!
     * Constructor for a given number of CPUs. Note that the resulting
     * CPU set may be larger, as the mask must be able to hold at least
     * CPU_SETSIZE number of CPUs.
     */
    ScopedCPUMaskUnix(int numCPUs);

    /*!
     * Destructor
     */
    virtual ~ScopedCPUMaskUnix();

    /*!
     * \returns a 0-1 string representation for the CPU mask,
     *          where 1s represent a usable CPU and 0 an inactive CPU
     */
    std::string toString() const;

    //! \returns a const cpu_set_t* mask
    const cpu_set_t* getMask() const
    {
        return mMask;
    }

    //! \returns a mutable cpu_set_t* mask
    cpu_set_t* getMask()
    {
        return mMask;
    }

    //! \returns the size of the CPU set in bytes
    size_t getSize() const
    {
        return mSize;
    }

    //! \returns the number of online CPUs
    static int getNumOnlineCPUs();

private:
    void initialize(int numCPUs);

protected:
    size_t mSize;
    cpu_set_t* mMask;
};

/*!
 * \class ScopedCPUAffinityUnix
 * \brief Class for generating a CPU mask on Unix that corresponds
 *        to the current CPU affinity
 *
 * ScopedCPUAffinityUnix handles the creation/destruction of a dynamically
 * created cpu_set_t on Unix systems that represents the CPU affinity for
 * the current PID.
 */
class ScopedCPUAffinityUnix : public ScopedCPUMaskUnix
{
public:
    /*!
     * Constructor that obtains the CPU affinity mask for the current
     * process.
     *
     * Note that if called from inside a thread, the CPU affinity for that
     * thread is generated.
     */
    ScopedCPUAffinityUnix();
};
}

#endif
#endif
