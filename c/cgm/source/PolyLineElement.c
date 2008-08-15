#include "cgm/PolyLineElement.h"



NITFPRIV(void) polyDestroy(NITF_DATA* data)
{
    
    
    /* TODO!! */
    cgm_PolyLineElement* poly = (cgm_PolyLineElement*)data;
    nitf_ListIterator it, end;
    
    nitf_List* list = (nitf_List*)poly->vertices;

    if (poly->attributes)
        cgm_LineAttributes_destruct( &(poly->attributes) );

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
    
    cgm_PolyLineElement* poly = (cgm_PolyLineElement*)data;
    nitf_ListIterator it, end;
    nitf_List* list = (nitf_List*)poly->vertices;
    
    if (poly->attributes)
    {
        cgm_LineAttributes_print( poly->attributes );
    }

    it = nitf_List_begin(list);
    end = nitf_List_end(list);

    while (nitf_ListIterator_notEqualTo(&it, &end))
    {
	cgm_Vertex* v = (cgm_Vertex*)nitf_ListIterator_get(&it);
	printf("\t");
	cgm_Vertex_print(v);
	nitf_ListIterator_increment(&it);
    }

}


NITFAPI(cgm_Element*) cgm_PolyLineElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(CGM_POLYLINE_ELEMENT, error);
    if (element)
    {
	cgm_PolyLineElement* poly = (cgm_PolyLineElement*)
	    NITF_MALLOC(sizeof(cgm_PolyLineElement));

	if (!poly)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
        poly->attributes = NULL;
	poly->vertices = NULL;
	element->data = (NITF_DATA*)poly;
    }
    element->print = &polyPrint;
    element->destroy = &polyDestroy;
    
    return element;
    
}
