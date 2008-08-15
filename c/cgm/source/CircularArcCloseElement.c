#include "cgm/CircularArcCloseElement.h"


NITFPRIV(void) destroy(NITF_DATA* data)
{
    NITF_FREE( data );
}

NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_CircularArcCloseElement* arc = 
        (cgm_CircularArcCloseElement*)data;
    printf("\tFill Color: (%d, %d, %d)\n", 
	   arc->fillColor[CGM_R],
	   arc->fillColor[CGM_G],
	   arc->fillColor[CGM_B]);
    printf("\tInterior Style [%d]\n", arc->interiorStyle);
    printf("\tEdge Visibility [%d]\n", arc->edgeVisibility);
    printf("\tEdge Width [%d]\n", arc->edgeWidth);
    printf("\tEdge Color: (%d, %d, %d)\n", 
	   arc->edgeColor[CGM_R],
	   arc->edgeColor[CGM_G],
	   arc->edgeColor[CGM_B]);
    printf("\tEA O(%d, %d)\n",
	   arc->centerX, arc->centerY);
    printf("\tEA P1(%d, %d)\n",
	   arc->startX, arc->startY);
    printf("\tEA P2(%d, %d)\n",
	   arc->endX, arc->endY);
    printf("\tEA R(%d)\n",
	   arc->radius);
    printf("\tEA Close(%d)\n",
	   arc->closeType);
    

}

NITFAPI(cgm_Element*) cgm_CircularArcCloseElement_construct(nitf_Error* error)
{
    cgm_Element* element = 
        cgm_Element_construct(CGM_CIRCULAR_ARC_CENTER_CLOSE_ELEMENT,
                              error);
    if (element)
    {
	cgm_CircularArcCloseElement* arc = (cgm_CircularArcCloseElement*)
	    NITF_MALLOC(sizeof(cgm_CircularArcCloseElement));
	if (!arc)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}

	arc->fillColor[0] = -1;
	arc->fillColor[1] = -1;
	arc->fillColor[2] = -1;
	arc->interiorStyle = CGM_IS_NOT_SET;
	arc->edgeVisibility = -1;
	arc->edgeWidth = -1;
	arc->edgeType = CGM_TYPE_NOT_SET;
        
	arc->centerX = -1;
	arc->centerY = -1;
	arc->startX = -1;
	arc->startY = -1;
	arc->endX = -1;
	arc->endY = -1;
	arc->radius = -1;
	arc->closeType = -1;
	element->data = (NITF_DATA*)arc;
        
    }
    element->print = &print;
    element->destroy = &destroy;
    
    return element;
    
}
