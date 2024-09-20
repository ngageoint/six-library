/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2017, MDA Information Systems LLC
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

#include "nitf/J2KStream.hpp"

#include <sstream>
#include <stdexcept>

#include "nitf/NITFException.hpp"

j2k::Stream::Stream(j2k::StreamType streamType, size_t chunkSize)
{
    const auto isInputStream = streamType == j2k::StreamType::INPUT;

    mStream = j2k_stream_create(chunkSize, isInputStream);
    if (!mStream)
    {
        std::ostringstream os;
        os << "Failed creating an openjpeg stream with a chunk size of " << chunkSize << " bytes.";
        throw except::Exception(Ctxt(os));
    }
}

j2k::Stream::~Stream()
{
    j2k_stream_destroy(mStream);
}