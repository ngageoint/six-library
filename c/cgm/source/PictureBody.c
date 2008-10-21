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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cgm/PictureBody.h"

NITFAPI(cgm_PictureBody*) cgm_PictureBody_construct(nitf_Error* error)
{
    cgm_PictureBody* body =
        (cgm_PictureBody*)NITF_MALLOC(sizeof(cgm_PictureBody));

    if (!body)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                NITF_CTXT, NITF_ERR_MEMORY);
        return NULL;
    }

    body->auxColor = NULL;
    body->elements = NULL;
    
    body->elements = nitf_List_construct(error);
    if (!body->elements)
    {
        cgm_PictureBody_destruct(&body);
        return NULL;
    }
    
    body->auxColor = (short*)NITF_MALLOC(sizeof(short) * CGM_RGB);
    if (!body->auxColor)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
        cgm_PictureBody_destruct(&body);
        return NULL;
    }

    body->transparency = 1;
    body->auxColor[CGM_R] = -1;
    body->auxColor[CGM_G] = -1;
    body->auxColor[CGM_B] = -1;
    return body;
}

NITFAPI(void) cgm_PictureBody_destruct(cgm_PictureBody** body)
{
    if (*body)
    {
        if ( (*body)->elements )
        {
            nitf_List_destruct( & (*body)->elements );
        }
        if ( (*body)->auxColor )
        {
            NITF_FREE( (*body)->auxColor );
        }
        NITF_FREE( *body );
        *body = NULL;
    }
}
