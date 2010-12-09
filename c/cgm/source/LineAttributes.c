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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "cgm/LineAttributes.h"


NITFAPI(cgm_LineAttributes*) 
    cgm_LineAttributes_construct(nitf_Error* error)
{
    cgm_LineAttributes* atts = 
        (cgm_LineAttributes*)NITF_MALLOC(sizeof(cgm_LineAttributes));
    if (!atts)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    atts->lineWidth = -1;
    atts->lineType = CGM_TYPE_NOT_SET;
    
    atts->lineColor = cgm_Color_construct(-1, -1, -1, error);
    if (!atts->lineColor)
    {
        cgm_LineAttributes_destruct(&atts);
        return NULL;
    }
    
    return atts;
}

NITFAPI(cgm_LineAttributes*) 
    cgm_LineAttributes_clone(cgm_LineAttributes* source, nitf_Error* error)
{
    cgm_LineAttributes* atts = cgm_LineAttributes_construct(error);
    if (!atts)
        return NULL;

    atts->lineWidth = source->lineWidth;
    atts->lineType = source->lineType;
    
    if (source->lineColor)
    {
        atts->lineColor->r = source->lineColor->r;
        atts->lineColor->g = source->lineColor->g;
        atts->lineColor->b = source->lineColor->b;
    }
    
    return atts;
}


NITFAPI(void) cgm_LineAttributes_destruct(cgm_LineAttributes** atts)
{
    if (*atts)
    {
        if ((*atts)->lineColor)
        {
            cgm_Color_destruct(&((*atts)->lineColor));
        }
        NITF_FREE( *atts );
        *atts = NULL;
    }
}

NITFAPI(void) cgm_LineAttributes_print(cgm_LineAttributes* atts)
{
    if (atts->lineColor)
    {
        printf("\tLine Color: ");
        cgm_Color_print(atts->lineColor);
    }
    printf("\tLine Width [%d]\n", atts->lineWidth);
    printf("\tLine Type: [%d]\n", atts->lineType);

}
