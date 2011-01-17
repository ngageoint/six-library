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

#ifndef __NITF_GRAPHIC_SEGMENT_H__
#define __NITF_GRAPHIC_SEGMENT_H__

#include "nitf/System.h"
#include "nitf/GraphicSubheader.h"


NITF_CXX_GUARD

/*!
 *  \struct nitf_GraphicSegment
 *  \brief ...
 *
 *  The GraphicSegment object contains the sub-objects that
 *  ...
 */
typedef struct _nitf_GraphicSegment
{
    nitf_GraphicSubheader *subheader;
    nitf_Uint64 offset;
    nitf_Uint64 end;
    char *graphic;
}
nitf_GraphicSegment;


/*!
 *  Construct a new header.  This simply produces an empty
 *  subheader
 *
 *  \param error
 *  \return A graphic header
 */
NITFAPI(nitf_GraphicSegment *)
nitf_GraphicSegment_construct(nitf_Error * error);


/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_GraphicSegment *)
nitf_GraphicSegment_clone(nitf_GraphicSegment * source,
                          nitf_Error * error);

/*!
 *  Destruct a graphic subheader, and NULL-set it
 *  \param segment  A graphic segment
 */
NITFAPI(void) nitf_GraphicSegment_destruct(nitf_GraphicSegment ** segment);

NITF_CXX_ENDGUARD

#endif
