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

#include "cgm/RectangleElement.h"

NITFPRIV(void) rectangleDestroy(NITF_DATA* data)
{
    cgm_RectangleElement* rect = (cgm_RectangleElement*)data;

    if (rect->attributes)
    {
        cgm_FillAttributes_destruct( &(rect->attributes) );
    }
    if (rect->rectangle)
    {
        cgm_Rectangle_destruct( & (rect->rectangle) );
    }
    NITF_FREE(data);
}

NITFPRIV(cgm_Element*) rectangleClone(NITF_DATA* data, nitf_Error* error)
{
    cgm_RectangleElement *source = NULL, *dest = NULL;
    cgm_Element* element = NULL;
    assert(data);

    source = (cgm_RectangleElement*)data;
    element = cgm_RectangleElement_construct(error);

    if (!element)
        return NULL;
    dest = (cgm_RectangleElement*)element->data;

    if (source->attributes)
    {
        dest->attributes = cgm_FillAttributes_clone(source->attributes, error);
        if (!dest->attributes)
        {
            cgm_Element_destruct(&element);
            return NULL;
        }
    }
    
    if (source->rectangle)
    {
        dest->rectangle = cgm_Rectangle_clone(source->rectangle, error);
        if (!dest->rectangle)
        {
            cgm_Element_destruct(&element);
            return NULL;
        }
    }
    
    return element;
}

NITFPRIV(void) rectanglePrint(NITF_DATA* data)
{
    cgm_RectangleElement* rect = (cgm_RectangleElement*)data;
    if (rect->attributes)
        cgm_FillAttributes_print( rect->attributes );
    printf("\t");
    if (rect->rectangle)
        cgm_Rectangle_print(rect->rectangle);

}

NITFAPI(cgm_Element*) cgm_RectangleElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(CGM_RECTANGLE_ELEMENT, error);
    if (element)
    {
        cgm_RectangleElement* rect = (cgm_RectangleElement*)
        NITF_MALLOC(sizeof(cgm_RectangleElement));
        if (!rect)
        {
            NITF_FREE(element);
            return NULL;
        }

        rect->attributes = NULL;
        rect->rectangle = NULL;

        element->data = (NITF_DATA*)rect;
    }

    element->print = &rectanglePrint;
    element->clone = &rectangleClone;
    element->destroy = &rectangleDestroy;

    return element;
}
