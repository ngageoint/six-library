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

#include "cgm/TextAttributes.h"


NITFAPI(cgm_TextAttributes*) 
    cgm_TextAttributes_construct(nitf_Error* error)
{
    cgm_TextAttributes* atts = 
        (cgm_TextAttributes*)NITF_MALLOC(sizeof(cgm_TextAttributes));
    if (!atts)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }
    atts->textColor = NULL;
    atts->characterOrientation = NULL;
    atts->characterHeight = 1;
    atts->textFontIndex = 1;
    
    atts->textColor = cgm_Color_construct(0, 0, 0, error);
    if (!atts->textColor)
    {
        cgm_TextAttributes_destruct(&atts);
        return NULL;
    }

    atts->characterOrientation = cgm_Rectangle_construct(error);
    if (!atts->characterOrientation)
    {
        cgm_TextAttributes_destruct( &atts );
        return NULL;
    }
    /* set some defaults */
    atts->characterOrientation->x1 = 0;
    atts->characterOrientation->y1 = 1;
    atts->characterOrientation->x2 = 1;
    atts->characterOrientation->y2 = 0;

    return atts;
}

NITFAPI(cgm_TextAttributes*) 
    cgm_TextAttributes_clone(cgm_TextAttributes* source, nitf_Error* error)
{
    cgm_TextAttributes* atts = cgm_TextAttributes_construct(error);
    if (!atts)
        return NULL;
    
    atts->characterHeight = source->characterHeight;
    atts->textFontIndex = source->textFontIndex;
    
    if (source->textColor)
    {
        atts->textColor->r = source->textColor->r;
        atts->textColor->g = source->textColor->g;
        atts->textColor->b = source->textColor->b;
    }
    
    if (source->characterOrientation)
    {
        atts->characterOrientation->x1 = source->characterOrientation->x1;
        atts->characterOrientation->y1 = source->characterOrientation->y1;
        atts->characterOrientation->x2 = source->characterOrientation->x2;
        atts->characterOrientation->y2 = source->characterOrientation->y2;
    }
    
    return atts;
}

NITFAPI(void) cgm_TextAttributes_destruct(cgm_TextAttributes** atts)
{
    if (*atts)
    {
        if ((*atts)->textColor)
        {
            cgm_Color_destruct( &((*atts)->textColor) );
        }
        if ((*atts)->characterOrientation)
        {
            cgm_Rectangle_destruct( &((*atts)->characterOrientation) );
        }
        NITF_FREE( *atts );
        *atts = NULL;
    }
}

NITFAPI(void) cgm_TextAttributes_print(cgm_TextAttributes* atts)
{

    if (atts->textColor)
    {
        printf("\tText Color: ");
        cgm_Color_print(atts->textColor);
    }
    printf("\tCharacter Height: %d\n", atts->characterHeight);
    printf("\tText Font Index: %d\n", atts->textFontIndex);

    printf("\tCharacter orientation: ");
    if (atts->characterOrientation)
        cgm_Rectangle_print(atts->characterOrientation);
}
