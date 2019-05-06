/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
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

#ifndef __IO_STREAM_SPLITTER_H__
#define __IO_STREAM_SPLITTER_H__

#include <string>
#include <vector>

#include <sys/Conf.h>
#include <io/InputStream.h>

namespace io
{
/*!
 * StreamSplitter splits the bytes from a stream into substrings separated by a
 * specified delimiter string. It uses buffered stream reads internally for
 * better efficiency than InputStream::readln for reading large amounts of data.
 */
class StreamSplitter {
public:
    /*!
     * \brief Create a stream splitter.
     *
     * \param inputStream The stream to read.
     * \param delimiter String containing the delimiter. Defaults to Unix line
     *        break "\n".
     * \param bufferSize Size of internal buffer.
     *        Defaults to 64KiB (2^16 bytes).
     */
    explicit StreamSplitter(io::InputStream& inputStream,
                            const std::string& delimiter = std::string("\n"),
                            size_t bufferSize = 65536);

    /*!
     * \brief Get the next substring from the stream.
     *
     * \param[out] substring A string which will be written and possibly resized
     *             if this call succeeds. Otherwise it will NOT be modified
     *             (return value should be checked).
     * \return true if this call succeeded, false if this call failed.
     */
    bool getNext(std::string& substring);

    /*!
     * \brief Check if the stream has no more substrings to return.
     *
     * \return true if no substrings remain, false otherwise
     */
    bool isEnd() const;

    /*!
     * \brief Get the number of substrings this splitter has returned.
     */
    size_t getNumSubstringsReturned() const;

    /*!
     * \brief Get the number of bytes this splitter has returned (does not
     *        include delimiters).
     */
    size_t getNumBytesReturned() const;

    /*!
     * \brief Get the number of bytes this splitter has processed (includes
     *        delimiters).
     *
     * This includes the content of all returned substrings and all delimiters
     * seen so far.
     *
     * For seekable streams, this may be used to set the stream position to the
     * point immediately following the last call to getNext, since the stream
     * may have been read past that point.
     */
    size_t getNumBytesProcessed() const;

private:
    /*!
     * \brief Append the buffer section from mBufferBegin to bufferSegmentEnd
     *        to the substring and remove it from the buffer.
     */
    void transferBufferSegmentToSubstring(std::string& substring,
                                          size_t& substringSize,
                                          sys::SSize_T bufferSegmentEnd);

    /*!
     * \brief Read from the stream if it has more data and the buffer has space.
     */
    void handleStreamRead();

    const std::string mDelimiter;
    sys::SSize_T mBufferValidBegin;
    sys::SSize_T mBufferValidEnd;
    size_t mNumSubstringsReturned;
    size_t mNumBytesReturned;
    size_t mNumDelimitersProcessed;
    std::vector<sys::byte> mBufferStorage;
    const sys::SSize_T mBufferCapacity;
    sys::byte* const mBuffer;
    io::InputStream& mInputStream;
    bool mStreamEmpty;
};
}

#endif
