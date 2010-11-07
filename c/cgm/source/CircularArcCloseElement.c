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

#include "cgm/CircularArcCloseElement.h"


NITFPRIV(void) destroyCircularArcClose(NITF_DATA* data)
{
    if ( ((cgm_CircularArcCloseElement*)data)->attributes)
    {
        cgm_FillAttributes_destruct( & ((cgm_CircularArcCloseElement*)data)->attributes);
    }
    NITF_FREE( data );
}

NITFPRIV(cgm_Element*) cloneCircularArcClose(NITF_DATA* data, nitf_Error* error)
{
    cgm_CircularArcCloseElement *source = NULL, *dest = NULL;
    cgm_Element* element = NULL;
    assert(data);
    
    source = (cgm_CircularArcCloseElement*)data;
    element = cgm_CircularArcCloseElement_construct(error);
    
    if (!element)
        return NULL;
    dest = (cgm_CircularArcCloseElement*)element->data;
    
    dest->centerX = source->centerX;
    dest->centerY = source->centerY;
    dest->startX = source->startX;
    dest->startY = source->startY;
    dest->endX = source->endX;
    dest->endY = source->endY;
    dest->radius = source->radius;
    dest->closeType = source->closeType;
    
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

NITFPRIV(void) printCircularArcClose(NITF_DATA* data)
{
    cgm_CircularArcCloseElement* arc = (cgm_CircularArcCloseElement*)data;

    if (arc->attributes)
        cgm_FillAttributes_print( arc->attributes );
    printf("\tOrigin (%d, %d)\n",
            arc->centerX, arc->centerY);
    printf("\tStart (%d, %d)\n",
            arc->startX, arc->startY);
    printf("\tEnd (%d, %d)\n",
            arc->endX, arc->endY);
    printf("\tRadius [%d]\n",
            arc->radius);
    printf("\tClose Type (%s)\n",
            arc->closeType == CGM_CLOSE_TYPE_PIE ?
                    "pie": "chord");
}

NITFAPI(cgm_Element*) cgm_CircularArcCloseElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(
            CGM_CIRCULAR_ARC_CENTER_CLOSE_ELEMENT, error);
    if (element)
    {
        cgm_CircularArcCloseElement* arc = (cgm_CircularArcCloseElement*)
        NITF_MALLOC(sizeof(cgm_CircularArcCloseElement));
        if (!arc)
        {
            NITF_FREE(element);
            return NULL;
        }
        arc->attributes = NULL;
        arc->centerX = -1;
        arc->centerY = -1;
        arc->startX = -1;
        arc->startY = -1;
        arc->endX = -1;
        arc->endY = -1;
        arc->radius = -1;
        arc->closeType = CGM_CLOSE_TYPE_PIE;
        element->data = (NITF_DATA*)arc;
    }
    element->print = &printCircularArcClose;
    element->destroy = &destroyCircularArcClose;
    element->clone = &cloneCircularArcClose;

    return element;
}
