/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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

#ifndef __IO_MOCK_SEEKABLE_INPUT_STREAM_H__
#define __IO_MOCK_SEEKABLE_INPUT_STREAM_H__

#include <algorithm>
#include <cstring>
#include "except/Exception.h"
#include "io/InputStream.h"
#include "io/SeekableStreams.h"
#include "mem/ScopedArray.h"


/*!
 *  \file MockSeekableInputStream.h
 *  \brief A SeekableInputStream implementation for use with testing
 */
namespace io
{

/*!
 *  \class MockSeekableInputStream
 *  \brief A SeekableInputStream from string
 *
 *  Use this object to create an input stream, where the available()
 *  method is based on the pos in the string, and the streamTo() and read()
 *  are string operations.
 *
 *  The purpose of this class is to simplify
 *  the testing of methods which take a SeekableInputStream parameter
 */
class MockSeekableInputStream : public SeekableInputStream
{
public:

    /*!
     *  Constructor.  Takes an input array of data
     *  \param input The desired stream contents
     *  \param length The size of the input
     */
    MockSeekableInputStream(const sys::byte* input, size_t length) :
        mSource(input),
        mLength(length),
        mOffset(0)
    {
    }

    /*!
     * Convenince constructor from a string
     * \param input the desired strema contents
     */
    MockSeekableInputStream(const std::string& input) :
        mSource(copyString(input)),
        mLength(input.size()),
        mOffset(0)
    {
    }

    virtual ~MockSeekableInputStream()
    {}

    /*!
     * Returns the number of bytes that can be read
     * without blocking by the next caller of a method for this input
     *
     * \throw except::IOException
     * \return number of bytes which are readable
     *
     */
    virtual sys::Off_T available()
    {
        return mLength - mOffset;
    }

    /*!
     *  Go to the offset at the location specified.
     *  \return The number of bytes between off and the start of the stream
     */
    virtual sys::Off_T seek(sys::Off_T off, Whence whence)
    {
        int destination;
        switch (whence)
        {
            case END:
                destination = mLength + off;
                break;

            case START:
                destination = off;
                break;

            default:
                destination = mOffset + off;
        }
        if (destination < 0 || destination > mLength)
        {
            throw except::Exception(Ctxt("Requested off-set out of bounds"));
        }
        mOffset = destination;
        return mOffset;
    }

    /*!
     *  Tell the current offset
     *  \return The byte offset
     */
    virtual sys::Off_T tell()
    {
        return mOffset;
    }

    /*!
     * Read up to len bytes of data from input stream into an array
     *
     * \param b   Buffer to read into
     * \param len The length to read
     * \throw except::IOException
     * \return  The number of bytes read
     *
     */
    virtual sys::SSize_T read(sys::byte* b, sys::Size_T len)
    {
        len = std::min(len, mLength - mOffset);
        std::memcpy(b, mSource.get() + mOffset, len);
        mOffset += len;
        return len;
    }

private:
    sys::byte* copyString(const std::string& input)
    {
        sys::byte* copy = new sys::byte[input.size()];
        std::memcpy(copy, input.data(), input.size());
        return copy;
    }

    mem::ScopedArray<const sys::byte> mSource;
    size_t mLength;
    size_t mOffset;

};
}
#endif

