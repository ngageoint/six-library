/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#include <nitf/coda-oss.hpp>
#include <nitf/IOStreamWriter.hpp>

namespace nitf
{
IOStreamWriter::IOStreamWriter(mem::SharedPtr<io::SeekableOutputStream> stream) :
    mStream(stream)
{
}

void IOStreamWriter::readImpl(void* , size_t )
{
    throw except::Exception(
            Ctxt("IOStreamWriter cannot perform reads. "
                 "It is a write-only handle."));
}

void IOStreamWriter::writeImpl(const void* buffer, size_t size)
{
    mStream->write(static_cast<const std::byte*>(buffer), size);
}

bool IOStreamWriter::canSeekImpl() const noexcept
{
    return true;
}

nitf::Off IOStreamWriter::seekImpl(nitf::Off offset, int whence)
{
    // This whence does not match io::Seekable::Whence
    // We need to perform a mapping to the correct values.
    io::Seekable::Whence ioWhence = io::Seekable::START;
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
                Ctxt("Unknown whence value when seeking IOStreamWriter: " +
                     std::to_string(whence)));
    }

    return mStream->seek(offset, ioWhence);
}

nitf::Off IOStreamWriter::tellImpl() const
{
    return mStream->tell();
}

nitf::Off IOStreamWriter::getSizeImpl() const
{
    const nitf::Off currentLoc = mStream->tell();
    mStream->seek(0, io::Seekable::END);
    const nitf::Off size = mStream->tell();
    mStream->seek(currentLoc, io::Seekable::START);
    return size;
}

int IOStreamWriter::getModeImpl() const noexcept
{
    return NITF_ACCESS_WRITEONLY;
}

void IOStreamWriter::closeImpl() noexcept
{
}
}
