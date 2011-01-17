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

#ifndef __NITF_SEGMENT_READER_H__
#define __NITF_SEGMENT_READER_H__

#include "nitf/System.h"

NITF_CXX_GUARD


/*!
  \brief nitf_SegmentReader - Reader object for reading Segments

  The nitf_SegmentReader object provides support for reading segment data. A simple
  byte stream oriented model is provided that presents the data as a self-
  contained, flat file of bytes. Seeking within the virtual file is permitted.

  The segment reader uses the IOHandle supplied (and retained) by the Reader
  during the nitf_Reader_read call which must precede the use of the
  segment reader. The user must not use this IOHandle during the read of the
  data. The file position of this handle is maintained by the segment reader
  and it may not move in a way that is consistent with the movement of
  the pointer in the virtual file.

  The constructor for this object is a method of the nitf_Reader object. The
  methods available are:
                nitf_Reader_newTextReader
                nitf_Reader_newGraphicReader


  \param input The input interface
  \param dataLength The length of the user data
  \param baseOffset Offset to the start of data
  \param virtualOffset current offset (within this region)
*/

typedef struct _nitf_SegmentReader
{
    nitf_IOInterface* input;
    nitf_Uint32 dataLength;
    nitf_Uint64 baseOffset;
    nitf_Uint64 virtualOffset;
}
nitf_SegmentReader;




/*!
  \brief nitf_SegmentReader_read - Read segment data

  The nitf_SegmentReader_read function reads data from the associated segment.
  The reading of the data is serial as if a flat file were being read by an
  input stream. Except for the first argument, this function has the same
  calling sequence and behavior as IOHandle_read.

  \return TRUE is returned on success. On error, the error object
  is set.
*/

NITFAPI(NITF_BOOL) nitf_SegmentReader_read
(
    nitf_SegmentReader *segmentReader,    /*!< Associated SegmentReader */
    NITF_DATA *buffer,                    /*!< Buffer to hold data */
    size_t count,                         /*!< Amount of data to return */
    nitf_Error *error                     /*!< For error returns */
);

/*!
  \brief nitf_SegmentReader_seek - Seek in segment data

  The nitf_SegmentReader_seek function allows the user to seek within the extension
  data. Except for the first argument, this function has the same calling
  sequence and behaivior as IOHandle_seek. The offset is relative to the top
  of the segment data.

  The returned value maybe tested with NITF_IO_SUCCESS()

  \return The offset from the beginning to the current position
  is set.
*/

NITFAPI(nitf_Off) nitf_SegmentReader_seek
(
    nitf_SegmentReader *segmentReader, /*!< Associated SegmentReader */
    nitf_Off offset,                /*!< The seek offset */
    int whence,                  /*!< Starting at (SEEK_SET, SEEK_CUR, SEEK_END)*/
    nitf_Error *error            /*!< For error returns */
);

/*!
  \brief nitf_SegmentReader_tell - Tell location in segment data

  The nitf_SegmentReader_tell function allows the user to determine the current
  offset within the extension data. Except for the first argument, this
  function has the same calling sequence and behaivior as IOHandle_tell. The
  offset is relative to the top of the segment data.

  The returned value maybe tested with NITF_IO_SUCCESS()

  \return The offset from the beginning to the current position
  is set.
*/

NITFAPI(nitf_Off) nitf_SegmentReader_tell
(
    nitf_SegmentReader *segmentReader,  /*!< Associated SegmentReader */
    nitf_Error *error                   /*!< For error returns */
);

/*!
  \brief nitf_SegmentReader_getSize - Determine size of the data

  The nitf_SegmentReader_seek function allows the user to determine the size of
  the data. Except for the first argument, this function has the same calling
  sequence and behavior as IOHandle_getSize.

  The returned value maybe tested with NITF_IO_SUCCESS()

  \return The offset from the beginning to the current position
  is set.
*/

NITFAPI(nitf_Off) nitf_SegmentReader_getSize
(
    nitf_SegmentReader *segmentReader,  /*!< Associated SegmentReader */
    nitf_Error *error                   /*!< For error returns */
);

/*!
  \brief nitf_SegmentReader_destruct - Destructor for the SegmentReader object

  nitf_SegmentReader_destruct is the destructor for the SegmentReader object.
  This function does not close the associated file handle.

  \return None
*/

NITFAPI(void) nitf_SegmentReader_destruct
(
    nitf_SegmentReader **segmentReader    /*!< Associated SegmentReader */
);

NITF_CXX_ENDGUARD

#endif
