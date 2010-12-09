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

#include "cgm/CircleElement.h"

NITFPRIV(void) destroyCircle(NITF_DATA* data)
{
    if ( ((cgm_CircleElement*)data)->attributes)
    {
        cgm_FillAttributes_destruct( & ((cgm_CircleElement*)data)->attributes);
    }
    NITF_FREE( data );
}

NITFPRIV(cgm_Element*) cloneCircle(NITF_DATA* data, nitf_Error* error)
{
    cgm_CircleElement *source = NULL, *dest = NULL;
    cgm_Element* element = NULL;
    assert(data);

    source = (cgm_CircleElement*)data;
    element = cgm_CircleElement_construct(error);

    if (!element)
        return NULL;
    dest = (cgm_CircleElement*)element->data;

    dest->centerX = source->centerX;
    dest->centerY = source->centerY;
    dest->radius = source->radius;
    
    if (source->attributes)
    {
        dest->attributes = cgm_FillAttributes_clone(source->attributes, error);
        if (!dest->attributes)
        {
            cgm_Element_destruct(&element);
            return NULL;
        }
    }
    
    return element;
}

NITFPRIV(void) printCircle(NITF_DATA* data)
{
    cgm_CircleElement* circle = (cgm_CircleElement*)data;
    if (circle->attributes)
        cgm_FillAttributes_print(circle->attributes);
    printf("\tOrigin: (%d, %d), Radius: [%d]\n",
            circle->centerX, circle->centerY, circle->radius);

}
NITFAPI(cgm_Element*) cgm_CircleElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(CGM_CIRCLE_ELEMENT, error);
    if (element)
    {
        cgm_CircleElement* circle = (cgm_CircleElement*)
        NITF_MALLOC(sizeof(cgm_CircleElement));
        if (!circle)
        {
            NITF_FREE(element);
            return NULL;
        }
        circle->attributes = NULL;
        circle->centerX = -1;
        circle->centerY = -1;
        circle->radius = -1;
        element->data = (NITF_DATA*)circle;
    }
    element->print = &printCircle;
    element->destroy = &destroyCircle;
    element->clone = &cloneCircle;

    return element;
}
