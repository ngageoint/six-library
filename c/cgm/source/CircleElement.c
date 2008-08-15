#include "cgm/CircleElement.h"

NITFPRIV(void) destroy(NITF_DATA* data)
{
    NITF_FREE( data );
}
NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_CircleElement* circle = (cgm_CircleElement*)data;
    printf("\tFill Color: (%d, %d, %d)\n", 
	   circle->fillColor[CGM_R],
	   circle->fillColor[CGM_G],
	   circle->fillColor[CGM_B]);
    printf("\tInterior Style [%d]\n", circle->interiorStyle);
    printf("\tEdge Visibility [%d]\n", circle->edgeVisibility);
    printf("\tEdge Width [%d]\n", circle->edgeWidth);
    printf("\tEdge Color: (%d, %d, %d)\n", 
	   circle->edgeColor[CGM_R],
	   circle->edgeColor[CGM_G],
	   circle->edgeColor[CGM_B]);
    printf("\tC O(%d, %d), R(%d)\n",
	   circle->centerX, circle->centerY, circle->radius);
    
}
NITFAPI(cgm_Element*) cgm_CircleElement_construct(nitf_Error* error)
{
    cgm_Element* element = cgm_Element_construct(error);
    if (element)
    {
	cgm_CircleElement* circle = (cgm_CircleElement*)
	    NITF_MALLOC(sizeof(cgm_CircleElement));
	if (!circle)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
	circle->fillColor[0] = -1;
	circle->fillColor[1] = -1;
	circle->fillColor[2] = -1;
	circle->interiorStyle = CGM_IS_NOT_SET;
	circle->edgeVisibility = -1;
	circle->edgeWidth = -1;
	circle->edgeType = CGM_TYPE_NOT_SET;
        
	circle->centerX = -1;
	circle->centerY = -1;
	circle->radius = -1;
	element->data = (NITF_DATA*)circle;
    }
    element->print = &print;
    element->destroy = &destroy;
    
    return element;
    
}
