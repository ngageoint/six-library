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

#ifndef __NITF_SEGMENT_SOURCE_H__
#define __NITF_SEGMENT_SOURCE_H__

#include "nitf/System.h"
#include "nitf/DataSource.h"
#include "nitf/SegmentReader.h"


NITF_CXX_GUARD


/*!
 * Typedef nitf_BandSource
 */
typedef nitf_DataSource             nitf_SegmentSource;


/*!
 * Define nitf_SegmentSource_destruct which is just nitf_DataSource_destruct
 */
#define nitf_SegmentSource_destruct    nitf_DataSource_destruct


/*!
 *  \fn nitf_SegmentMemorySource_construct
 *  \brief A class for reading segment data from memory (or VM)
 *
 *  The memory source class allows us to read directly from
 *  a data buffer.  In the event that this is a memory-mapped file,
 *  we will likely get a performance gain over the direct fd approach.
 *
 *  The constructor takes in a buffer, a size, and optionally
 *  a sampling factor (Typically, the factor will be applied most
 *  times during the case of memory mapping, although it may
 *  be used to sample down or cut the image into pieces).
 *
 *  Contains a static declaration of the specific implementation.
 *
 *  \param data The data
 *  \param size The size
 *  \param start The start offset
 *  \param byteSkip The number of bytes to skip (0 signifies contiguous read)
 *  \param copyData Whether or not to make a copy of the data.  If this is
 *  false, the data must outlive the memory source.
 */
NITFAPI(nitf_SegmentSource *) nitf_SegmentMemorySource_construct
(
    const char* data,
    nitf_Off size,
    nitf_Off start,
    int byteSkip,
    NITF_BOOL copyData,
    nitf_Error* error
);

/*!
 *  \fn nitf_SegmentFileSource_construct
 *  \brief Provides impl for reading segment data from an FD or HANDLE
 *
 *  The FileSource class is a DataSource that comes from an open
 *  file descriptor or handle.  Due to any number of constraints,
 *  we allow the creator to specify a start point, and a byte skip.
 *
 *  Construct a file source from a handle, a start point, and
 *  a pixel skip.
 *
 *  \param fname The file to use
 *  \param start The location to seek to (as the beginning)
 *  \param byteSkip The number of bytes to skip(0 signifies contiguous read)
 */
NITFAPI(nitf_SegmentSource *) nitf_SegmentFileSource_construct
(
    nitf_IOHandle handle,
    nitf_Off start,
    int byteSkip,
    nitf_Error * error
);


NITFAPI(nitf_SegmentSource *) nitf_SegmentFileSource_constructIO
(
    nitf_IOInterface* io,
    nitf_Off start,
    int byteSkip,
    nitf_Error * error
);

NITFAPI(nitf_SegmentSource *) nitf_SegmentReaderSource_construct
(
    nitf_SegmentReader *reader,
    nitf_Error * error
);


NITF_CXX_ENDGUARD

#endif
