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

#include "nitf/TextSegment.h"

NITFAPI(nitf_TextSegment *) nitf_TextSegment_construct(nitf_Error * error)
{
    nitf_TextSegment *segment =
        (nitf_TextSegment *) NITF_MALLOC(sizeof(nitf_TextSegment));

    if (!segment)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    /*  The text offset in the file  */
    segment->offset = 0;
    /*  The text end  (offset + length text)  */
    segment->end = 0;

    /*  This object gets created NOW  */
    segment->subheader = NULL;
    segment->subheader = nitf_TextSubheader_construct(error);
    if (!segment->subheader)
    {
        nitf_TextSegment_destruct(&segment);
        return NULL;
    }
    /*  Yes!  We have a success  */
    return segment;
}


NITFAPI(nitf_TextSegment *) nitf_TextSegment_clone(nitf_TextSegment *
        source,
        nitf_Error * error)
{
    nitf_TextSegment *segment = NULL;

    if (source)
    {
        segment =
            (nitf_TextSegment *) NITF_MALLOC(sizeof(nitf_TextSegment));

        if (!segment)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }
        /*  The text offset in the file  */
        segment->offset = source->offset;
        /*  The text end  (offset + length text)  */
        segment->end = source->end;

        /* Just in case we self-destruct  */
        segment->subheader =
            nitf_TextSubheader_clone(source->subheader, error);
        if (!segment->subheader)
        {
            nitf_TextSegment_destruct(&segment);
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


NITFAPI(void) nitf_TextSegment_destruct(nitf_TextSegment ** segment)
{
    if (*segment)
    {
        if ((*segment)->subheader)
        {
            /*  Destroy subheader info  */
            nitf_TextSubheader_destruct(&(*segment)->subheader);
        }

        NITF_FREE(*segment);
        *segment = NULL;

    }
}
