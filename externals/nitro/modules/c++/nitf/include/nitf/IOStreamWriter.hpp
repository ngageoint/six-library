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

#ifndef __NITF_IO_STREAM_WRITER_H__
#define __NITF_IO_STREAM_WRITER_H__
#pragma once

#include <memory>

#include <nitf/CustomIO.hpp>
#include <io/SeekableStreams.h>

namespace nitf
{
/*
 *  \class IOStreamWriter
 *  \brief Adapter class that takes in any io::SeekableOutputStream and creates
 *         an interface that usable by NITRO.
 */
class IOStreamWriter : public CustomIO
{
public:
    /*
     *  \func Constructor
     *  \brief Sets up the stream writer from a seekable output stream.
     *
     *  \param stream The stream to use for writing
     */
    IOStreamWriter(std::shared_ptr<io::SeekableOutputStream> stream);

private:
    void readImpl(void* buffer, size_t size) override;

    void writeImpl(const void* buffer, size_t size) override;

    bool canSeekImpl() const noexcept override;

    nitf::Off seekImpl(nitf::Off offset, int whence) override;

    nitf::Off tellImpl() const override;

    nitf::Off getSizeImpl() const override;

    int getModeImpl() const noexcept override;

    void closeImpl() noexcept override;

    std::shared_ptr<io::SeekableOutputStream> mStream;
};
}

#endif
