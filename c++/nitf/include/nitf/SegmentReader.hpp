/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_SEGMENT_READER_HPP__
#define __NITF_SEGMENT_READER_HPP__

#include "nitf/SegmentReader.h"
#include "nitf/Object.hpp"
#include <string>

/*!
 *  \file SegmentReader.hpp
 *  \brief  Contains wrapper implementation for SegmentReader
 */
namespace nitf
{

/*!
 *  \class SegmentReader
 *  \brief  The C++ wrapper for the nitf_SegmentReader
 */
DECLARE_CLASS(SegmentReader)
{
public:
    SegmentReader()
    {
    }

    //! Copy constructor
    SegmentReader(const SegmentReader & x);

    //! Assignment Operator
    SegmentReader & operator=(const SegmentReader & x);

    //! Set native object
    SegmentReader(nitf_SegmentReader * x);

    ~SegmentReader();

    /*!
     * \brief nitf_SegmentReader_read - Read segment data
     *
     * The nitf_SegmentReader_read function reads data from the associated segment.
     * The reading of the data is serial as if a flat file were being read by an
     * input stream. Except for the first argument, this function has the same
     * calling sequence and behavior as IOInterface_read.
     *
     * \param buffer    buffer to hold data
     * \param count     amount of data to return
     */
    void read(NITF_DATA *buffer, size_t count) throw (nitf::NITFException);

    /*!
     * \brief seek - Seek in segment data
     *
     * The nitf_SegmentReader_seek function allows the user to seek within the extension
     * data. Except for the first argument, this function has the same calling
     * sequence and behaivior as IOInterface_seek. The offset is relative to the top
     * of the segment data.
     *
     * The returned value maybe tested with NITF_IO_SUCCESS()
     * \param offset    the seek offest
     * \param whence    starting at (SEEK_SET, SEEK_CUR, SEEK_END)
     * \return The offset from the beginning to the current position is set.
     */
    nitf::Off seek(nitf::Off offset, int whence) throw (nitf::NITFException);

    /*!
     * \brief tell - Tell location in segment data
     *
     * The tell function allows the user to determine the current
     * offset within the extension data. Except for the first argument, this
     * function has the same calling sequence and behaivior as IOInterface_tell. The
     * offset is relative to the top of the segment data.
     *
     * The returned value may be tested with NITF_IO_SUCCESS()
     *
     * \return The offset from the beginning to the current position is set.
     */
    nitf::Off tell();

    /*!
     * \brief getSize - Determine size of the data
     *
     * The nitf_SegmentReader_seek function allows the user to determine the size of
     * the data. Except for the first argument, this function has the same calling
     * sequence and behavior as IOInterface_getSize.
     *
     * The returned value maybe tested with NITF_IO_SUCCESS()
     *
     * \return The offset from the beginning to the current position is set.
     */
    nitf::Off getSize();

private:
    nitf_Error error;
};

}
#endif
