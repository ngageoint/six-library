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

#ifndef __IO_SEEKABLE_STREAMS_H__
#define __IO_SEEKABLE_STREAMS_H__

#include "io/InputStream.h"
#include "io/OutputStream.h"
#include "io/BidirectionalStream.h"
#include "io/Seekable.h"

/*!
 *  \file
 *  \brief This class is interface like and completes the Seekable interface.
 *
 *  When I first created the Seekable interface, I overlooked the possible
 *  use-case that you may, in fact, wish to guarantee that your Seekable
 *  is also an InputStream.  What you would have, then, are two derived types:
 *  SeekableInputStream, SeekableOutputStream, and SeekableBidirectionalStream.
 *  Here are the definitions, which will allow that inheritance 
 */

namespace io
{
struct SeekableInputStream :
            public InputStream, public Seekable
{
    SeekableInputStream() = default;
    virtual ~SeekableInputStream() = default;
    using InputStream::streamTo;
};

struct SeekableOutputStream :
            public OutputStream, public Seekable
{
    SeekableOutputStream() = default;
    virtual ~SeekableOutputStream() = default;
};

struct SeekableBidirectionalStream :
            public BidirectionalStream, public Seekable
{
    SeekableBidirectionalStream() = default;
    virtual ~SeekableBidirectionalStream() = default;
    using InputStream::streamTo;
};

}


#endif
