/* =========================================================================
 * This file is part of tiff-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __TIFF_HEADER_H__
#define __TIFF_HEADER_H__

#include <import/io.h>
#include "tiff/Common.h"

namespace tiff
{

/**
 *********************************************************************
 * @class Header
 * @brief Contains TIFF header information
 *********************************************************************/
class Header : public io::Serializable
{
public:
    enum ByteOrder { MM, II };

    /**
     *****************************************************************
     * Constructor.  Allows the user to set the values in the header
     * and also provides resonable defaults.
     *
     * @param id
     *   the TIFF identifier, always "42"
     * @param byteOrder
     *   the byte order of the file "MM" for Big Endian, "II" 
     *   for Little Endian
     * @param ifdOffset
     *   the offset to the first IFD
     *****************************************************************/
    Header(const unsigned short id = 42, const char byteOrder[2] = "  ",
            const sys::Uint32_T ifdOffset = 8) :
        mId(id), mIFDOffset(ifdOffset)
    {
        bool isBigEndian = sys::isBigEndianSystem();
        // The code below previously used strncpy(), but compilers are now
        // quite aggressive about checking for potential problems.  We're only
        // dealing with two characters, so it's easy enough to do something else.
        if (byteOrder[0] == ' ' && byteOrder[1] == ' ')
        {
            //set it based on the system
            // strncpy(mByteOrder, isBigEndian ? "MM" : "II", 2);
            if (isBigEndian)
            {
                mByteOrder[0] = mByteOrder[1] = 'M'; // "MM"
            }
            else
            {
                mByteOrder[0] = mByteOrder[1] = 'I';  // "II"
            }
        }
        else
        {
            // strncpy(mByteOrder, byteOrder, 2);
            mByteOrder[0] = byteOrder[0];
            mByteOrder[1] = byteOrder[1];
        }

        mDifferentByteOrdering = isBigEndian ? \
                getByteOrder() != MM : getByteOrder() != II;
    }

    //! Destructor
    ~Header()
    {
    }

    /**
     *****************************************************************
     * Writes the TIFF header to the specified output stream.
     *
     * @param output
     *   the stream to write the header to
     *****************************************************************/
    void serialize(io::OutputStream& output);

    /**
     *****************************************************************
     * Reads the TIFF header from the specified input stream.
     *
     * @param input
     *   the stream to read the header from
     *****************************************************************/
    void deserialize(io::InputStream& input);

    /**
     *****************************************************************
     * Prints the TIFF header to the specified output stream in
     * string format.
     *
     * @param output
     *   the stream to print the header to
     *****************************************************************/
    void print(io::OutputStream& output) const;

    /**
     *****************************************************************
     * Retrieves the offset to the IFD.
     *
     * @return
     *   the IFD offset
     *****************************************************************/
    sys::Uint32_T getIFDOffset() const
    {
        return mIFDOffset;
    }

    ByteOrder getByteOrder() const
    {
        if (mByteOrder[0] == 'M' && mByteOrder[1] == 'M')
            return MM;

        return II;
    }
    
    bool isDifferentByteOrdering()
    {
        return mDifferentByteOrdering;
    }
    
private:

    //! The byte order
    char mByteOrder[2];

    //! The TIFF identifier
    unsigned short mId;

    //! The IFD offset
    sys::Uint32_T mIFDOffset;
    
    bool mDifferentByteOrdering;
    
};

} // End namespace.

#endif // __TIFF_HEADER_H__
