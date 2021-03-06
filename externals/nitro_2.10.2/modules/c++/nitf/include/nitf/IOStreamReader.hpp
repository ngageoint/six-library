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
struct IOStreamReader /*final*/ : public CustomIO // no "final", SWIG doesn't like it
{
    /*
     *  \func Constructor
     *  \brief Sets up the stream reader from a seekable input stream.
     *
     *  \param stream The stream to use for reading. The stream must remain
     *         in scope throughout the lifetime of the IOStreamReader.
     */
    IOStreamReader(io::SeekableInputStream& stream);

    IOStreamReader(const IOStreamReader&) = delete;
    IOStreamReader& operator=(const IOStreamReader&) = delete;

private:
    void readImpl(void* buffer, size_t size) override;

    void writeImpl(const void* buffer, size_t size) override;

    bool canSeekImpl() const override;

    nitf::Off seekImpl(nitf::Off offset, int whence) override;

    nitf::Off tellImpl() const override;

    nitf::Off getSizeImpl() const override;

    int getModeImpl() const override;

    void closeImpl() override;

    io::SeekableInputStream& mStream;
};
}

#endif
