/* =========================================================================
 * This file is part of io-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2018, MDA Information Systems LLC
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

#ifndef __IO_BUFFER_VIEW_STREAM_H__
#define __IO_BUFFER_VIEW_STREAM_H__

#include <mem/BufferView.h>
#include <sys/Conf.h>
#include <except/Error.h>
#include <except/Exception.h>
#include <io/SeekableStreams.h>
#include <string.h>

/*!
 *  \file
 *  \brief  Class for streaming preallocated data, inherits from
 *      SeekableInputStream, SeekableOutputStream
 */
namespace io
{
/*!
 *  \class
 *  \brief  Class for streaming preallocated data, inherits from
 *      SeekableInputStream, SeekableOutputStream
 */
template <typename T>
class BufferViewStream: public SeekableInputStream, public SeekableOutputStream
{
public:
    /*!
     * Default constructor
     * \param bufferView The BufferView to wrap in the stream
     */
    BufferViewStream(const mem::BufferView<T>& bufferView) :
        mBufferView(bufferView),
        mPosition(0)
    {
    }

    //! Returns current location in buffer in bytes
    virtual sys::Off_T tell()
    {
        return mPosition * sizeof(T);
    }

    /*!
     * Seek to given location. Throw an exception if the seek would be out of
     * bounds.
     * \param offset Offset to seek to
     * \param whence Location to seek from
     * \return new position
     */
    virtual sys::Off_T seek(sys::Off_T offset, Whence whence);

    /*
     * \return The available bytes to read from the stream
     */
    virtual sys::Off_T available()
    {
        return (mBufferView.size - mPosition) * sizeof(T);
    }

    using OutputStream::write;
    using InputStream::streamTo;
    using InputStream::read;

    /*
     * Writes the bytes in data to the stream.
     * \param buffer The data to write to the stream
     * \param size The number of bytes to write to the stream
     */
    virtual void write(const void* buffer, size_t size);

    /*!
     * Get a pointer to the internal buffer.
     * This pointer should not be treated as valid
     * after a call to the seek, write, or reset methods
     * \return pointer to the internal buffer
     */
    T* get()
    {
        return mBufferView.data;
    }

    //! Returns const pointer to internal buffer
    const T* get() const
    {
        return mBufferView.data;
    }

    /*!
     * Overload for reading into a typed buffer
     * \param[out] buffer Buffer to read into
     * \param numElements How many -elements- (not bytes) to read
     */
    sys::SSize_T read(T* buffer, size_t numElements)
    {
        return InputStream::read(buffer, numElements * sizeof(T)) / sizeof(T);
    }

    /*!
    * Overload for writing from a typed buffer
    * \param buffer Buffer to write from
    * \param numElements How many -elements- (not bytes) to write
    */
    void write(const T* buffer, size_t numElements)
    {
        write(reinterpret_cast<const void*>(buffer), numElements * sizeof(T));
    }

protected:
    /*!
     * Read up to len bytes of data from this buffer into an array
     * update the mark
     * \param buffer Buffer to read into
     * \param len The length to read
     * \return  The number of bytes read
     */
    virtual sys::SSize_T readImpl(void* buffer, size_t len);


private:
    const mem::BufferView<T> mBufferView;
    sys::Off_T mPosition;
};

template <typename T>
sys::Off_T BufferViewStream<T>::seek(sys::Off_T offset, Whence whence)
{
    offset /= sizeof(T);
    // Let's not change anything until we know it will be valid
    sys::Off_T newPos = mPosition;
    switch (whence)
    {
        case START:
            newPos = offset;
            break;
        case END:
            if (offset > static_cast<sys::Off_T>(mBufferView.size))
            {
                newPos = 0;
            }
            else
            {
                newPos = mBufferView.size - offset;
            }
            break;
        default:
            newPos += offset;
            break;
    }

    if (newPos > static_cast<sys::Off_T>(mBufferView.size) || newPos < 0)
    {
        throw except::Exception(Ctxt("Attempted to seek beyond end of stream"));
    }
    mPosition = newPos;
    return tell();
}

template <typename T>
void BufferViewStream<T>::write(const void* buffer, size_t numBytes)
{
    const size_t numElements = numBytes / sizeof(T);
    const sys::Size_T newPos = mPosition + numElements;
    if (newPos > mBufferView.size)
    {
        std::ostringstream msg;
        msg << "Write of size " << numBytes << " runs out of bounds.";
        throw except::Exception(Ctxt(msg.str()));
    }

    ::memcpy(mBufferView.data + mPosition, buffer, numBytes);
    mPosition = newPos;
}

template <typename T>
sys::SSize_T BufferViewStream<T>::readImpl(void* buffer, size_t numBytes)
{
    size_t numElements = numBytes / sizeof(T);
    if (available() < static_cast<sys::Off_T>(numBytes))
    {
        numBytes = available();
        numElements = numBytes / sizeof(T);
    }
    if (numBytes == 0)
    {
        return 0;
    }

    ::memcpy(buffer, mBufferView.data + mPosition, numBytes);
    mPosition += numElements;
    return numBytes;
}
}
#endif

