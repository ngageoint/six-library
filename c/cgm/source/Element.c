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

#include "cgm/Element.h"

NITFAPI(cgm_Element*) cgm_Element_construct(cgm_ElementType type,
        nitf_Error* error)
{
    cgm_Element* element = (cgm_Element*)NITF_MALLOC(sizeof(cgm_Element));
    if (!element)
    {
        nitf_Error_init(error,
                NITF_STRERROR( NITF_ERRNO ),
                NITF_CTXT,
                NITF_ERR_MEMORY);
        return NULL;
    }
    element->type = type;
    element->print = NULL;
    element->destroy = NULL;
    element->data = NULL;
    return element;

}

NITFAPI(cgm_Element*) cgm_Element_clone(cgm_Element* source, nitf_Error* error)
{
    return source->clone(source->data, error);
}

NITFAPI(void) cgm_Element_destruct(cgm_Element** element)
{
    if (*element)
    {
        if ( (*element)->data )
        (*element)->destroy( (*element)->data );
        NITF_FREE( (*element) );
    }
    *element = NULL;
}

NITFAPI(void) cgm_Element_print(cgm_Element* element)
{
    if (element->print && element->data)
    {
        printf("Element: [%s]\n", cgm_Element_getName(element));
        (* element->print )(element->data);
    }
}

#define TRY_NAME(X, Y) if (X == Y) return #X

NITFAPI(const char*) cgm_Element_getName(cgm_Element* element)
{
    static char* dontKnow = "No idea";
    cgm_ElementType t = element->type;
    TRY_NAME(CGM_TEXT_ELEMENT, t);
    TRY_NAME(CGM_POLYGON_ELEMENT, t);
    TRY_NAME(CGM_POLYLINE_ELEMENT, t);
    TRY_NAME(CGM_POLYSET_ELEMENT, t);
    TRY_NAME(CGM_ELLIPSE_ELEMENT, t);
    TRY_NAME(CGM_ELLIPTICAL_ARC_CENTER_ELEMENT, t);
    TRY_NAME(CGM_ELLIPTICAL_ARC_CENTER_CLOSE_ELEMENT, t);
    TRY_NAME(CGM_RECTANGLE_ELEMENT, t);
    TRY_NAME(CGM_CIRCLE_ELEMENT, t);
    TRY_NAME(CGM_CIRCULAR_ARC_CENTER_ELEMENT, t);
    TRY_NAME(CGM_CIRCULAR_ARC_CENTER_CLOSE_ELEMENT, t);
    return dontKnow;
}
