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

#ifndef __IO_FILE_INPUT_STREAM_IOS_H__
#define __IO_FILE_INPUT_STREAM_IOS_H__

#if defined(USE_IO_STREAMS)

#include <ios>
#include <iostream>
#include <fstream>
#include "except/Exception.h"
#include "sys/filesystem.h"
#include "io/InputStream.h"
#include "io/SeekableStreams.h"

#include "sys/CPlusPlus.h"
#if CODA_OSS_cpp17
#include <std/filesystem>
#endif

/*!
 *  \file FileInputStreamIOS.h
 *  \brief The InputStream representation of a file
 *  \todo  Redefine the readln function to use getline() 
 *  The file contains the representation of an input stream
 *  from a file.  It mimics the Java io package API, with added 
 *  streaming capabilities
 */

namespace io
{

/*!
 *  \class FileInputStreamIOS
 *  \brief An InputStream from file
 *
 *  Use this object to create an input stream, where the available()
 *  method is based on the pos in the file, and the streamTo() and read()
 *  are file operations
 */
class FileInputStreamIOS : public SeekableInputStream
{
public:
    //!  Constructor
    FileInputStreamIOS()
    {}

    /*!
     *  Alternate Constructor.  Takes an input file and a mode
     *  \param inputFile The file name
     *  \param mode The mode to open the file in
     */
    FileInputStreamIOS(const coda_oss::filesystem::path& inputFile,
                       std::ios::openmode mode = std::ios::in);
    #if CODA_OSS_cpp17
    FileInputStreamIOS(const std::filesystem::path& inputFile,
                       std::ios::openmode mode = std::ios::in);
    #endif


    /*!
     *  Alternate Constructor.  Takes an input file and a mode
     *  \param inputFile The file name
     *  \param mode The mode to open the file in
     */
    FileInputStreamIOS(const char* inputFile,
                       std::ios::openmode mode = std::ios::in);


    virtual ~FileInputStreamIOS()
    {
        if (isOpen() ) close();
    }
    /*!
     * Returns the number of bytes that can be read
     * without blocking by the next caller of a method for this input
     * 
     * \throw except::IOException
     * \return number of bytes which are readable
     * 
     */
    virtual sys::Off_T available();

    /*!
     *  Report whether or not the file is open
     *  \return True if file is open
     */
    virtual bool isOpen();


    /*!
     *  Open the file in the mode provided
     *  \param file The file to open
     *  \param mode The mode
     */
    virtual void open(const char *file,
                      std::ios::openmode mode = std::ios::in );


    /*!
     *  Go to the offset at the location specified.
     *  \return The number of bytes between off and our origin.
     */
    virtual sys::Off_T seek(sys::Off_T offset, Whence whence);

    /*!
     *  Tell the current offset
     *  \return The byte offset
     */
    virtual sys::Off_T tell();

    //!  Close the file
    void close();

    /*!
     *  Access the stream directly
     *  \return The stream in native C++
     */
    virtual std::ifstream& stream()
    {
        return mFStream;
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
    virtual sys::SSize_T readImpl(void* buffer, size_t len);


    std::ifstream mFStream;
};



}
#endif
#endif
