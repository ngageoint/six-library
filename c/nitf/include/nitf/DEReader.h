/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#ifndef __NITF_DE_READER_H__
#define __NITF_DE_READER_H__

#include "nitf/IOHandle.h"
#include "nitf/System.h"
#include "nitf/Error.h"
#include "nitf/DESegment.h"
#include "nitf/DESubheader.h"
#include "nitf/UserSegment.h"

NITF_CXX_GUARD

/*!
  \brief nitf_DEReader - Reader object for reading Data Extension Segments

  The nitf_DEReader object provides support for reading DES data. A simple
  byte stream oriented model is provided that presents the data as a self-
  contained, flat file of bytes. Seeking within the virtual file is permitted.

  For DES with compressed data, the virtual file is the uncompressed data and
  decompression is handled by the DES handler. Although seeking is allowed
  there could be a severe performance penalty in some decompression cases.

  Access to DES specific methods for reading and possibly decoding the data
  are provided by a plugin that is keyed by DES segment ID.

  The DE reader uses the IOHandle supplied (and retained) by the Reader
  during the nitf_Reader_read call which must precede the use of the
  DE reader. The user must not use this IOHandle during the read of the
  DE data. The file position of this handle is maintained by the DE reader
  and it may not move in a way that is consistent with the movement of
  the pointer in the virtual file.

  The constructor for this object is a method of the nitf_Reader object. The
  method is:
                nitf_Reader_newDEReader
*/

typedef struct _nitf_DEReader
{
    nitf_IOHandle inputHandle;   /*!< I/O handle for reads */
    nitf_UserSegment *user;      /*!< User segment information */

}
nitf_DEReader;


/*!
  \brief nitf_DEReader_read - Read DE data

  The nitf_DEReader_read function reads data from the associated DE segment.
  The reading of the data is serial as if a flat file were being read by an
  input stream. Except for the first argument, this function has the same
  calling sequence and behavior as IOHandle_read.

  \return TRUE is returned on success. On error, the error object
  is set.
*/

NITFAPI(NITF_BOOL) nitf_DEReader_read
(
    nitf_DEReader *deReader,                  /*!< Associated DEReader */
    NITF_DATA *buffer,                    /*!< Buffer to hold data */
    size_t count,                         /*!< Amount of data to return */
    nitf_Error *error                     /*!< For error returns */
);

/*!
  \brief nitf_DEReader_seek - Seek in DE data

  The nitf_DEReader_seek function allows the user to seek within the extension
  data. Except for the first argument, this function has the same calling
  sequence and behaivior as IOHandle_seek. The offset is relative to the top
  of the DES data.

  The returned value maybe tested with NITF_IO_SUCCESS()

  \return The offset from the beginning to the current position
  is set.
*/

NITFAPI(off_t) nitf_DEReader_seek
(
    nitf_DEReader *deReader,         /*!< Associated DEReader */
    off_t offset,                /*!< The seek offset */
    int whence,                  /*!< Starting at (SEEK_SET, SEEK_CUR, SEEK_END)*/
    nitf_Error *error            /*!< For error returns */
);

/*!
  \brief nitf_DEReader_tell - Tell location in DE data

  The nitf_DEReader_tell function allows the user to determine the current
  offset within the extension data. Except for the first argument, this
  function has the same calling sequence and behaivior as IOHandle_tell. The
  offset is relative to the top of the DES data.

  The returned value maybe tested with NITF_IO_SUCCESS()

  \return The offset from the beginning to the current position
  is set.
*/

NITFAPI(off_t) nitf_DEReader_tell
(
    nitf_DEReader *deReader,          /*!< Associated DEReader */
    nitf_Error *error             /*!< For error returns */
);

/*!
  \brief nitf_DEReader_getSize - Determine size of the data

  The nitf_DEReader_seek function allows the user to determine the size of
  the data. Except for the first argument, this function has the same calling
  sequence and behavior as IOHandle_getSize.

  The returned value maybe tested with NITF_IO_SUCCESS()

  \return The offset from the beginning to the current position
  is set.
*/

NITFAPI(off_t) nitf_DEReader_getSize
(
    nitf_DEReader *deReader,          /*!< Associated DEReader */
    nitf_Error *error             /*!< For error returns */
);

/*!
  \brief nitf_DEReader_destruct - Destructor for the DEReader object

  nitf_DEReader_destruct is the destructor for the DEReader object. This
  function does not close the associated file handle.

  \return None
*/

NITFAPI(void) nitf_DEReader_destruct
(
    nitf_DEReader **deReader              /*!< Associated DEReader */
);

NITF_CXX_ENDGUARD

#endif
