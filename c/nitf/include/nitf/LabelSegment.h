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

#ifndef __NITF_LABEL_SEGMENT_H__
#define __NITF_LABEL_SEGMENT_H__

#include "nitf/System.h"
#include "nitf/LabelSubheader.h"


NITF_CXX_GUARD

/*!
 *  \struct nitf_LabelSegment
 *  \brief ...
 *
 *  The LabelSegment object contains the sub-objects that
 *  ...
 */
typedef struct _nitf_LabelSegment
{
    nitf_LabelSubheader *subheader;
    nitf_Uint64 offset;
    nitf_Uint64 end;
    char *label;
}
nitf_LabelSegment;


/*!
 *  Construct a new header.  This simply produces an empty
 *  subheader
 *
 *  \param error
 *  \return A label header
 */
NITFAPI(nitf_LabelSegment *)
nitf_LabelSegment_construct(nitf_Error * error);


/*!
 *  Clone this object.  This is a deep copy operation.
 *
 *  \param source The source object
 *  \param error  An error to populate upon failure
 *  \return A new object that is identical to the old
 */
NITFAPI(nitf_LabelSegment *)
nitf_LabelSegment_clone(nitf_LabelSegment * source, nitf_Error * error);

/*!
 *  Destruct a label subheader, and NULL-set it
 *  \param segment  A label segment
 */
NITFAPI(void) nitf_LabelSegment_destruct(nitf_LabelSegment ** segment);

NITF_CXX_ENDGUARD

#endif
