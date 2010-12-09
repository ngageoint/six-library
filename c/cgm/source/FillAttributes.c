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

#include "cgm/FillAttributes.h"


NITFAPI(cgm_FillAttributes*) 
    cgm_FillAttributes_construct(nitf_Error* error)
{
    cgm_FillAttributes* atts = 
        (cgm_FillAttributes*)NITF_MALLOC(sizeof(cgm_FillAttributes));
    if (!atts)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    atts->fillColor = NULL;
    atts->edgeColor = NULL;
    
    atts->interiorStyle = CGM_IS_NOT_SET;
    atts->edgeVisibility = -1;
    atts->edgeWidth = -1;
    atts->edgeType = CGM_TYPE_NOT_SET;
    
    atts->fillColor = cgm_Color_construct(-1, -1, -1, error);
    if (!atts->fillColor)
    {
        cgm_FillAttributes_destruct(&atts);
        return NULL;
    }
    
    atts->edgeColor = cgm_Color_construct(-1, -1, -1, error);
    if (!atts->edgeColor)
    {
        cgm_FillAttributes_destruct(&atts);
        return NULL;
    }
    
    return atts;
}


NITFAPI(cgm_FillAttributes*) 
    cgm_FillAttributes_clone(cgm_FillAttributes* source, nitf_Error* error)
{
    cgm_FillAttributes* atts = cgm_FillAttributes_construct(error);
    if (!atts)
        return NULL;
    
    if (source->fillColor)
    {
        atts->fillColor->r = source->fillColor->r;
        atts->fillColor->g = source->fillColor->g;
        atts->fillColor->b = source->fillColor->b;
    }
    if (source->edgeColor)
    {
        atts->edgeColor->r = source->edgeColor->r;
        atts->edgeColor->g = source->edgeColor->g;
        atts->edgeColor->b = source->edgeColor->b;
    }

    atts->interiorStyle = source->interiorStyle;
    atts->edgeVisibility = source->edgeVisibility;
    atts->edgeWidth = source->edgeWidth;
    atts->edgeType = source->edgeType;

    return atts;
}


NITFAPI(void) cgm_FillAttributes_destruct(cgm_FillAttributes** atts)
{
    if (*atts)
    {
        if ((*atts)->fillColor)
        {
            cgm_Color_destruct(&((*atts)->fillColor));
        }
        if ((*atts)->edgeColor)
        {
            cgm_Color_destruct(&((*atts)->edgeColor));
        }
        NITF_FREE( *atts );
        *atts = NULL;
    }
}

NITFAPI(void) cgm_FillAttributes_print(cgm_FillAttributes* atts)
{
    if (atts->fillColor)
    {
        printf("\tFill Color: ");
        cgm_Color_print(atts->fillColor);
    }
    printf("\tInterior Style [%d]\n", atts->interiorStyle);
    printf("\tEdge Visibility [%d]\n", atts->edgeVisibility);
    printf("\tEdge Width [%d]\n", atts->edgeWidth);
    printf("\tEdge Type [%d]\n", atts->edgeType);
    if (atts->edgeColor)
    {
        printf("\tEdge Color: ");
        cgm_Color_print(atts->edgeColor);
    }
}
