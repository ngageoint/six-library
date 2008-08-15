#include "cgm/PolySetElement.h"



NITFPRIV(void) polyDestroy(NITF_DATA* data)
{
    
    
    /* TODO!! */
    cgm_PolySetElement* poly = (cgm_PolySetElement*)data;
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
    
    cgm_PolySetElement* poly = (cgm_PolySetElement*)data;
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
    printf("\tEdge Type [%d]\n", poly->edgeType);
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


NITFAPI(cgm_Element*) cgm_PolySetElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
    if (element)
    {
	cgm_PolySetElement* poly = (cgm_PolySetElement*)
	    NITF_MALLOC(sizeof(cgm_PolySetElement));

	if (!poly)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}

	poly->fillColor[CGM_R] = -1;
	poly->fillColor[CGM_G] = -1;
	poly->fillColor[CGM_B] = -1;
	poly->interiorStyle = CGM_IS_NOT_SET;
	poly->edgeVisibility = -1;
	poly->edgeWidth = -1;
	poly->edgeType = CGM_TYPE_NOT_SET;

	poly->edgeWidth = -1;
	poly->edgeType = CGM_TYPE_NOT_SET;
	poly->edgeColor[CGM_R] = -1;
	poly->edgeColor[CGM_G] = -1;
	poly->edgeColor[CGM_B] = -1;
	poly->vertices = NULL;
	element->data = (NITF_DATA*)poly;
    }
    element->print = &polyPrint;
    element->destroy = &polyDestroy;
    
    return element;
    
}
