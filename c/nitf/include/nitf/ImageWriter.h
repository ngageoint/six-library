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

#ifndef __NITF_IMAGE_WRITER_H__
#define __NITF_IMAGE_WRITER_H__

#include "nitf/ImageIO.h"
#include "nitf/IOHandle.h"
#include "nitf/System.h"
#include "nitf/Error.h"
#include "nitf/ImageSource.h"

NITF_CXX_GUARD

/*!
 *  TODO: Add documentation
 */
typedef struct _nitf_ImageWriter
{
    nitf_IOHandle outputHandle;
    nitf_ImageSource *imageSource;
    nitf_ImageIO *imageBlocker;
}
nitf_ImageWriter;

/*!
 *  \fn nitf_ImageWriter_write
 *  \brief Method used by the actual Writer to write an image out.
 *
 *  This function writes out an image from a source, by pulling
 *  the required amounts from the source attached.  This function is
 *  probably only useable from the Writer.write() function.  The
 *  image source must be attached at this point, or an exception
 *  will occur.
 *
 *  \param imageWriter The image writer object
 *  \param numBitsPerPixel The number of bits per pixel in the image
 *  \param numImageBands  The number of bands in the image
 *  \param numMultispectralBands Number of multi-spectral image bands
 *  \param numRows The number of rows
 *  \param numCols The number of columns
 *  \param An error to return on failure
 *
 */
NITFPROT(NITF_BOOL) nitf_ImageWriter_write(nitf_ImageWriter * writer,
        int numBitsPerPixel,
        int numImageBands,
        int numMultispectralImageBands,
        int numRows,
        int numCols,
        nitf_Error * error);

/*!
 *  TODO: Add documentation
 */
NITFAPI(void) nitf_ImageWriter_destruct(nitf_ImageWriter ** writer);

/*!
 * Attach the given nitf_ImageSource to this ImageWriter
 * The ImageWriter obtains ownership of the passed imageSource, and will destruct
 * it when the ImageWriter is destructed.
 */
NITFAPI(NITF_BOOL) nitf_ImageWriter_attachSource(nitf_ImageWriter * writer,
    nitf_ImageSource *imageSource, nitf_Error * error);

/*!
  \brief nitf_ImageWriter_setWriteCaching - Enable/disable cached writes

  nitf_ImageWriter_setWriteCaching enables/disables cached writes. Enabling
  cached writes causes the system to accumulate full blocks of data prior to
  writing. This is more efficent in terms of writing but requires more memory.

  For blocking modes, R, P, and B blocking modes, one block sized buffer is
  required for each block column (number of blocks/row). For S mode one
  block is required for each band for each block column, however for the
  same iamge dimensions, pixel size and number of bands it amount to the
  same storage since the blocks of the S mode image are smaller (each
  contains only one band of data)

  \return Returns the current enable/disable state
*/

NITFPROT(int) nitf_ImageWriter_setWriteCaching
(
    nitf_ImageWriter * iWriter,     /*!< Object to modify */
    int enable                      /*!< Enable cached writes if true */
);

NITF_CXX_ENDGUARD

#endif
