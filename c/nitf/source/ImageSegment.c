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

#include "nitf/ImageSegment.h"
/*
NITFPRIV(nitf_DecompressionInterface*) getDecompIface(const char* comp,
                              int* bad,
                              nitf_Error* error);

NITFPRIV(int) destroyDecompIface(nitf_DecompressionInterface** decompIface,
                 const char* comp,
                 nitf_Error* error);
*/

NITFAPI(nitf_ImageSegment *) nitf_ImageSegment_construct(nitf_Error *
        error)
{
    nitf_ImageSegment *segment =
        (nitf_ImageSegment *) NITF_MALLOC(sizeof(nitf_ImageSegment));

    if (!segment)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    /*  This object gets created NOW  */
    segment->subheader = NULL;

    /*  The image offset in the file  */
    segment->imageOffset = 0;
    /*  The image end  (offset + length image)  */
    segment->imageEnd = 0;
    segment->subheader = nitf_ImageSubheader_construct(error);
    if (!segment->subheader)
    {
        nitf_ImageSegment_destruct(&segment);
        return NULL;
    }
    /*  Yes!  We have a success  */
    return segment;
}


NITFAPI(nitf_ImageSegment *) nitf_ImageSegment_clone(nitf_ImageSegment *
        source,
        nitf_Error * error)
{
    nitf_ImageSegment *segment = NULL;

    if (source)
    {
        segment =
            (nitf_ImageSegment *) NITF_MALLOC(sizeof(nitf_ImageSegment));

        if (!segment)
        {
            nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                            NITF_CTXT, NITF_ERR_MEMORY);
            return NULL;
        }
        /* Just in case we self-destruct  */
        segment->subheader = NULL;

        /*  The image offset in the file  */
        segment->imageOffset = source->imageOffset;
        /*  The image end  (offset + length image)  */
        segment->imageEnd = source->imageEnd;
        segment->subheader =
            nitf_ImageSubheader_clone(source->subheader, error);
        if (!segment->subheader)
        {
            nitf_ImageSegment_destruct(&segment);
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


NITFAPI(void) nitf_ImageSegment_destruct(nitf_ImageSegment ** segment)
{
    if (*segment)
    {
        if ((*segment)->subheader)
        {
            /*  Destroy subheader info  */
            nitf_ImageSubheader_destruct(&(*segment)->subheader);
        }
    }
    NITF_FREE(*segment);
    *segment = NULL;
}

