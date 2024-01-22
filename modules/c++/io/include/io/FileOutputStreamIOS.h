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

#ifndef __IO_FILE_OUTPUT_STREAM_IOS_H__
#define __IO_FILE_OUTPUT_STREAM_IOS_H__

#if defined(USE_IO_STREAMS)
#include <ios>
#include <iostream>
#include <fstream>
#include "io/SeekableStreams.h"
#include <import/except.h>
#include <import/sys.h>


/*!
 *  \file FileOutputStreamIOS.h
 *  \brief The OutputStream representation of a file
 *
 *  The file contains the representation of an input stream
 *  from a file.  It mimics the Java io package API.  These classes are deprecated,
 */

namespace io
{


/*!
 *  \class FileOutputStreamOS
 *  \brief A type of OutputStream that writes to file
 *
 *  This class corresponds closely to its java namesake.
 *  It also leverages upon the std::iostream package from C++,
 *  allowing its native type to be accessible to the developer,
 *  should he or she have the desire to modify it directly
 */
struct FileOutputStreamIOS : public OutputStream
{
    FileOutputStreamIOS() = default;


    /*!
     *  Alternate Constructor.  Takes an output file and a mode
     *  \param outputFile The file name
     *  \param creationFlags see sys::File
     */
    FileOutputStreamIOS(const std::string& outputFile,
                       int creationFlags = sys::File::CREATE | sys::File::TRUNCATE);

    /*!
     *  Alternate Constructor.  Takes an output file and a mode
     *  \param outputFile The file name
     *  \param creationFlags see sys::File
     */
    FileOutputStreamIOS(const char* outputFile,
                       int creationFlags = sys::File::CREATE | sys::File::TRUNCATE);

    //! Deconstructor, closes the file stream.
    virtual ~FileOutputStreamIOS()
    {
        if (isOpen()) close();
    }

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
                      std::ios::openmode mode = std::ios::out);

    using OutputStream::write;

    //!  Close the file
    void close();

    /*!
     * This method defines a given OutputStream. By defining,
     * this method, you can define the unique attributes of an OutputStream
     * inheriting class.
     * \param buffer The byte array to write to the stream
     * \param len the length of bytes to write
     * \throw IoException
     */
    virtual void write(const void* buffer, size_t len);
    /*!
     *  Access the stream directly
     *  \return The stream in native C++
     */
    virtual std::ofstream& stream()
    {
        return mFStream;
    }

    virtual void flush()
    {
        mFStream.flush();
    }
    
    sys::Off_T seek(sys::Off_T offset, io::Seekable::Whence whence);
    
    sys::Off_T tell();

protected:
    std::ofstream mFStream;
    std::string mFileName;
};

}
#endif
#endif
