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
#pragma once

#include <assert.h>

#include <memory>

#include <nitf/coda-oss.hpp>

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
 *  see if you set it to a non-nullptr address.  If so, we assume that you
 *  have created a properly sized buffer, and we will use that buffer
 *  for the request.  If you leave it nullptr (the path of least resistance),
 *  we assume that you intend for us to allocate a properly sized rectangle
 *  before the buffer is filled with pixel data.
 *
 *  Returned data is always component-interleaved.
 *
 *
 */
class Region final
{
    UByte* mBuffer = nullptr;
    ptrdiff_t startRow = 0;
    ptrdiff_t numRows = -1;
    ptrdiff_t startCol = 0;
    ptrdiff_t numCols = -1;
public:
    //!  Constructor.  Sets params for full window size, and buffer is nullptr
    Region() = default;

    //!  Destructor.  Does not deallocate a work buffer, that is up to the user
    ~Region() = default;

    /*!
     *  Set the start row.  Valid values are between 0 and numRows-1
     *  Since -1 is a valid value for numRows initializer, we do not evaluate
     *  this until read time.
     *
     */
    void setStartRow(ptrdiff_t row)
    {
        startRow = row;
    }
    /*!
     *  Set the start col.  Valid values are between 0 and numCols-1
     *  Since -1 is a valid value for numCols initializer, we do not evaluate
     *  this until read time.
     *
     */

    void setStartCol(ptrdiff_t col)
    {
        startCol = col;
    }

    /*!
     *  Set the number of rows to read.  Valid values are between 1 and 
     *  numRows, or -1 to request numRows without querying the
     *  actual rows
     */
    void setNumRows(ptrdiff_t rows)
    {
        numRows = rows;
    }

    /*!
     *  Set the number of cols to read.  Valid values are between 1 and 
     *  numRows, or -1 to request numRows without querying the
     *  actual rows
     */
    void setNumCols(ptrdiff_t cols)
    {
        numCols = cols;
    }

    /*!
     *  Get the start row
     */
    ptrdiff_t getStartRow() const
    {
        return startRow;
    }

    /*!
     *  Get the start col
     */
    ptrdiff_t getStartCol() const
    {
        return startCol;
    }

    /*!
     *  Get the number of rows.  This reflects what has been
     *  set by the user until a read has been done.  Therefore, it
     *  should not be used before a read has been done, except to verify
     *  the number of requested rows.
     */
    ptrdiff_t getNumRows() const
    {
        return numRows;
    }

    /*!
     *  Get the number of cols.  This reflects what has been
     *  set by the user until a read has been done.  Therefore, it
     *  should not be used before a read has been done, except to verify
     *  the number of requested cols.
     */
    ptrdiff_t getNumCols() const
    {
        return numCols;
    }

    /*!
     *  Get the buffer.  Before a read has been done, this may be nullptr,
     *  depending on if the user has initialized the buffer using the
     *  setBuffer() function.  After a read has been done, this value
     *  should be non-nullptr.  The work buffer must be deallocated by the
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
        assert(buffer != nullptr);
        mBuffer = buffer;
    }

    /*!
     *  Create a buffer of the given size, call setBuffer() and return the buffer.
     */
    std::unique_ptr<std::byte[]> setBuffer(size_t size)
    {
        assert(getBuffer() == nullptr);

        std::unique_ptr<std::byte[]> retval(new std::byte[size]);
        setBuffer(reinterpret_cast<UByte*>(retval.get()));
        return retval;
    }
};
}

#endif
