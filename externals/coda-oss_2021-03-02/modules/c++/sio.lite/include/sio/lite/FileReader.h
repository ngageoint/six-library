/* =========================================================================
 * This file is part of sio.lite-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sio.lite-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIO_LITE_FILE_READER_H__
#define __SIO_LITE_FILE_READER_H__

#include <import/sys.h>
#include <io/Seekable.h>
#include <io/FileInputStream.h>
#include "sio/lite/InvalidHeaderException.h"
#include "sio/lite/StreamReader.h"

namespace sio
{
namespace lite
{


/**
 *  \class FileReader
 *  \brief Seekable version of StreamReader
 *
 *  This class takes advantage of our Seekable interface, in addition
 *  to piggybacking off of the StreamReader.  This clears up the situation
 *  with Seekable vs Non-Seekable SIO streams.  If its a pure stream, use
 *  the StreamReader (e.g., data coming in from a socket).  If its really a file
 *  use the FileReader in conjunction with a FileInputStream.
 *
 *  To make this safer, only supports the FileInputStream and std::string 
 *  args for now:
 *
    \code 

    FileReader reader("/path/to/file.sio");
    
    FileHeader* fhdr = reader->getHeader();
    int nl = fhdr->getNumLines();
    int ne = fhdr->getNumElements();
    int es = fhdr->getElementSize();
    int et = fhdr->getElementType();

    // Read a double element from the user data
    std::vector<sys::byte>& ud = fhdr->getUserData("userDefinedDouble");       

    // Cast address from sys::byte* -> double*, and deref:
    double d = fhdr->isDifferentByteOrdering() ? 
        sys::byteSwap<double>(*(double*)&ud[0]) : *(double*)&ud[0];

    // Read in an amplitude single precision float image
    // all at once and byte swap
    assert( es == sizeof(float) );
    long total( (long)nl * (long)ne );
    std::vector<float> v(total);

    // Slurp file into memory
    reader->read((sys::byte*)&v[0], total);

    // Swap data if necessary
    if (fhdr->isDifferentByteOrdering())
        sys::byteSwap((sys:byte*)&v[0], sizeof(float), total);

    \endcode
 */

class FileReader : public StreamReader, public io::Seekable
{


public:

    /** Constructor */
    FileReader() : StreamReader() {}

    /** Destructor */
    ~FileReader()
    {
    }

    FileReader(const std::string& file) :
        StreamReader(new io::FileInputStream(file), true)
    {
    }


    /**  Construct from stream  */
    FileReader(io::FileInputStream* is, bool adopt = false) : 
        StreamReader(is, adopt)
    {
    }

    /*!
     *  Overloaded seek, only works if this is a FileInputStream.
     *  Reports position relative to file header if Whence == 
     *  io::Seekable::START.  Otherwise, works as a normal FileInputStream.
     *  In a typical
     *  header with no UD (i.e., headerLength = 20), seek
     *  with offset 0 to get to the 20th byte:

        \code

        sio.seek(0, io::Seekable::START);
        sio.read(buf, n);
        assert( sio.tell() == n );

        \endcode
     *
     *  For simplicity, the final position is reported in the return value relative
     *  to header start (always).  This is done by calling tell().
     */
    sys::Off_T seek( sys::Off_T offset, Whence whence );

    /*!
     *  Overloaded method, only works if this is a FileInputStream.
     *  Reports position relative to file header.  In a typical
     *  header with no UD (i.e., headerLength = 20), this will
     *  yield 0 as a return value.
     *
     */
    sys::Off_T tell();


    void killStream();
protected:
};
}
}

#endif

