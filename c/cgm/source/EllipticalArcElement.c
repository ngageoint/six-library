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

#include "cgm/EllipticalArcElement.h"

NITFPRIV(void) ellipticalArcDestroy(NITF_DATA* data)
{
    if ( ((cgm_EllipticalArcElement*)data)->attributes)
    {
        cgm_LineAttributes_destruct( & ((cgm_EllipticalArcElement*)data)->attributes);
    }
    NITF_FREE( data );
}

NITFPRIV(cgm_Element*) ellipticalArcClone(NITF_DATA* data, nitf_Error* error)
{
    cgm_EllipticalArcElement *source = NULL, *dest = NULL;
    cgm_Element* element = NULL;
    assert(data);

    source = (cgm_EllipticalArcElement*)data;
    element = cgm_EllipticalArcElement_construct(error);

    if (!element)
        return NULL;
    dest = (cgm_EllipticalArcElement*)element->data;

    dest->centerX = source->centerX;
    dest->centerY = source->centerY;
    dest->end1X = source->end1X;
    dest->end1Y = source->end1Y;
    dest->end2X = source->end2X;
    dest->end2Y = source->end2Y;
    dest->startVectorX = source->startVectorX;
    dest->endVectorY = source->endVectorY;
    
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

NITFPRIV(void) ellipticalArcPrint(NITF_DATA* data)
{
    cgm_EllipticalArcElement* arc = (cgm_EllipticalArcElement*)data;
    if (arc->attributes)
        cgm_LineAttributes_print(arc->attributes);

    printf("\tOrigin (%d, %d)\n",
            arc->centerX, arc->centerY);
    printf("\tEnd Point 1 (%d, %d)\n",
            arc->end1X, arc->end1Y);
    printf("\tEnd Point 2 (%d, %d)\n",
            arc->end2X, arc->end2Y);
    printf("\tStart Vector (%d, %d)\n",
            arc->startVectorX, arc->startVectorY);
    printf("\tEnd Vector (%d, %d)\n",
            arc->endVectorX, arc->endVectorY);
}

NITFAPI(cgm_Element*) cgm_EllipticalArcElement_construct(nitf_Error* error)
{
    cgm_Element* element =
        cgm_Element_construct(CGM_ELLIPTICAL_ARC_CENTER_ELEMENT, error);
    if (element)
    {
        cgm_EllipticalArcElement* arc = (cgm_EllipticalArcElement*)
        NITF_MALLOC(sizeof(cgm_EllipticalArcElement));
        if (!arc)
        {
            NITF_FREE(element);
            return NULL;
        }
        arc->attributes = NULL;
        arc->centerX = -1;
        arc->centerY = -1;
        arc->end1X = -1;
        arc->end1Y = -1;
        arc->end2X = -1;
        arc->end2Y = -1;
        arc->startVectorX = -1;
        arc->endVectorY = -1;
        element->data = (NITF_DATA*)arc;
    }
    element->print = &ellipticalArcPrint;
    element->clone = &ellipticalArcClone;
    element->destroy = &ellipticalArcDestroy;

    return element;
}
