/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * tiff-c++ is free software; you can redistribute it and/or modify
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

#include "tiff/Header.h"
#include <sstream>
#include <import/io.h>

// INCOMPLETE
void tiff::Header::serialize(io::OutputStream& output)
{
    output.write((sys::byte *)&mByteOrder, sizeof(mByteOrder));
    output.write((sys::byte *)&mId, sizeof(mId));
    output.write((sys::byte *)&mIFDOffset, sizeof(mIFDOffset));
}

void tiff::Header::deserialize(io::InputStream& input)
{
    input.read((sys::byte *)&mByteOrder, sizeof(mByteOrder));
    input.read((sys::byte *)&mId, sizeof(mId));
    input.read((sys::byte *)&mIFDOffset, sizeof(mIFDOffset));
    
    mDifferentByteOrdering = sys::isBigEndianSystem() ? \
            getByteOrder() != tiff::Header::MM : getByteOrder() != tiff::Header::II;
    
    if (mDifferentByteOrdering)
    {
        mId = sys::byteSwap(mId);
        mIFDOffset = sys::byteSwap(mIFDOffset);
    }
}

void tiff::Header::print(io::OutputStream& output) const
{
    std::ostringstream message;
    message << "Type:          " << mByteOrder[0] << mByteOrder[1] << std::endl;
    message << "Version:       " << mId << std::endl;
    message << "IFD Offset:    " << mIFDOffset << std::endl;
    output.write(message.str());
}


