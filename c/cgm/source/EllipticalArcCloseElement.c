#include "cgm/EllipticalArcCloseElement.h"


NITFPRIV(void) destroy(NITF_DATA* data)
{
    NITF_FREE( data );
}

NITFPRIV(void) print(NITF_DATA* data)
{
    cgm_EllipticalArcCloseElement* arc = (cgm_EllipticalArcCloseElement*)data;
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
	   arc->end1X, arc->end1Y);
    printf("\tEA P2(%d, %d)\n",
	   arc->end2X, arc->end2Y);
    printf("\tEA SV(%d, %d)\n",
	   arc->startVectorX, arc->startVectorY);
    printf("\tEA EV(%d, %d)\n",
	   arc->endVectorX, arc->endVectorY);
    

}

NITFAPI(cgm_Element*) cgm_EllipticalArcCloseElement_construct(nitf_Error* error)
{
    cgm_Element* element = 
        cgm_Element_construct(CGM_ELLIPTICAL_ARC_CENTER_CLOSE_ELEMENT,
                              error);
    if (element)
    {
	cgm_EllipticalArcCloseElement* arc = (cgm_EllipticalArcCloseElement*)
	    NITF_MALLOC(sizeof(cgm_EllipticalArcCloseElement));
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
