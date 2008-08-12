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

#include "cgm/TextElement.h"

NITFPRIV(void) textPrint(NITF_DATA* data)
{
    cgm_TextElement* text = (cgm_TextElement*)data;
    
    printf("\tText Color: (%d, %d, %d)\n", 
	   text->color[CGM_R],
	   text->color[CGM_G],
	   text->color[CGM_B]);
    printf("\tCharacter Height: %d\n", text->characterHeight);
    printf("\tText Font Index: %d\n", text->textFontIndex);

    printf("\tCharacter orientation: ");
    cgm_Rectangle_print(text->characterOrientation);
    printf("\tText: ");
    cgm_Text_print(text->text);

}
NITFPRIV(void) textDestroy(NITF_DATA* data)
{
    
    
    /* TODO!! */
    cgm_TextElement* text = (cgm_TextElement*)data;

    if (text->characterOrientation)
	cgm_Rectangle_destruct( &(text->characterOrientation) );
    
    if (text->text)
    {
	cgm_Text_destruct( &(text->text) );
    }

    NITF_FREE(data);

}

NITFAPI(cgm_Element*) cgm_TextElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
    if (element)
    {
	cgm_TextElement* text = (cgm_TextElement*)
	    NITF_MALLOC(sizeof(cgm_TextElement));
	if (!text)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
	/* TODO: This default arg in constructor doesnt appear to be 
	   that helpful.  Remove?  Otherwise, might want to make consistent
	 */
	text->text = cgm_Text_construct(NULL, error);
	if (!text->text)
	{
	    NITF_FREE( text );
	    NITF_FREE( element );
	    return NULL;
	      
	}

	text->characterOrientation = cgm_Rectangle_construct(error);
	if (!text->characterOrientation)
	{
	    cgm_Text_destruct( &(text->text) );
	    NITF_FREE( text );
	    NITF_FREE( element );
	    return NULL;
	      
	}

	text->color[CGM_R] = -1;
	text->color[CGM_G] = -1;
	text->color[CGM_B] = -1;

	text->characterHeight = -1;
	text->textFontIndex = -1;
	
	element->data = (NITF_DATA*)text;

    }

    element->print = &textPrint;
    element->destroy = &textDestroy;
	    
    return element;

}


