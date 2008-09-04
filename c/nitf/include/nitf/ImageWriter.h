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

#include "nitf/System.h"
#include "nitf/Error.h"
#include "nitf/ImageSource.h"
#include "nitf/WriteHandler.h"
#include "nitf/ImageSubheader.h"

NITF_CXX_GUARD


typedef nitf_WriteHandler nitf_ImageWriter;

/*!
 * Constructs a new ImageWriter, using the passed-in subheader as the basis
 * for the values.
 */
NITFAPI(nitf_ImageWriter*) nitf_ImageWriter_construct(
        nitf_ImageSubheader *subheader, nitf_Error * error);


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
