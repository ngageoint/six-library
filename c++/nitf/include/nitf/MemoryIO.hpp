/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_MEMORY_IO_HPP__
#define __NITF_MEMORY_IO_HPP__

#include "nitf/NITFException.hpp"
#include "nitf/System.hpp"
#include "nitf/IOInterface.hpp"

/*!
 * \file MemoryIO.hpp
 * \brief Contains wrapper implementation for BufferAdapter
 */

namespace nitf
{

/*!
 *  \class MemoryIO
 *  \brief The C++ wrapper of the nitf_BufferAdapter
 */
class MemoryIO : public IOInterface
{

private:
    void create(char *buffer, nitf::Off size, bool adopt = false)
    {
        nitf_IOInterface* interface = nitf_BufferAdapter_construct(
                buffer, static_cast<size_t>(size), adopt, &error);

        if (!interface)
            throw nitf::NITFException(&error);
        setNative(interface);
        setManaged(false);
    }

public:

    MemoryIO(nitf::Off capacity) throw(nitf::NITFException)
    {
        char* buffer = new char[static_cast<size_t>(capacity)];

        create(buffer, capacity, true);
    }

    MemoryIO(char *buffer, nitf::Off size, bool adopt = false) 
            throw(nitf::NITFException)
    {
        create(buffer, size, adopt);
    }

    ~MemoryIO() {}
};

}
#endif
