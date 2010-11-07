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

#include "cgm/TextElement.h"

NITFPRIV(void) textPrint(NITF_DATA* data)
{
    cgm_TextElement* text = (cgm_TextElement*)data;

    if (text->attributes)
        cgm_TextAttributes_print( text->attributes );
    printf("\tText: ");
    if (text->text)
        cgm_Text_print(text->text);
}

NITFPRIV(void) textDestroy(NITF_DATA* data)
{
    /* TODO!! */
    cgm_TextElement* text = (cgm_TextElement*)data;

    if (text->attributes)
        cgm_TextAttributes_destruct( &(text->attributes) );

    if (text->text)
    {
        cgm_Text_destruct( &(text->text) );
    }
    NITF_FREE(data);
}

NITFPRIV(cgm_Element*) textClone(NITF_DATA* data, nitf_Error* error)
{
    cgm_TextElement *source = NULL, *dest = NULL;
    cgm_Element* element = NULL;
    assert(data);

    source = (cgm_TextElement*)data;
    element = cgm_TextElement_construct(error);

    if (!element)
        return NULL;
    dest = (cgm_TextElement*)element->data;

    if (source->attributes)
    {
        dest->attributes = cgm_TextAttributes_clone(source->attributes, error);
        if (!dest->attributes)
        {
            cgm_Element_destruct(&element);
            return NULL;
        }
    }
    
    if (source->text)
    {
        dest->text = cgm_Text_clone(source->text, error);
        if (!dest->text)
        {
            cgm_Element_destruct(&element);
            return NULL;
        }
    }

    return element;
}

NITFAPI(cgm_Element*) cgm_TextElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(CGM_TEXT_ELEMENT, error);
    if (element)
    {
        cgm_TextElement* text = (cgm_TextElement*)
        NITF_MALLOC(sizeof(cgm_TextElement));
        if (!text)
        {
            NITF_FREE(element);
            return NULL;

        }
        text->attributes = NULL;
        text->text = NULL;
        element->data = (NITF_DATA*)text;
    }

    element->print = &textPrint;
    element->clone = &textClone;
    element->destroy = &textDestroy;

    return element;
}

