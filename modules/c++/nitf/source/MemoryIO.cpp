/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <nitf/MemoryIO.hpp>

namespace nitf
{
nitf_IOInterface* MemoryIO::create(void* buffer,
                                   size_t size,
                                   bool adopt) throw(nitf::NITFException)
{
    nitf_Error error;
    nitf_IOInterface* const interface = nitf_BufferAdapter_construct(
            static_cast<char*>(buffer), size, adopt, &error);

    if (!interface)
    {
        if (adopt)
        {
            // It's our job to free this now
            NRT_FREE(buffer);
        }
        throw nitf::NITFException(&error);
    }

    return interface;
}

MemoryIO::MemoryIO(size_t capacity) throw(nitf::NITFException) :
    IOInterface(create(NRT_MALLOC(capacity), capacity, true))
{
    // NOTE: We are telling the C layer to adopt this memory which means it
    //       will use NRT_FREE() to deallocate it.  So, we must allocate with
    //       NRT_MALLOC().
    setManaged(false);
}

MemoryIO::MemoryIO(void* buffer, size_t size, bool adopt)
        throw(nitf::NITFException) :
    IOInterface(create(buffer, size, adopt))
{
    setManaged(false);
}
}
