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

#ifndef __IO_FILE_INPUT_STREAM_OS_H__
#define __IO_FILE_INPUT_STREAM_OS_H__

#include "config/Exports.h"

#if !defined(USE_IO_STREAMS)

#include "except/Exception.h"
#include "sys/File.h"
#include "sys/filesystem.h"
#include "io/InputStream.h"
#include "io/SeekableStreams.h"


/*!
 *  \file FileInputStreamOS.h
 *  \brief The InputStream representation of a file using the OS handle
 *  The file contains the representation of an input stream
 *  from a file.  It mimics the Java io package API, with added
 *  streaming capabilities
 */

namespace io
{
constexpr size_t defaultNumThreads = 1;
constexpr size_t defaultChunkSize = 32L * 1024L * 1024L;
constexpr size_t defaultMinChunksForThreading = 4;

/*!
 *  \class FileInputStreamOS
 *  \brief An InputStream from file
 *
 *  Use this object to create an input stream, where the available()
 *  method is based on the pos in the file, and the streamTo() and read()
 *  are file operations
 */
struct CODA_OSS_API FileInputStreamOS : public SeekableInputStream
{
protected:
    sys::File mFile;
    size_t mMaxReadThreads;
    size_t mParallelChunkSize;
    size_t mMinChunksForThreading;

public:

    FileInputStreamOS() = default;

    /*!
     *  Alternate Constructor.  Takes an input file and a mode
     *  \param inputFile The file name
     *  \param mode The mode to open the file in
     */
    FileInputStreamOS(const std::string& inputFile) :
        mMaxReadThreads(defaultNumThreads),
        mParallelChunkSize(defaultChunkSize),
        mMinChunksForThreading(defaultMinChunksForThreading)
    {
        // Let this SystemException slide for now
        mFile.create(inputFile,
                     sys::File::READ_ONLY,
                     sys::File::EXISTING);
    }
    explicit FileInputStreamOS(const coda_oss::filesystem::path& inputFile) :
        FileInputStreamOS(inputFile.string()) { }
    FileInputStreamOS(const char* inputFile) : // "file.txt" could be either std::string or std::filesystem::path
        FileInputStreamOS(std::string(inputFile))  {  }

    FileInputStreamOS(const sys::File& inputFile) :
        mMaxReadThreads(defaultNumThreads),
        mParallelChunkSize(defaultChunkSize),
        mMinChunksForThreading(defaultMinChunksForThreading)
    {
        mFile = inputFile;
    }

    virtual ~FileInputStreamOS()
    {
        if ( isOpen() )
        {
            close();
        }
    }

    /*!
     * Returns the number of bytes that can be read
     * without blocking by the next caller of a method for this input
     *
     * \throw except::IOException
     * \return number of bytes which are readable
     *
     */
    virtual sys::Off_T available() override;

    /*!
     *  Report whether or not the file is open
     *  \return True if file is open
     */
    virtual bool isOpen() const noexcept
    {
        return mFile.isOpen();
    }


    /*!
     *  Open the file in the mode provided
     *  \param file The file to open
     *  \param mode The mode
     */
    virtual void create(const std::string& str)
    {
        mFile.create(str,
                     sys::File::READ_ONLY,
                     sys::File::EXISTING);

    }



    /*!
     *  Go to the offset at the location specified.
     *  \return The number of bytes between off and our origin.
     */
    virtual sys::Off_T seek(sys::Off_T off, Whence whence) override
    {
        int from = sys::File::FROM_CURRENT;
        switch (whence)
        {
            case END:
                from = sys::File::FROM_END;
                break;

            case START:
                from = sys::File::FROM_START;
                break;

            case CURRENT:
            default:
                from = sys::File::FROM_CURRENT;
        }
        return mFile.seekTo( off, from );
    }

    /*!
     *  Tell the current offset
     *  \return The byte offset
     */
    virtual sys::Off_T tell() override
    {
        return mFile.getCurrentOffset();
    }

    //!  Close the file
    void close()
    {
        mFile.close();
    }

    /*!
     *  Set the limit of the number of parallel read threads.  Upon
     *  construction, value is set to 1 read thread.  This sets a limit which is
     *  considered for every read() call on the object.  Parallel reading is
     *  only enabled if the read is at least
     *  getParallelChunkSize()*setMinimumChunkCount() bytes, and if
     *  getMaxReadThreads() is larger than 1.
     *  \param maxReadThreads Maximum number of parallel read threads
     */
    void setMaxReadThreads(size_t maxReadThreads)
    {
        mMaxReadThreads = maxReadThreads;
    }

    /*!
     *  Get the limit of the number of parallel read threads.
     *  \return Maximum number of parallel read threads
     */
    size_t getMaxReadThreads() const
    {
        return mMaxReadThreads;
    }

    /*!
     *  Set the chunked read size when doing parallel reads.
     *  \param chunkSize Chunk size, in bytes
     */
    void setParallelChunkSize(size_t chunkSize)
    {
        mParallelChunkSize = chunkSize;
    }

    /*!
     *  Get the chunked read size for parallel reads
     *  \return Chunk size, in bytes
     */
    size_t getParallelChunkSize() const
    {
        return mParallelChunkSize;
    }

    /*!
     *  Set the minimum number of chunks (of size getParallelChunkSize() bytes)
     *  for enabling parallel reading
     *  \param minChunks Minimum chunk count
     */
    void setMinimumChunkCount(size_t minChunks)
    {
        mMinChunksForThreading = minChunks;
    }

    /*!
     *  Get the minimum number of chunks (of size getParallelChunkSize() bytes)
     *  for enabling parallel reading
     *  \return Minimum chunk count
     */
    size_t getMinimumChunkCount() const
    {
        return mMinChunksForThreading;
    }

protected:
    /*!
     * Read up to len bytes of data from input stream into an array
     *
     * \param buffer Buffer to read into
     * \param len The length to read
     * \throw except::IOException
     * \return  The number of bytes read
     *
     */
    virtual sys::SSize_T readImpl(void* buffer, size_t len) override;
};
}

#endif
#endif
