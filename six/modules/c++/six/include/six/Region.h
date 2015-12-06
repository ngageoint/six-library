/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_H__
#define __SIX_H__

#include "six/Types.h"

namespace six
{
/*!
 *  \class Region
 *  \brief Request window into pixel data
 *
 *  Gives the user a region into the pixel data.  This class is
 *  used for both complex and derived products.  When you initialize
 *  this object, you can set your own malloc'ed work buffer.  When you
 *  request a read from ReadControl, we check your mBuffer value to
 *  see if you set it to a non-NULL address.  If so, we assume that you
 *  have created a properly sized buffer, and we will use that buffer
 *  for the request.  If you leave it NULL (the path of least resistance),
 *  we assume that you intend for us to allocate a properly sized rectangle
 *  before the buffer is filled with pixel data.
 *
 *  Returned data is always component-interleaved.
 *
 *
 */
class Region
{
    UByte* mBuffer;
    sys::SSize_T startRow;
    sys::SSize_T numRows;
    sys::SSize_T startCol;
    sys::SSize_T numCols;
public:
    //!  Constructor.  Sets params for full window size, and buffer is NULL
    Region() :
        mBuffer(NULL), startRow(0), numRows(-1), startCol(0), numCols(-1)
    {
    }

    //!  Destructor.  Does not deallocate a work buffer, that is up to the user
    ~Region() {}

    /*!
     *  Set the start row.  Valid values are between 0 and numRows-1
     *  Since -1 is a valid value for numRows initializer, we do not evaluate
     *  this until read time.
     *
     */
    void setStartRow(sys::SSize_T row)
    {
        startRow = row;
    }
    /*!
     *  Set the start col.  Valid values are between 0 and numCols-1
     *  Since -1 is a valid value for numCols initializer, we do not evaluate
     *  this until read time.
     *
     */

    void setStartCol(sys::SSize_T col)
    {
        startCol = col;
    }

    /*!
     *  Set the number of rows to read.  Valid values are between 1 and 
     *  numRows, or -1 to request numRows without querying the
     *  actual rows
     */
    void setNumRows(sys::SSize_T rows)
    {
        numRows = rows;
    }

    /*!
     *  Set the number of cols to read.  Valid values are between 1 and 
     *  numRows, or -1 to request numRows without querying the
     *  actual rows
     */
    void setNumCols(sys::SSize_T cols)
    {
        numCols = cols;
    }

    /*!
     *  Get the start row
     */
    sys::SSize_T getStartRow() const
    {
        return startRow;
    }

    /*!
     *  Get the start col
     */
    sys::SSize_T getStartCol() const
    {
        return startCol;
    }

    /*!
     *  Get the number of rows.  This reflects what has been
     *  set by the user until a read has been done.  Therefore, it
     *  should not be used before a read has been done, except to verify
     *  the number of requested rows.
     */
    sys::SSize_T getNumRows() const
    {
        return numRows;
    }

    /*!
     *  Get the number of cols.  This reflects what has been
     *  set by the user until a read has been done.  Therefore, it
     *  should not be used before a read has been done, except to verify
     *  the number of requested cols.
     */
    sys::SSize_T getNumCols() const
    {
        return numCols;
    }

    /*!
     *  Get the buffer.  Before a read has been done, this may be NULL,
     *  depending on if the user has initialized the buffer using the
     *  setBuffer() function.  After a read has been done, this value
     *  should be non-NULL.  The work buffer must be deallocated by the
     *  program, since the Region does not currently do so.
     */
    UByte* getBuffer()
    {
        return mBuffer;
    }

    /*!
     *  Set a work buffer as our main buffer.  This function does not
     *  do any error checking on the buffer or its size.  If the caller
     *  makes use of this optional function, it is assumed that they have
     *  already sized this buffer correctly.
     */
    void setBuffer(UByte* buffer)
    {
        mBuffer = (UByte*) buffer;
    }
};
}

#endif

