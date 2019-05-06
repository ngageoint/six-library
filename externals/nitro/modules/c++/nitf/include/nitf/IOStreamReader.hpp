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

#ifndef __NITF_IO_STREAM_READER_H__
#define __NITF_IO_STREAM_READER_H__

#include <nitf/CustomIO.hpp>
#include <io/SeekableStreams.h>

namespace nitf
{
/*
 *  \class IOStreamReader
 *  \brief Adapter class that takes in any io::SeekableInputStream and creates
 *         an interface that usable by NITRO.
 */
class IOStreamReader : public CustomIO
{
public:
    /*
     *  \func Constructor
     *  \brief Sets up the stream reader from a seekable input stream.
     *
     *  \param stream The stream to use for reading. The stream must remain
     *         in scope throughout the lifetime of the IOStreamReader.
     */
    IOStreamReader(io::SeekableInputStream& stream);

private:
    void readImpl(void* buffer, size_t size);

    void writeImpl(const void* buffer, size_t size);

    bool canSeekImpl() const;

    nitf::Off seekImpl(nitf::Off offset, int whence);

    nitf::Off tellImpl() const;

    nitf::Off getSizeImpl() const;

    int getModeImpl() const;

    void closeImpl();

    io::SeekableInputStream& mStream;
};
}

#endif
