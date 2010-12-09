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

#include "cgm/EllipseElement.h"

NITFPRIV(void) ellipseDestroy(NITF_DATA* data)
{
    if ( ((cgm_EllipseElement*)data)->attributes)
    {
        cgm_FillAttributes_destruct( & ((cgm_EllipseElement*)data)->attributes);
    }
    NITF_FREE( data );
}

NITFPRIV(cgm_Element*) ellipseClone(NITF_DATA* data, nitf_Error* error)
{
    cgm_EllipseElement *source = NULL, *dest = NULL;
    cgm_Element* element = NULL;
    assert(data);

    source = (cgm_EllipseElement*)data;
    element = cgm_EllipseElement_construct(error);

    if (!element)
        return NULL;
    dest = (cgm_EllipseElement*)element->data;

    dest->centerX = source->centerX;
    dest->centerY = source->centerY;
    dest->end1X = source->end1X;
    dest->end1Y = source->end1Y;
    dest->end2X = source->end2X;
    dest->end2Y = source->end2Y;
    
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

NITFPRIV(void) ellipsePrint(NITF_DATA* data)
{
    cgm_EllipseElement* ellipse = (cgm_EllipseElement*)data;
    if (ellipse->attributes)
        cgm_FillAttributes_print( ellipse->attributes );

    printf("\tOrigin (%d, %d)\n",
            ellipse->centerX, ellipse->centerY);
    printf("\tEnd Point 1 (%d, %d)\n",
            ellipse->end1X, ellipse->end1Y);
    printf("\tEnd Point 2 (%d, %d)\n",
            ellipse->end2X, ellipse->end2Y);
}

NITFAPI(cgm_Element*) cgm_EllipseElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(CGM_ELLIPSE_ELEMENT, error);
    if (element)
    {
        cgm_EllipseElement* ellipse = (cgm_EllipseElement*)
        NITF_MALLOC(sizeof(cgm_EllipseElement));
        if (!ellipse)
        {
            NITF_FREE(element);
            return NULL;
        }
        ellipse->attributes = NULL;
        
        ellipse->centerX = -1;
        ellipse->centerY = -1;
        ellipse->end1X = -1;
        ellipse->end1Y = -1;
        ellipse->end2X = -1;
        ellipse->end2Y = -1;
        
        element->data = (NITF_DATA*)ellipse;
    }
    element->print = &ellipsePrint;
    element->clone = &ellipseClone;
    element->destroy = &ellipseDestroy;

    return element;
}
