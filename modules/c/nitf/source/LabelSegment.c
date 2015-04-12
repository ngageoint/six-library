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

#include "nitf/LabelSegment.h"

NITFAPI(nitf_LabelSegment *) nitf_LabelSegment_construct(nitf_Error *
        error)
{
    nitf_LabelSegment *segment =
        (nitf_LabelSegment *) NITF_MALLOC(sizeof(nitf_LabelSegment));

    if (!segment)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    /*  The label offset in the file  */
    segment->offset = 0;
    /*  The label end  (offset + length label)  */
    segment->end = 0;

    /*  This object gets created NOW  */
    segment->subheader = NULL;
    segment->subheader = nitf_LabelSubheader_construct(error);
    if (!segment->subheader)
    {
        nitf_LabelSegment_destruct(&segment);
        return NULL;
    }
    /*  Yes!  We have a success  */
    return segment;
}


NITFAPI(nitf_LabelSegment *) nitf_LabelSegment_clone(nitf_LabelSegment *
        source,
        nitf_Error * error)
{
    nitf_LabelSegment *segment = NULL;

    if (source)
    {
        segment =
            (nitf_LabelSegment *) NITF_MALLOC(sizeof(nitf_LabelSegment));

        if (!segment)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }
        /*  The label offset in the file  */
        segment->offset = source->offset;
        /*  The label end  (offset + length label)  */
        segment->end = source->end;

        /* Just in case we self-destruct  */
        segment->subheader = NULL;
        segment->subheader =
            nitf_LabelSubheader_clone(source->subheader, error);
        if (!segment->subheader)
        {
            nitf_LabelSegment_destruct(&segment);
            return NULL;
        }
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Trying to clone NULL pointer");
    }
    /*  Yes!  We have a success  */
    return segment;
}


NITFAPI(void) nitf_LabelSegment_destruct(nitf_LabelSegment ** segment)
{
    if (*segment)
    {
        if ((*segment)->subheader)
        {
            /*  Destroy subheader info  */
            nitf_LabelSubheader_destruct(&(*segment)->subheader);
        }
    }
    NITF_FREE(*segment);
    *segment = NULL;
}
