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

    atts->textColor[CGM_R] = -1;
    atts->textColor[CGM_G] = -1;
    atts->textColor[CGM_B] = -1;
    
    atts->characterHeight = -1;
    atts->textFontIndex = -1;
    
    atts->characterOrientation = cgm_Rectangle_construct(error);
    if (!atts->characterOrientation)
    {
        cgm_TextAttributes_destruct( &atts );
        return NULL;
        
    }

    return atts;
}

NITFAPI(void) cgm_TextAttributes_destruct(cgm_TextAttributes** atts)
{
    if (*atts)
    {
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

    printf("\tText Color: (%d, %d, %d)\n", 
	   atts->textColor[CGM_R],
	   atts->textColor[CGM_G],
	   atts->textColor[CGM_B]);
    printf("\tCharacter Height: %d\n", atts->characterHeight);
    printf("\tText Font Index: %d\n", atts->textFontIndex);

    printf("\tCharacter orientation: ");
    cgm_Rectangle_print(atts->characterOrientation);

}
