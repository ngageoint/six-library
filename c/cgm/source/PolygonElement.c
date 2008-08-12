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

#include "cgm/PolygonElement.h"

NITFPRIV(void) polyDestroy(NITF_DATA* data)
{
    
    
    /* TODO!! */
    cgm_PolygonElement* poly = (cgm_PolygonElement*)data;
    nitf_ListIterator it, end;
    
    nitf_List* list = (nitf_List*)poly->vertices;
    it = nitf_List_begin(list);
    end = nitf_List_end(list);

    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
	cgm_Vertex* v = (cgm_Vertex*)nitf_ListIterator_get(&it);
	cgm_Vertex_destruct(&v);
	nitf_ListIterator_increment(&it);
    }
    
    nitf_List_destruct(&list);
    
    NITF_FREE(data);

}


NITFPRIV(void) polyPrint(NITF_DATA* data)
{
    
    cgm_PolygonElement* poly = (cgm_PolygonElement*)data;
    nitf_ListIterator it, end;
    
    nitf_List* list = (nitf_List*)poly->vertices;
    it = nitf_List_begin(list);
    end = nitf_List_end(list);


    printf("\tFill Color: (%d, %d, %d)\n", 
	   poly->fillColor[CGM_R],
	   poly->fillColor[CGM_G],
	   poly->fillColor[CGM_B]);
    printf("\tInterior Style [%d]\n", poly->interiorStyle);
    printf("\tEdge Visibility [%d]\n", poly->edgeVisibility);
    printf("\tEdge Width [%d]\n", poly->edgeWidth);
    printf("\tEdge Color: (%d, %d, %d)\n", 
	   poly->edgeColor[CGM_R],
	   poly->edgeColor[CGM_G],
	   poly->edgeColor[CGM_B]);

    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
	cgm_Vertex* v = (cgm_Vertex*)nitf_ListIterator_get(&it);
	printf("\t");
	cgm_Vertex_print(v);
	nitf_ListIterator_increment(&it);
    }

}


NITFAPI(cgm_Element*) cgm_PolygonElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
    if (element)
    {
	cgm_PolygonElement* poly = (cgm_PolygonElement*)
	    NITF_MALLOC(sizeof(cgm_PolygonElement));

	if (!poly)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
	poly->vertices = NULL;
	element->data = (NITF_DATA*)poly;

    }
    element->print = &polyPrint;
    element->destroy = &polyDestroy;
    
    return element;
    
}
