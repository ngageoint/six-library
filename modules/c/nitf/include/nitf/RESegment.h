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

#ifndef __NITF_RE_SEGMENT_H__
#define __NITF_RE_SEGMENT_H__

#include "nitf/System.h"
#include "nitf/RESubheader.h"


NITF_CXX_GUARD

/*!
 *  \struct nitf_RESegment
 *  \brief ...
 *
 *  The RESegment object contains the sub-objects that
 *  make up a RES
 */
typedef struct _nitf_RESegment
{
    nitf_RESubheader *subheader;
    nitf_Uint64 offset;
    nitf_Uint64 end;
    char *data;
}
nitf_RESegment;


/*!
 *  Construct a new (empty) segment.
 *
 *  \param error
 *  \return a RES
 */
NITFAPI(nitf_RESegment *) nitf_RESegment_construct(nitf_Error * error);


/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_RESegment *)
nitf_RESegment_clone(nitf_RESegment * source, nitf_Error * error);

/*!
 *  Destruct an re subheader, and NULL-set it
 *  \param segment  A RE segment
 */
NITFAPI(void) nitf_RESegment_destruct(nitf_RESegment ** segment);

NITF_CXX_ENDGUARD

#endif
