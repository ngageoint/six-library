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

#include "cgm/CircularArcElement.h"

NITFPRIV(void) circularArcDestroy(NITF_DATA* data)
{
    if ( ((cgm_CircularArcElement*)data)->attributes)
    {
        cgm_LineAttributes_destruct( & ((cgm_CircularArcElement*)data)->attributes);
    }
    NITF_FREE( data );
}

NITFPRIV(cgm_Element*) circularArcClone(NITF_DATA* data, nitf_Error* error)
{
    cgm_CircularArcElement *source = NULL, *dest = NULL;
    cgm_Element* element = NULL;
    assert(data);

    source = (cgm_CircularArcElement*)data;
    element = cgm_CircularArcElement_construct(error);

    if (!element)
        return NULL;
    dest = (cgm_CircularArcElement*)element->data;

    dest->centerX = source->centerX;
    dest->centerY = source->centerY;
    dest->startX = source->startX;
    dest->startY = source->startY;
    dest->endX = source->endX;
    dest->endY = source->endY;
    dest->radius = source->radius;
    
    if (source->attributes)
    {
        dest->attributes = cgm_LineAttributes_clone(source->attributes, error);
        if (!dest->attributes)
        {
            cgm_Element_destruct(&element);
            return NULL;
        }
    }

    return element;
}

NITFPRIV(void) circularArcPrint(NITF_DATA* data)
{
    cgm_CircularArcElement* arc = (cgm_CircularArcElement*)data;

    if (arc->attributes)
        cgm_LineAttributes_print( arc->attributes );

    printf("\tOrigin (%d, %d)\n",
            arc->centerX, arc->centerY);
    printf("\tPoint 1(%d, %d)\n",
            arc->startX, arc->startY);
    printf("\tPoint 2(%d, %d)\n",
            arc->endX, arc->endY);
    printf("\tRadius [%d]\n",
            arc->radius);
}

NITFAPI(cgm_Element*) cgm_CircularArcElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(
            CGM_CIRCULAR_ARC_CENTER_ELEMENT, error);
    if (element)
    {
        cgm_CircularArcElement* arc = (cgm_CircularArcElement*)
        NITF_MALLOC(sizeof(cgm_CircularArcElement));
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
        element->data = (NITF_DATA*)arc;

    }
    element->print = &circularArcPrint;
    element->clone = &circularArcClone;
    element->destroy = &circularArcDestroy;

    return element;
}
