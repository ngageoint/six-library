/* =========================================================================
 * This file is part of mt-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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


#ifndef __MT_ABSTRACT_CPU_AFFINITY_THREAD_INITIALIZER_H__
#define __MT_ABSTRACT_CPU_AFFINITY_THREAD_INITIALIZER_H__

namespace mt
{
/*!
 * \class AbstractCPUAffinityThreadInitializer
 * \brief Abstract class for setting the CPU affinity of a thread
 */
class AbstractCPUAffinityThreadInitializer
{
public:
    AbstractCPUAffinityThreadInitializer() {}
    virtual ~AbstractCPUAffinityThreadInitializer() {}

    /*!
     * Set the CPU affinity of the currently running thread
     */
    virtual void initialize() = 0;
};
}
#endif
