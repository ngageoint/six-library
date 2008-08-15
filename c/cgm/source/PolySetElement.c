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
	poly->vertices = NULL;
	element->data = (NITF_DATA*)poly;
    }
    element->print = &polyPrint;
    element->destroy = &polyDestroy;
    
    return element;
    
}
