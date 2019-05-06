/* =========================================================================
 * This file is part of zip-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * zip-c++ is free software; you can redistribute it and/or modify
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

#ifndef __ZIP_GZIP_INPUT_STREAM_H__
#define __ZIP_GZIP_INPUT_STREAM_H__

#include "zip/Types.h"

namespace zip
{
/*!
 *  \class GZipInputStream
 *  \brief Read from a gzip file
 *
 *  This class lacks the knowledge of how many uncompressed bytes
 *  are available from a stream, and so, like the NetConnection, it
 *  does not override the available() function.  For this reason,
 *  the user should only call streamTo() if the optional buffer size
 *  argument is given, and in a loop.  On the last run, the buffer
 *  size should will probably smaller than the amount requested.
 */
class GZipInputStream: public io::InputStream
{
    gzFile mFile;
public:

    //!  Constructor requires initialization
    GZipInputStream(const std::string& file);

    /*!
     *  Close the gzip stream.  You must call this
     *  afterward (it is not done automatically);
     */
    virtual void close();

protected:
    /*!
     *  Read len bytes into the buffer from the stream.
     *  This is a little tricky since we do not know the
     *  length of the read.
     */
    virtual sys::SSize_T readImpl(void* buffer, size_t len);

};
}

#endif

