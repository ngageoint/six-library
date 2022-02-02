/* =========================================================================
 * This file is part of io-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * io-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CODA_OSS_io_StringStream_h_INCLUDED_
#define CODA_OSS_io_StringStream_h_INCLUDED_
#pragma once

/*! \file StringStream.h
 *  \brief  A stream interface to the std::stringstream from C++ STL.
 *
 *  String streams are very useful.  They are even more useful with cafe 
 *  streaming capabilities.  The capabilities are added by making the class
 *  inherit from a stream -- a class that can pipe information back and
 *  forth to other streams.
 */

#include <sstream>

#include "gsl/gsl.h"
#include "io/BidirectionalStream.h"
#include "sys/Conf.h"
#include "io/SeekableStreams.h"
#include "coda_oss/string.h"
#include "str/Encoding.h"

namespace io
{
template<typename CharT>
struct StringStreamT final : public SeekableBidirectionalStream
{
    StringStreamT() = default;

    StringStreamT(const StringStreamT&) = delete;
    StringStreamT& operator=(const StringStreamT&) = delete;

    using stringstream = std::basic_stringstream<CharT>;

    /*!
     *  Returns the stringstream associated with this StringStream
     *  \return the stringstream
     */
    const stringstream& stream() const
    {
        return mData;
    }

    sys::Off_T tell()
    {
        return mData.tellg();
    }

    sys::Off_T seek(sys::Off_T offset, Whence whence)
    {
        std::ios::seekdir flags = std::ios::cur;
        switch (whence)
        {
        case START:
            flags = std::ios::beg;
            break;
        case END:
            flags = std::ios::end;
            break;
        case CURRENT:
        default:
            flags = std::ios::cur;
            break;
        }

        // off_t orig = tell();
        mData.seekg(offset, flags);
        return tell();
    }

    /*!
     *  Returns the available bytes to read from the stream
     *  \return the available bytes to read
     */
    sys::Off_T available()
    {
        const auto where = tell();

        mData.seekg(0, std::ios::end);
        const auto until = tell();

        mData.seekg(where, std::ios::beg);
        return (until - where);
    }

    using OutputStream::write;

    /*!
     *  Writes the bytes in data to the stream.
     *  \param buffer the data to write to the stream
     *  \param size the number of bytes to write to the stream
     */
    void write(const void* buffer, sys::Size_T size)
    {
        auto buffer_ = static_cast<const CharT*>(buffer);
        mData.write(buffer_, gsl::narrow<std::streamsize>(size));
    }

    //! Returns the internal std::stringstream
    stringstream& stream()
    {
        return mData;
    }

    void reset()
    {
        mData.str(std::basic_string<CharT>());
        // clear eof/errors/etc.
        mData.clear();
    }

private:
    /*!
     * Read up to len bytes of data from this buffer into an array
     * update the mark
     * \param buffer Buffer to read into
     * \param len The length to read
     * \throw IoException
     * \return  The number of bytes read
     */
    sys::SSize_T readImpl(void* buffer, size_t len_) override
    {
        const auto maxSize = available();
        if (maxSize <= 0)
            return io::InputStream::IS_END;
            
        auto len = gsl::narrow<sys::Off_T>(len_);
        if (maxSize < len)
            len = maxSize;
            
        if (len <= 0)
            return 0;
            
        auto buffer_ = static_cast<CharT*>(buffer);
        mData.read(buffer_, gsl::narrow<std::streamsize>(len));
            
        // Could be problem if streams are broken alternately could
        // return gcount in else case above
        return gsl::narrow<sys::SSize_T>(len);
    }

    stringstream mData{stringstream::in | stringstream::out | stringstream::binary};
};

using StringStream = StringStreamT<std::string::value_type>;
using U8StringStream = StringStreamT<coda_oss::u8string::value_type>;
using W1252StringStream = StringStreamT<str::W1252string::value_type>;

}
#endif // CODA_OSS_io_StringStream_h_INCLUDED_
