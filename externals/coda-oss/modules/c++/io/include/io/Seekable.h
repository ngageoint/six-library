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

#ifndef __IO_SEEKABLE_H__
#define __IO_SEEKABLE_H__
#pragma once

/*!
 *  Unlike in Java, we have chosen to make our InputStream and OutputStream
 *  classes into interfaces, not abstract classes (in the sense that we
 *  did not allow the mark object).  This allowed us to create a third
 *  interface which we use for sockets, among other things: the 
 *  BidirectionalStream and ByteStream interfaces.  These have a strong
 *  need to behave similarly to their C++/C counterparts.  
 *
 *  As a result, where Java simply adds an offset field to the read() and
 *  write() methods, we felt that it would make more sense to have 
 *  an interface for seeks.  Down the line, this allows us to keep our
 *  sockets simple, but to implement full-fledged file IO seeks.  
 */

namespace io
{
class Seekable
{
public:
    Seekable() = default;
    virtual ~Seekable() {}
    
    enum class Whence { CURRENT = 0, START, END };
    static constexpr Whence CURRENT = Whence::CURRENT;
    static constexpr Whence START = Whence::START;
    static constexpr Whence END = Whence::END;

    /*!
     *  Seek to an offset
     *  \param offset The place to seek
     */
    virtual sys::Off_T seek( sys::Off_T offset, Whence whence ) = 0;
    virtual sys::Off_T tell() = 0;
};

}

#endif
