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

#include "io/StringStream.h"

//sys::SSize_T io::StringStream::readImpl(void* buffer, size_t len)
//{
//    sys::Off_T maxSize = available();
//    if (maxSize <= 0) return io::InputStream::IS_END;
//
//    if (maxSize < (sys::Off_T)len)
//        len = maxSize;
//
//    if (len <= 0) return 0;
//
//    mData.read((char *)buffer, len);
//    // Could be problem if streams are broken
//    // alternately could return gcount in else
//    // case above
//    return len;
//}
