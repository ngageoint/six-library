/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __NITF_IMAGE_WRITER_H__
#define __NITF_IMAGE_WRITER_H__

#include "nitf/System.h"
#include "nitf/ImageSource.h"
#include "nitf/ImageSubheader.h"
#include "nitf/WriteHandler.h"

NITF_CXX_GUARD


typedef nitf_WriteHandler nitf_ImageWriter;

/*!
 * Constructs a new ImageWriter, using the passed-in subheader as the basis
 * for the values.
 */
NITFAPI(nitf_ImageWriter*)
    nitf_ImageWriter_construct(nitf_ImageSubheader *subheader,
                               nrt_HashTable* options,
                               nitf_Error * error);


/*!
 * Attach the given nitf_ImageSource to this ImageWriter
 * The ImageWriter obtains ownership of the passed imageSource, and will destruct
 * it when the ImageWriter is destructed.
 */
NITFAPI(NITF_BOOL) nitf_ImageWriter_attachSource(nitf_ImageWriter * writer,
                                                 nitf_ImageSource *imageSource,
                                                 nitf_Error * error);


/*!
 * \brief nitf_ImageWriter_setWriteCaching - Enable/disable cached writes
 *
 * nitf_ImageWriter_setWriteCaching enables/disables cached writes. Enabling
 * cached writes causes the system to accumulate full blocks of data prior to
 * writing. This is more efficent in terms of writing but requires more memory.
 *
 * For blocking modes, R, P, and B blocking modes, one block sized buffer is
 * required for each block column (number of blocks/row). For S mode one
 * block is required for each band for each block column, however for the
 * same iamge dimensions, pixel size and number of bands it amount to the
 * same storage since the blocks of the S mode image are smaller (each
 * contains only one band of data)
 *
 * \return Returns the current enable/disable state
 */
NITFAPI(int) nitf_ImageWriter_setWriteCaching
(
    nitf_ImageWriter * iWriter,     /*!< Object to modify */
    int enable                      /*!< Enable cached writes if true */
);

/*!
 * \brief nitf_ImageWriter_setDirectBlockWrite - Enable/disable direct block writing
 *
 * nitf_ImageWriter_setDirectBlockWrite enables/disables direct block writing.
 * If this is set to 1 and the number of image bands is 1, then each block of data
 * will be written directly to the NITF and bypass any manipulation or re-organization.
 * If you know for certain that you're band sources will give you the data formatted
 * precisely as required for whatever you're writing out, then enable this for better
 * write performance.  This is most useful in conjunction with the DirectBlockSource
 * band source for file copies.
 */
NITFAPI(void) nitf_ImageWriter_setDirectBlockWrite
(
    nitf_ImageWriter * iWriter,     /*!< Object to modify */
    int enable                      /*!< Enable cached writes if true */
);

/*!
 *  Function allows the user access to the product's pad pixels.
 *  For example, if you wanted transparent pixels for fill, you would
 *  set this function using arguments (writer, 0, 1, error)
 *
 *  \param writer  The image writer
 *  \param value   The pixel fill value
 *  \param length  The length in bytes of the pixel
 *  \param error   An error to populate if the function fails
 *  \return NITF_SUCCESS if function succeeded, NITF_FAILURE if function
 *  failed
 */
NITFAPI(NITF_BOOL) nitf_ImageWriter_setPadPixel(nitf_ImageWriter* imageWriter,
                                                nitf_Uint8* value,
                                                nitf_Uint32 length,
                                                nitf_Error* error);

NITF_CXX_ENDGUARD

#endif
