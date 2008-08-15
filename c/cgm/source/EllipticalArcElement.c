#include "cgm/EllipticalArcElement.h"


NITFPRIV(void) destroy(NITF_DATA* data)
{
    NITF_FREE( data );
}

NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_EllipticalArcElement* arc = (cgm_EllipticalArcElement*)data;
    printf("\tLine Type [%d]\n", arc->lineType);
    printf("\tLine Width [%d]\n", arc->lineWidth);
    printf("\tLine Color: (%d, %d, %d)\n", 
	   arc->lineColor[CGM_R],
	   arc->lineColor[CGM_G],
	   arc->lineColor[CGM_B]);
    printf("\tEA O(%d, %d)\n",
	   arc->centerX, arc->centerY);
    printf("\tEA P1(%d, %d)\n",
	   arc->end1X, arc->end1Y);
    printf("\tEA P2(%d, %d)\n",
	   arc->end2X, arc->end2Y);
    printf("\tEA SV(%d, %d)\n",
	   arc->startVectorX, arc->startVectorY);
    printf("\tEA EV(%d, %d)\n",
	   arc->endVectorX, arc->endVectorY);
    

}

NITFAPI(cgm_Element*) cgm_EllipticalArcElement_construct(nitf_Error* error)
{
    cgm_Element* element = 
        cgm_Element_construct(CGM_ELLIPTICAL_ARC_CENTER_ELEMENT,
                              error);
    if (element)
    {
	cgm_EllipticalArcElement* arc = (cgm_EllipticalArcElement*)
	    NITF_MALLOC(sizeof(cgm_EllipticalArcElement));
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
	arc->end1X = -1;
	arc->end1Y = -1;
	arc->end2X = -1;
	arc->end2Y = -1;
	arc->startVectorX = -1;
	arc->endVectorY = -1;
	element->data = (NITF_DATA*)arc;
        
    }
    element->print = &print;
    element->destroy = &destroy;
    
    return element;
    
}
