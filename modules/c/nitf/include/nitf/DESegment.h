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

#ifndef __NITF_DE_SEGMENT_H__
#define __NITF_DE_SEGMENT_H__

#include "nitf/System.h"
#include "nitf/DESubheader.h"


NITF_CXX_GUARD

/*!
 *  \struct nitf_DESegment
 *  \brief ...
 *
 *  The DESegment object contains the sub-objects that
 *  make up a DES
 */
typedef struct _nitf_DESegment
{
    nitf_DESubheader *subheader;
    nitf_Uint64 offset;
    nitf_Uint64 end;
}
nitf_DESegment;


/*!
 *  Construct a new (empty) segment.
 *
 *  \param error
 *  \return a DES
 */
NITFAPI(nitf_DESegment *) nitf_DESegment_construct(nitf_Error * error);


/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_DESegment *)
nitf_DESegment_clone(nitf_DESegment * source, nitf_Error * error);

/*!
 *  Destruct an de subheader, and NULL-set it
 *  \param segment  A DE segment
 */
NITFAPI(void) nitf_DESegment_destruct(nitf_DESegment ** segment);

NITF_CXX_ENDGUARD

#endif
