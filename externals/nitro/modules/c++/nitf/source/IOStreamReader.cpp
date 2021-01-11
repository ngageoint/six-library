/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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

#include <nitf/IOStreamReader.hpp>
#include <except/Exception.h>

#include "nitf/cstddef.h"

namespace nitf
{
IOStreamReader::IOStreamReader(io::SeekableInputStream& stream) :
    mStream(stream)
{
}

void IOStreamReader::readImpl(void* buffer, size_t size)
{
    mStream.read(static_cast<nitf::byte*>(buffer), size);
}

void IOStreamReader::writeImpl(const void* , size_t)
{
    throw except::Exception(
            Ctxt("IOStreamReader cannot perform writes. "
                 "It is a read-only handle."));
}

bool IOStreamReader::canSeekImpl() const
{
    return true;
}

nitf::Off IOStreamReader::seekImpl(nitf::Off offset, int whence)
{
    // This whence does not match io::Seekable::Whence
    // We need to perform a mapping to the correct values.
    io::Seekable::Whence ioWhence;
    switch (whence)
    {
    case SEEK_SET:
        ioWhence = io::Seekable::START;
        break;
    case SEEK_CUR:
        ioWhence = io::Seekable::CURRENT;
        break;
    case SEEK_END:
        ioWhence = io::Seekable::END;
        break;
    default:
        throw except::Exception(
                Ctxt("Unknown whence value when seeking IOStreamReader: " +
                     std::to_string(whence)));
    }

    return mStream.seek(offset, ioWhence);
}

nitf::Off IOStreamReader::tellImpl() const
{
    return mStream.tell();
}

nitf::Off IOStreamReader::getSizeImpl() const
{
    // There's no way to ask a stream what its total size is, so need to seek
    // to the end to find out
    const nitf::Off origPos = mStream.tell();
    const nitf::Off size = mStream.seek(0, io::Seekable::END);
    mStream.seek(origPos, io::Seekable::START);
    return size;
}

int IOStreamReader::getModeImpl() const
{
    return NITF_ACCESS_READONLY;
}

void IOStreamReader::closeImpl()
{
}
}
