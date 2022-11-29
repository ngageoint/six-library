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
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 26408) // Avoid malloc() and free(), prefer the nothrow version of new with delete (r.10).
#endif
nitf_IOInterface* MemoryIO::create(void* buffer,
                                   size_t size,
                                   bool adopt)
{
    nitf_Error error{};
    nitf_IOInterface* const ioInterface = nitf_BufferAdapter_construct(
            static_cast<char*>(buffer), size, adopt, &error);

    if (!ioInterface)
    {
        if (adopt)
        {
            // It's our job to free this now
            NRT_FREE(buffer);
        }
        throw nitf::NITFException(&error);
    }

    return ioInterface;
}

MemoryIO::MemoryIO(size_t capacity) :
    IOInterface(create(NRT_MALLOC(capacity), capacity, true))
{
    // NOTE: We are telling the C layer to adopt this memory which means it
    //       will use NRT_FREE() to deallocate it.  So, we must allocate with
    //       NRT_MALLOC().
    setManaged(false);
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

MemoryIO::MemoryIO(void* buffer, size_t size, bool adopt)
  : IOInterface(create(buffer, size, adopt))
{
    setManaged(false);
}
}
