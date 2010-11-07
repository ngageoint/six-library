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

#include "cgm/PolySetElement.h"

NITFPRIV(void) polyDestroy(NITF_DATA* data)
{
    /* TODO!! */
    cgm_PolySetElement* poly = (cgm_PolySetElement*)data;
    nitf_ListIterator it, end;

    nitf_List* list = (nitf_List*)poly->vertices;

    if (poly->attributes)
    {
        cgm_FillAttributes_destruct( &(poly->attributes) );
    }

    it = nitf_List_begin(list);
    end = nitf_List_end(list);

    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        cgm_VertexClose* v = (cgm_VertexClose*)nitf_ListIterator_get(&it);
        cgm_VertexClose_destruct(&v);
        nitf_ListIterator_increment(&it);
    }

    nitf_List_destruct(&list);

    NITF_FREE(data);
}

NITFPRIV(cgm_Element*) polyClone(NITF_DATA* data, nitf_Error* error)
{
    cgm_PolySetElement *source = NULL, *dest = NULL;
    cgm_Element* element = NULL;
    assert(data);

    source = (cgm_PolySetElement*)data;
    element = cgm_PolySetElement_construct(error);

    if (!element)
        return NULL;
    dest = (cgm_PolySetElement*)element->data;

    if (source->attributes)
    {
        dest->attributes = cgm_FillAttributes_clone(source->attributes, error);
        if (!dest->attributes)
        {
            cgm_Element_destruct(&element);
            return NULL;
        }
    }
    
    /* TODO vertices */

    return element;
}

NITFPRIV(void) polyPrint(NITF_DATA* data)
{
    cgm_PolySetElement* poly = (cgm_PolySetElement*)data;
    nitf_ListIterator it, end;

    nitf_List* list = (nitf_List*)poly->vertices;
    it = nitf_List_begin(list);
    end = nitf_List_end(list);

    if (poly->attributes)
        cgm_FillAttributes_print(poly->attributes);

    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
        cgm_VertexClose* v = (cgm_VertexClose*)nitf_ListIterator_get(&it);
        printf("\t");
        cgm_VertexClose_print(v);
        nitf_ListIterator_increment(&it);
    }
}

NITFAPI(cgm_Element*) cgm_PolySetElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(CGM_POLYSET_ELEMENT, error);
    if (element)
    {
        cgm_PolySetElement* poly = (cgm_PolySetElement*)
        NITF_MALLOC(sizeof(cgm_PolySetElement));

        if (!poly)
        {
            NITF_FREE(element);
            return NULL;

        }
        poly->attributes = NULL;
        poly->vertices = nitf_List_construct(error);
        if (!poly->vertices)
        {
            NITF_FREE(poly);
            NITF_FREE(element);
            return NULL;
        }

        element->data = (NITF_DATA*)poly;
    }
    element->print = &polyPrint;
    element->clone = &polyClone;
    element->destroy = &polyDestroy;

    return element;
}
