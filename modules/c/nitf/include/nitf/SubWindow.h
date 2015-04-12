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

#ifndef __NITF_SUB_WINDOW_H__
#define __NITF_SUB_WINDOW_H__

#include "nitf/System.h"
#include "nitf/DownSampler.h"


NITF_CXX_GUARD

/*!
 * \struct nitf_SubWindow
 * \brief Sub-window specification structure
 *
 * The \b nitf_SubWindow specifies a sub-window. This object is used as an
 * argument to image read and write requests to specify the data requested.
 *
 * The \em bandList field specifies the desired bands. Bands are numbered
 * starting with zero and ordered as the bands appear physically in the file.
 * The user must supply one buffer for each requested band. Each buffer must
 * have enough storage for one band of the requested subimage size.
 *
 * The downsampler determines what downsampling (if any) is performed on
 * this sub-window request.  There is no default downsampler.  If you just
 * want to perform pixel skipping, call setDownSampler() with a
 * PixelSkip object.
 */
typedef struct _nitf_SubWindow
{
    nitf_Uint32 startRow;       /*!< Start row */
    nitf_Uint32 startCol;       /*!< Start column */

    nitf_Uint32 numRows;        /*!< Number of rows to read */
    nitf_Uint32 numCols;        /*!< Number of columns to read */

    nitf_Uint32 *bandList;      /*!< List of bands to read */
    nitf_Uint32 numBands;       /*!< Number of bands to read */

    nitf_DownSampler *downsampler;      /* A downsampler, if any  */

}
nitf_SubWindow;


/*!
 *  \fn nitf_SubWindow_construct(error)
 *
 *  This method constructs a new sub-window.  The sub-window defaults
 *  to all zeroes (meaning that the user must assign values to each
 *  field).  The downsampler is a method for sub-sampling, if necessary.
 *  This defaults to a NULL (or NO downsampler necessary).
 *
 *  \param error The error to populate on failure
 *  \return Returns a sub-window object on success, and NULL on failure
 */
NITFAPI(nitf_SubWindow *) nitf_SubWindow_construct(nitf_Error * error);

/*!
 *  \fn nitf_SubWindow_destruct(subWindow)
 *
 *  Destroys a sub-window.  This method will NOT destroy your
 *  downsampler, since you may want to reuse it for multiple objects.
 *  It also does not destroy the band list since how the list was created
 *  (i.e., NITF_MALLOC v static) is unknown.
 */
NITFAPI(void) nitf_SubWindow_destruct(nitf_SubWindow ** subWindow);


/*!
 *  \fn nitf_SubWindow_setDownSampler(subWindow, downsampler, error)
 *
 *  Bind a downsampler to the sub-window.
 *  \param subWindow The object
 *  \param downsampler A downsampler to bind to the object
 *  \param error An error to populate on failure
 *  \return NITF_SUCCESS on success, NITF_FAILURE on failure.
 */

NITFAPI(NITF_BOOL) nitf_SubWindow_setDownSampler(nitf_SubWindow *
        subWindow,
        nitf_DownSampler *
        downsampler,
        nitf_Error * error);

NITF_CXX_ENDGUARD

#endif
