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

#ifndef __NITF_IMAGE_SEGMENT_H__
#define __NITF_IMAGE_SEGMENT_H__

#include "nitf/System.h"
#include "nitf/ImageSource.h"
#include "nitf/ImageSubheader.h"
#include "nitf/ImageIO.h"


NITF_CXX_GUARD

/*!
 *  \struct nitf_ImageSegment
 *  \brief Basically, a pair, subheader and io routines
 *
 *  The ImageSegment object contains the sub-objects that
 *  make up an image, and
 */
typedef struct _nitf_ImageSegment
{
    nitf_ImageSubheader *subheader;
    nitf_Uint64 imageOffset;    /* Make these nitf_Off's */
    nitf_Uint64 imageEnd;       /* Make these nitf_Off's */
    /*nitf_ImageSource *imageSource;*/
}
nitf_ImageSegment;


/*!
 *  Construct a new header.  This simply produces an empty
 *  subheader, and later, allocates an imageIO
 *
 *  \param error
 *  \return An image segment or NULL
 */
NITFAPI(nitf_ImageSegment *)
nitf_ImageSegment_construct(nitf_Error * error);


/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_ImageSegment *)
nitf_ImageSegment_clone(nitf_ImageSegment * source, nitf_Error * error);

/*!
 *  Destruct an image subheader, and NULL-set it
 *  \param segment  An image segment
 */
NITFAPI(void) nitf_ImageSegment_destruct(nitf_ImageSegment ** segment);

NITF_CXX_ENDGUARD

#endif
