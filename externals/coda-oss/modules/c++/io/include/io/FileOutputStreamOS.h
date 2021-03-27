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

#ifndef CODA_OSS_io_FileOutputStreamOS_h_INCLUDED_
#define CODA_OSS_io_FileOutputStreamOS_h_INCLUDED_

#include <string>

#if !defined(USE_IO_STREAMS)

#include "io/SeekableStreams.h"
#include "sys/Filesystem.h"
#include "sys/File.h"
#include "coda_oss/filesystem.h"


/*!
 *  \file FileOutputStream.h
 *  \brief The OutputStream representation of a file
 *
 *  The file contains the representation of an input stream
 *  from a file.  It mimics the Java io package API.
 */

namespace io
{

/*!
 *  \class FileOutputStreamOS
 *  \brief A type of OutputStream that writes to file using OS FDs
 *
 *  This class corresponds closely to its java namesake.
 *  It uses native file handles to make writes.
 */
class FileOutputStreamOS : public SeekableOutputStream

{
protected:
    sys::File mFile;
public:
    //!  Default constructor
    FileOutputStreamOS()
    {}


    /*!
     *  Alternate Constructor.  Takes an output file and a mode
     *  \param outputFile The file name
     *  \param creationFlags  see sys::File
     */
    FileOutputStreamOS(const coda_oss::filesystem::path& outputFile,
                       int creationFlags = sys::File::CREATE | sys::File::TRUNCATE);

    //! Destructor, closes the file stream.
    virtual ~FileOutputStreamOS()
    {
        if ( isOpen() )
        {
            close();
        }
    }

    /*!
     *  Report whether or not the file is open
     *  \return True if file is open
     */
    virtual bool isOpen()
    {
        return mFile.isOpen();
    }

    /*!
     *  Open the file in the mode provided
     *  \param file The file to open
     *  \param creationFlags see sys::File
     */
    virtual void create(const coda_oss::filesystem::path& str,
                        int creationFlags = sys::File::CREATE | sys::File::TRUNCATE);

    //!  Close the file
    void close()
    {
        mFile.close();
    }

    virtual void flush();

    sys::Off_T seek(sys::Off_T offset, io::Seekable::Whence whence);

    sys::Off_T tell();

    using OutputStream::write;

    /*!
     * This method defines a given OutputStream. By defining,
     * this method, you can define the unique attributes of an OutputStream
     * inheriting class.
     * \param buffer The byte array to write to the stream
     * \param len the length of bytes to write
     * \throw IoException
     */
    virtual void write(const void* buffer, size_t len);
};
}

#endif
#endif // CODA_OSS_io_FileOutputStreamOS_h_INCLUDED_
