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


#ifndef __MT_ABSTRACT_CPU_AFFINITY_INITIALIZER_H__
#define __MT_ABSTRACT_CPU_AFFINITY_INITIALIZER_H__

#include <mt/AbstractCPUAffinityThreadInitializer.h>

namespace mt
{
/*!
 * \class AbstractCPUAffinityInitializer
 * \brief Abstract class for providing thread-level affinity initializers.
 *        Should be used by the primary thread to generate data for
 *        child threads.
 */
class AbstractCPUAffinityInitializer
{
public:
    virtual ~AbstractCPUAffinityInitializer() {}

    /*!
     * \returns a new thread initializer. In general, this should return
     *          a different affinity initializer each time it is called.
     */
    std::auto_ptr<AbstractCPUAffinityThreadInitializer> newThreadInitializer()
    {
        return std::auto_ptr<AbstractCPUAffinityThreadInitializer>(
                newThreadInitializerImpl());
    }

private:
    // To allow for covariant auto_ptrs, this private function can be
    // implemented in derived classes to return a raw, unmanaged pointer
    // with the override having a covariant return type.
    // Using name hiding, we can define newThreadInitializer() implementations
    // that wrap newThreadInitializerImpl() in the appropriate derived
    // class return type. This should be done in all derived classes.
    virtual AbstractCPUAffinityThreadInitializer* newThreadInitializerImpl() = 0;
};
}

#endif
