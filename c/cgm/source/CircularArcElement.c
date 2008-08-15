#include "cgm/CircularArcElement.h"


NITFPRIV(void) destroy(NITF_DATA* data)
{
    NITF_FREE( data );
}

NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_CircularArcElement* arc = (cgm_CircularArcElement*)data;
    printf("\tLine Type [%d]\n", arc->lineType);
    printf("\tLine Width [%d]\n", arc->lineWidth);
    printf("\tLine Color: (%d, %d, %d)\n", 
	   arc->lineColor[CGM_R],
	   arc->lineColor[CGM_G],
	   arc->lineColor[CGM_B]);
    printf("\tCA O(%d, %d)\n",
	   arc->centerX, arc->centerY);
    printf("\tCA P1(%d, %d)\n",
	   arc->startX, arc->startY);
    printf("\tCA P2(%d, %d)\n",
	   arc->endX, arc->endY);
    printf("\tCA R(%d)\n",
	   arc->radius);
    
}

NITFAPI(cgm_Element*) cgm_CircularArcElement_construct(nitf_Error* error)
{
    cgm_Element* element = 
        cgm_Element_construct(CGM_CIRCULAR_ARC_CENTER_ELEMENT, 
                              error);
    if (element)
    {
	cgm_CircularArcElement* arc = (cgm_CircularArcElement*)
	    NITF_MALLOC(sizeof(cgm_CircularArcElement));
	if (!arc)
	{
	    NITF_FREE(element);
	    return NULL;
	    
	}
	arc->lineWidth = -1;
	arc->lineType = CGM_TYPE_NOT_SET;
	arc->lineColor[CGM_R] = -1;
	arc->lineColor[CGM_G] = -1;
	arc->lineColor[CGM_B] = -1;
        
	arc->centerX = -1;
	arc->centerY = -1;
	arc->startX = -1;
	arc->startY = -1;
	arc->endX = -1;
	arc->endY = -1;
	arc->radius = -1;
	element->data = (NITF_DATA*)arc;
        
    }
    element->print = &print;
    element->destroy = &destroy;
    
    return element;
    
}
