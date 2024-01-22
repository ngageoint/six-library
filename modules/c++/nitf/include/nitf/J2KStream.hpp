/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2017, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#ifndef NITF_J2KStream_hpp_INCLUDED_
#define NITF_J2KStream_hpp_INCLUDED_

#include <stddef.h>

#include "j2k/TileWriter.h"

#include "nitf/exports.hpp"

namespace j2k
{
    /*!
     * \enum StreamType
     *
     * Determines whether openjpeg will use the stream for writing out or
     * reading in compressed data.
     */
    enum class StreamType
    {
        OUTPUT = 0,
        INPUT
    };

    /*!
     * \class Stream
     * \desc RAII wrapper around opj_stream_t.
     */
    struct NITRO_NITFCPP_API Stream final
    {
        /*!
         * Constructor
         *
         * \param streamType The type of stream - OUTPUT creates an output stream
         * to write j2k compressed data to during compression and INPUT creates
         * an input stream to read j2k compressed data from during decompression.
         *
         * \param chunkSize Size of the internal buffer used by openjpeg to
         * write compressed data to or to read compressed data from.
         */
        Stream(StreamType streamType,
            size_t chunkSize = NITRO_J2K_STREAM_CHUNK_SIZE);

        Stream(const Stream&) = delete;
        Stream& operator=(const Stream&) = delete;
        Stream(Stream&&) = default;
        Stream& operator=(Stream&&) = default;
        ~Stream();

        /*!
         * \return The openjpeg stream.
         */
        j2k_stream_t* getNative() const noexcept
        {
            return mStream;
        }

    private:
        //! The openjpeg output/input stream.
        j2k_stream_t* mStream = nullptr;
    };
}

#endif

