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
#ifndef __SIO_LITE_STREAM_READER_H__
#define __SIO_LITE_STREAM_READER_H__

#include <io/InputStream.h>
#include "sio/lite/FileHeader.h"

namespace sio
{
namespace lite
{

/**
 *  \class StreamReader
 *  \brief This package presents a simple file reader for the SIO format.
 *
 *  The sio.lite module is provided as a simple native solution for SIO
 *  in C++ and Java.  It works on at least type 1 and 2 SIO files
 *  (supports user data).  It also attempts 3 and 4 files, although they
 *  are not guaranteed to be successful.  This package also works
 *  cross-platform.  You are encouraged, as a user of this package, to
 *  contribute ideas and solutions to the maintainers to make this product
 *  more robust and extensible.
 *
 *  sio.lite byte swaps fields that are needed to understand an SIO
 *  file on any platform (either little or big-endian).  However, it
 *  relies on the end user to do the actual byte swapping (with the
 *  sys::byteSwap() functions).  This is very easy to do, and as such
 *  doesnt add much value to sio.lite, so the decision was made to 
 *  force this behavior externally.  Here are some common examples:
 *
    \code 

    StreamReader reader(stream);
    
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

    // Close/delete if necessary, if we didnt tell sio.lite to adopt
    // which you can do by passing adopt=true
    io::InputStream* is = reader->getInputStream();
    delete is;

    \endcode
 *
 */
class StreamReader : public io::InputStream
{
public:
    /** Constructor */
    StreamReader() : 
        inputStream(nullptr), header(nullptr), headerLength(0), own(false) {}

    /** Destructor */
    virtual ~StreamReader()
    {
        killHeader();
        killStream();
    }

    /*! 
     *  Construct from stream, note that adopt defaults to false,
     *  which is probably not what you want unless you are passing
     *  an address.  This is for legacy compatibility.
     */
    StreamReader(io::InputStream* is, bool adopt = false) : 
        inputStream(is), header(nullptr), headerLength(0), own(adopt)
    {
        // No longer calling setInputStream directly -- its virtual now
        parseHeader(true);
    }


    /**
     *  Reset the input stream to read from. 
     *  This has different behavior depending on the subclass, since
     *  a FileReader will want to close() its stream, whereas this
     *  class can make no such guarantees.
     *
     *  @param is The input stream to assign internally
     *  @param adopt Should we adopt the stream?
     */
    virtual void setInputStream(io::InputStream* is, bool adopt = false);

    //!  New method to allow you to get the raw stream
    io::InputStream* getInputStream() { return inputStream; }

    sio::lite::FileHeader* getHeader() { return header; }
    const FileHeader* getHeader() const { return header; }

    /*! 
     *  This method is deprecated, as read is now done during
     *  setInputStream().  This means that readHeader is now
     *  identical to getHeader().
     *
     */
    sio::lite::FileHeader* readHeader() { return header; }


    sys::Off_T available()
    {
        return inputStream->available();
    }

protected:
    /**
     *  Implements the necessary function to make this
     *  all work.
     *
     * NOTE: You will need to byteswap if the endianness of the
     * machine is different from the stream endianness.
     *  @param buffer The byte buffer to read into
     *  @param size the Number of bytes to read
     *  @return The number of bytes read
     */
    virtual sys::SSize_T readImpl(void* buffer, size_t size)
    {
        return inputStream->read(buffer, size);
    }

    /**
     *  Read the next integer in the stream.  This is vital for
     *  reading our header.
     *
     *  Java is very pedantic about showing what you're throwing.
     *  C++ doesn't really enforce this (of course its possible).
     *  Since Im not quite sure yet that they are throwing exactly
     *  the same stuff, Im not being very explicit about that right
     *  now.
     *
     *  @throws sio::lite::InvalidHeaderException
     *  @return The next integer in the stream.
     */
    int getNextInteger();


    /**
     *  Type 2 headers have user data.  This method
     *  pulls out the user data and puts it into the
     *  data structure appropriated for the storage of
     *  this user data (by key) in the file header.
     *  This function is called on all versions of SIO
     *  higher than 1 (which has no user data).
     *  It is only guaranteed to work on type 2, although
     *  it will probably successfully work on 3 as well.
     *
     */
    void readType2Header();

    /**
     *  In C++, we have to check our system endian-ness
     *  We find out if our file came from a system that has
     *  little-endian order, and we tell it to swap if necessary
     *
     *  We also want to determine what kind of SIO file we have.  Here
     *  we use only 1 & 2 version (Im not sure about 3 || 4)
     *  @throws sio::lite::InvalidHeaderException if the header is
     *  off.
     *
     */
    void checkMagic(bool calledFromConstructor = false);

    /**
     *  Since C++ doesnt do your garbage collection for you,
     *  we have this function, which is called internally when
     *  you try and swap headers.
     *
     */
    void killHeader();

    /**
     *  Since C++ doesnt do your garbage collection for you,
     *  we have this function, which is called internally when
     *  the stream is to be deleted for any reason
     *
     *  This method is virtual because the FileReader subclass needs
     *  to close the handle possibly
     */
    
    virtual void killStream();

    /**
     *  Read in the file header.  This should be done before
     *  the read method is used on the imagery.  This function
     *  reads type 1 and 2 headers (type 2 have user data).
     *  It also attempts to read types 3 and 4, but no promises
     *  are made that they will be successful.
     *
     *  This method used to be public, but its too dangerous to use
     *  it directly.  Instead, this method is now called as
     *  soon as an input stream is set
     *
     */
    void parseHeader(bool calledFromConstructor = false);


    io::InputStream* inputStream;
    FileHeader*  header;
    long headerLength;
    bool own;
};


}
}
#endif

