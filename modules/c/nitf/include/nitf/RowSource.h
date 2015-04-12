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

/*
  \file RowSource - Band source implementation for data produced in rows

  The RowSource object is derived from the DataSource object and is used for
  algorithms that generate data in rows. The RowSource object provides a
  wrapper to integrate a row oriented algorithm into the Writer source model.

  An example is the generation of the magnitude of a complex function. If the
  intended input image is very large and memory constraints prevent reading
  the entire image into memory, this simple algorithm could be implemented by
  reading the input in groups of full rows (strips), transforming each strip
  and then writing the rows to the output file.

  The algorithm must be organized into an object that allows generation of
  individual rows. In the example, the algorthm object would have to have a
  a setup function that initializes whatever state is required to begin
  processing rows. A "nextRow" method is required as well as a clean-up
  function. The "nextRow" function specifies the band and the algorithm
  object and must be able to provide the row data one band at a time. The
  caller is responsible for the making the set-up and clean-up calls and the
  object must handle input processing. The object may make the following two
  assumptions (only):

       Requests to the object are sequential, no seeks
       Requests to the object are in units of whole rows
       Bands from the same image source are read in parallel

*/

#ifndef __NITF_ROW_SOURCE_H__
#define __NITF_ROW_SOURCE_H__

#include "nitf/BandSource.h"

NITF_CXX_GUARD

/*!
  \brief NITF_ROW_SOURCE_NEXT_ROW - Next row function prototype

  \param algorithm - The algorithm object
  \param band      - The requested band
  \param buffer    - The buffer to receive the data
  \param error     - Error object for error returns

  \return Returns true on success. On failure the error object is set
*/
typedef NITF_BOOL(*NITF_ROW_SOURCE_NEXT_ROW) (void *algorithm,
        nitf_Uint32 band,
        NITF_DATA * buffer,
        nitf_Error * error);

/*
  \brief nitf_RowSource_construct - Constructor for the RowSource object

  The constructor for the row source object. This object maintains state
  information and cannot be reused for a second write.

  \return The new object or NULL on error. On error, the error object is
  set
*/
NITFAPI(nitf_BandSource *) nitf_RowSource_construct
(
    void *algorithm,                    /*!< The algorithm object */
    NITF_ROW_SOURCE_NEXT_ROW nextRow,   /*!< Pointer to the next row function */
    nitf_Uint32 band,                   /*!< Associate output band */
    nitf_Uint32 numRows,                /*!< Number of rows */
    nitf_Uint32 rowLength,              /*!< Length of each row in bytes (single band) */
    nitf_Error * error                  /*!< For error returns */
);

NITF_CXX_ENDGUARD
#endif
